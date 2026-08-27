#include "uart.h"

#include <stddef.h>

#include <avr/interrupt.h>

#include "bit_utils.h"
#include "uart_private.h"
#include "uart_registers.h"

#ifndef F_CPU
/* The build should define F_CPU (e.g. via the avr_options target in
 * CMake). This fallback only exists so the file still compiles stand-alone. */
#define F_CPU 8000000UL
#endif

/* -------------------------------------------------------------------------- */
/* Callback Dispatch                                                           */
/* -------------------------------------------------------------------------- */

static uart_callback_t uart_callbacks[UART_COUNT][UART_INTERRUPT_COUNT];

static void UART_Dispatch(uart_id_t uart, uart_interrupt_t interrupt)
{
    uart_callback_t callback = uart_callbacks[uart][interrupt];

    if (callback != NULL)
    {
        callback();
    }
}

void UART_RegisterCallback(
    uart_id_t uart,
    uart_interrupt_t interrupt,
    uart_callback_t callback)
{
    if ((uart >= UART_COUNT) || (interrupt >= UART_INTERRUPT_COUNT))
    {
        return;
    }

    uart_callbacks[uart][interrupt] = callback;
}

/* -------------------------------------------------------------------------- */
/* Baud Rate                                                                   */
/* -------------------------------------------------------------------------- */

/* Tries both normal and double-speed (U2X) UBRR values and keeps whichever
 * gets closer to the requested baud rate - same "try each hardware option,
 * measure the actual result, keep the best" approach used for PWM's
 * prescaler choice and the timer service's tick calculation elsewhere in
 * this project. */
static uint16_t UART_ComputeBaud(uint32_t baud_rate, uint8_t *out_u2x)
{
    uint32_t raw_normal;
    uint32_t raw_double;
    uint32_t ubrr_normal;
    uint32_t ubrr_double;
    uint32_t actual_normal;
    uint32_t actual_double;
    uint32_t diff_normal;
    uint32_t diff_double;

    if (baud_rate == 0U)
    {
        *out_u2x = 0U;
        return 0U;
    }

    raw_normal = F_CPU / (16UL * baud_rate);
    raw_double = F_CPU / (8UL * baud_rate);

    ubrr_normal = (raw_normal >= 1U) ? (raw_normal - 1U) : 0U;
    ubrr_double = (raw_double >= 1U) ? (raw_double - 1U) : 0U;

    actual_normal = (raw_normal >= 1U) ? (F_CPU / (16UL * (ubrr_normal + 1UL))) : 0U;
    actual_double = (raw_double >= 1U) ? (F_CPU / (8UL * (ubrr_double + 1UL))) : 0U;

    diff_normal = (raw_normal < 1U) ? 0xFFFFFFFFUL
        : ((actual_normal > baud_rate) ? (actual_normal - baud_rate) : (baud_rate - actual_normal));

    diff_double = (raw_double < 1U) ? 0xFFFFFFFFUL
        : ((actual_double > baud_rate) ? (actual_double - baud_rate) : (baud_rate - actual_double));

    if (diff_double < diff_normal)
    {
        *out_u2x = 1U;
        return (uint16_t)ubrr_double;
    }

    *out_u2x = 0U;
    return (uint16_t)ubrr_normal;
}

/* -------------------------------------------------------------------------- */
/* Configuration                                                               */
/* -------------------------------------------------------------------------- */

void UART_Init(const uart_config_t *config)
{
    const uart_descriptor_t *descriptor;
    uint16_t ubrr;
    uint8_t u2x;
    uint8_t control_c_value;

    if (config == NULL)
    {
        return;
    }

    descriptor = UART_RegistersGet(config->uart);

    if ((descriptor == NULL) ||
        (config->data_bits >= UART_DATA_BITS_COUNT) ||
        (config->parity >= UART_PARITY_COUNT) ||
        (config->stop_bits >= UART_STOP_BITS_COUNT))
    {
        return;
    }

    ubrr = UART_ComputeBaud(config->baud_rate, &u2x);

    UART_WriteBaudHigh(descriptor, (uint8_t)(ubrr >> 8));
    *descriptor->baud_low = (uint8_t)(ubrr & 0xFFU);

    if (u2x != 0U)
    {
        SET_BIT(*descriptor->control_a, descriptor->u2x_bit);
    }
    else
    {
        CLEAR_BIT(*descriptor->control_a, descriptor->u2x_bit);
    }

    /* control_c (UCSRC) can't be reliably read back on this chip - reading
     * that address always returns UBRRH's contents instead (see
     * uart_registers.c) - so the whole frame-format byte is composed here
     * as a fresh local value and written in one shot via
     * UART_WriteControlC, rather than incrementally modified with
     * SET_BIT/CLEAR_BIT the way UCSRA/UCSRB are. UMSEL is left at 0
     * (asynchronous mode) - this driver doesn't support synchronous USART
     * mode. */
    control_c_value = (uint8_t)(descriptor->parity_value[config->parity] << descriptor->parity_shift);
    control_c_value |= (uint8_t)(descriptor->stop_bits_value[config->stop_bits] << descriptor->stop_bits_bit);
    control_c_value |= (uint8_t)(descriptor->data_bits_value_c[config->data_bits] << descriptor->data_bits_c_shift);

    UART_WriteControlC(descriptor, control_c_value);

    /* UCSZ2 lives in UCSRB, not UCSRC - UCSRB reads back reliably, so
     * SET_BIT/CLEAR_BIT are fine here. */
    if (descriptor->data_bits_value_b[config->data_bits] != 0U)
    {
        SET_BIT(*descriptor->control_b, descriptor->data_bits_b_bit);
    }
    else
    {
        CLEAR_BIT(*descriptor->control_b, descriptor->data_bits_b_bit);
    }
}

/* -------------------------------------------------------------------------- */
/* Enable / Disable                                                            */
/* -------------------------------------------------------------------------- */

void UART_EnableTx(uart_id_t uart)
{
    const uart_descriptor_t *descriptor = UART_RegistersGet(uart);

    if (descriptor == NULL)
    {
        return;
    }

    SET_BIT(*descriptor->control_b, descriptor->tx_enable_bit);
}

void UART_DisableTx(uart_id_t uart)
{
    const uart_descriptor_t *descriptor = UART_RegistersGet(uart);

    if (descriptor == NULL)
    {
        return;
    }

    CLEAR_BIT(*descriptor->control_b, descriptor->tx_enable_bit);
}

void UART_EnableRx(uart_id_t uart)
{
    const uart_descriptor_t *descriptor = UART_RegistersGet(uart);

    if (descriptor == NULL)
    {
        return;
    }

    SET_BIT(*descriptor->control_b, descriptor->rx_enable_bit);
}

void UART_DisableRx(uart_id_t uart)
{
    const uart_descriptor_t *descriptor = UART_RegistersGet(uart);

    if (descriptor == NULL)
    {
        return;
    }

    CLEAR_BIT(*descriptor->control_b, descriptor->rx_enable_bit);
}

/* -------------------------------------------------------------------------- */
/* Raw Data Access                                                             */
/* -------------------------------------------------------------------------- */

void UART_WriteByte(uart_id_t uart, uint8_t data)
{
    const uart_descriptor_t *descriptor = UART_RegistersGet(uart);

    if (descriptor == NULL)
    {
        return;
    }

    *descriptor->data_reg = data;
}

uint8_t UART_ReadByte(uart_id_t uart)
{
    const uart_descriptor_t *descriptor = UART_RegistersGet(uart);

    if (descriptor == NULL)
    {
        return 0U;
    }

    return *descriptor->data_reg;
}

uint8_t UART_TransmitReady(uart_id_t uart)
{
    const uart_descriptor_t *descriptor = UART_RegistersGet(uart);

    if (descriptor == NULL)
    {
        return 0U;
    }

    return (uint8_t)READ_BIT(*descriptor->control_a, descriptor->flag_bit[UART_INTERRUPT_DATA_EMPTY]);
}

uint8_t UART_DataAvailable(uart_id_t uart)
{
    const uart_descriptor_t *descriptor = UART_RegistersGet(uart);

    if (descriptor == NULL)
    {
        return 0U;
    }

    return (uint8_t)READ_BIT(*descriptor->control_a, descriptor->flag_bit[UART_INTERRUPT_RX_COMPLETE]);
}

/* -------------------------------------------------------------------------- */
/* Interrupts                                                                  */
/* -------------------------------------------------------------------------- */

void UART_EnableInterrupt(uart_id_t uart, uart_interrupt_t interrupt)
{
    const uart_descriptor_t *descriptor = UART_RegistersGet(uart);
    uint8_t bit;

    if ((descriptor == NULL) || (interrupt >= UART_INTERRUPT_COUNT))
    {
        return;
    }

    bit = descriptor->interrupt_enable_bit[interrupt];

    if (bit == UART_UNUSED_BIT)
    {
        return;
    }

    SET_BIT(*descriptor->control_b, bit);
}

void UART_DisableInterrupt(uart_id_t uart, uart_interrupt_t interrupt)
{
    const uart_descriptor_t *descriptor = UART_RegistersGet(uart);
    uint8_t bit;

    if ((descriptor == NULL) || (interrupt >= UART_INTERRUPT_COUNT))
    {
        return;
    }

    bit = descriptor->interrupt_enable_bit[interrupt];

    if (bit == UART_UNUSED_BIT)
    {
        return;
    }

    CLEAR_BIT(*descriptor->control_b, bit);
}

void UART_ClearFlag(uart_id_t uart, uart_interrupt_t interrupt)
{
    const uart_descriptor_t *descriptor = UART_RegistersGet(uart);

    if ((descriptor == NULL) || (interrupt >= UART_INTERRUPT_COUNT))
    {
        return;
    }

    if (descriptor->flag_clearable[interrupt] == 0U)
    {
        /* RXC clears itself when UDR is read, UDRE clears itself when UDR
         * is written - there's nothing to do here for those. */
        return;
    }

    /* Unlike GIFR/TIFR (pure flag registers, where a raw single-bit write
     * is used specifically to avoid disturbing sibling flags), UCSRA mixes
     * TXC (write-1-to-clear) with read-only status bits (RXC/UDRE/FE/DOR/PE
     * - writes to these are ignored by hardware) and genuine read/write
     * config bits (U2X, MPCM). A normal read-modify-write (SET_BIT) is
     * actually what's correct here: it preserves U2X/MPCM, and writing
     * back whatever was read for the read-only bits has no effect either
     * way. A raw single-bit write, like TIMER_ClearFlag/EXT_INT_ClearFlag
     * use, would silently zero U2X here if it happened to be set. */
    SET_BIT(*descriptor->control_a, descriptor->flag_bit[interrupt]);
}

uint8_t UART_GetFlag(uart_id_t uart, uart_interrupt_t interrupt)
{
    const uart_descriptor_t *descriptor = UART_RegistersGet(uart);

    if ((descriptor == NULL) || (interrupt >= UART_INTERRUPT_COUNT))
    {
        return 0U;
    }

    return (uint8_t)READ_BIT(*descriptor->control_a, descriptor->flag_bit[interrupt]);
}

/* -------------------------------------------------------------------------- */
/* Errors                                                                      */
/* -------------------------------------------------------------------------- */

uint8_t UART_HasFrameError(uart_id_t uart)
{
    const uart_descriptor_t *descriptor = UART_RegistersGet(uart);

    if (descriptor == NULL)
    {
        return 0U;
    }

    return (uint8_t)READ_BIT(*descriptor->control_a, descriptor->frame_error_bit);
}

uint8_t UART_HasOverrunError(uart_id_t uart)
{
    const uart_descriptor_t *descriptor = UART_RegistersGet(uart);

    if (descriptor == NULL)
    {
        return 0U;
    }

    return (uint8_t)READ_BIT(*descriptor->control_a, descriptor->overrun_error_bit);
}

uint8_t UART_HasParityError(uart_id_t uart)
{
    const uart_descriptor_t *descriptor = UART_RegistersGet(uart);

    if (descriptor == NULL)
    {
        return 0U;
    }

    return (uint8_t)READ_BIT(*descriptor->control_a, descriptor->parity_error_bit);
}

/* -------------------------------------------------------------------------- */
/* ISRs - the only place in this driver that knows AVR vector names.          */
/* -------------------------------------------------------------------------- */

ISR(USART_RXC_vect)
{
    UART_Dispatch(UART_0, UART_INTERRUPT_RX_COMPLETE);
}

ISR(USART_TXC_vect)
{
    UART_Dispatch(UART_0, UART_INTERRUPT_TX_COMPLETE);
}

ISR(USART_UDRE_vect)
{
    UART_Dispatch(UART_0, UART_INTERRUPT_DATA_EMPTY);
}