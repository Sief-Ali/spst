#include "uart_registers.h"

#include <avr/io.h>
#include <stddef.h>

/* -------------------------------------------------------------------------- */
/* Descriptor Table                                                            */
/* -------------------------------------------------------------------------- */

static const uart_descriptor_t uart_descriptors[UART_COUNT] =
{
    [UART_0] =
    {
        .control_a = &UCSRA,
        .control_b = &UCSRB,

        /* avr-libc's own <avr/io.h> defines UCSRC as UBRRH - they are
         * literally the same address on this chip, which is exactly why
         * UART_WriteControlC/UART_WriteBaudHigh exist: every write through
         * either field must set/clear URSEL to say which one it means. */
        .control_c = &UCSRC,
        .baud_high = &UBRRH,
        .baud_low  = &UBRRL,

        .data_reg = &UDR,

        .ursel_bit = URSEL,
        .u2x_bit   = U2X,

        .data_bits_b_bit   = UCSZ2,
        .data_bits_c_shift = UCSZ0,

        .data_bits_value_b =
        {
            [UART_DATA_BITS_5] = 0U,
            [UART_DATA_BITS_6] = 0U,
            [UART_DATA_BITS_7] = 0U,
            [UART_DATA_BITS_8] = 0U,
            [UART_DATA_BITS_9] = 1U
        },

        .data_bits_value_c =
        {
            [UART_DATA_BITS_5] = 0U,
            [UART_DATA_BITS_6] = 1U,
            [UART_DATA_BITS_7] = 2U,
            [UART_DATA_BITS_8] = 3U,
            [UART_DATA_BITS_9] = 3U
        },

        .parity_shift = UPM0,
        .parity_value =
        {
            [UART_PARITY_NONE] = 0U,
            [UART_PARITY_EVEN] = 2U,
            [UART_PARITY_ODD]  = 3U
        },

        .stop_bits_bit = USBS,
        .stop_bits_value =
        {
            [UART_STOP_BITS_1] = 0U,
            [UART_STOP_BITS_2] = 1U
        },

        .tx_enable_bit = TXEN,
        .rx_enable_bit = RXEN,

        .interrupt_enable_bit =
        {
            [UART_INTERRUPT_RX_COMPLETE] = RXCIE,
            [UART_INTERRUPT_TX_COMPLETE] = TXCIE,
            [UART_INTERRUPT_DATA_EMPTY]  = UDRIE
        },

        .flag_bit =
        {
            [UART_INTERRUPT_RX_COMPLETE] = RXC,
            [UART_INTERRUPT_TX_COMPLETE] = TXC,
            [UART_INTERRUPT_DATA_EMPTY]  = UDRE
        },

        .flag_clearable =
        {
            /* RXC clears itself when UDR is read; UDRE clears itself when
             * UDR is written. Only TXC supports a manual write-1-to-clear. */
            [UART_INTERRUPT_RX_COMPLETE] = 0U,
            [UART_INTERRUPT_TX_COMPLETE] = 1U,
            [UART_INTERRUPT_DATA_EMPTY]  = 0U
        },

        .frame_error_bit   = FE,
        .overrun_error_bit = DOR,
        .parity_error_bit  = PE
    }
};

/* -------------------------------------------------------------------------- */
/* Descriptor Access                                                           */
/* -------------------------------------------------------------------------- */

const uart_descriptor_t *UART_RegistersGet(uart_id_t uart)
{
    if (uart >= UART_COUNT)
    {
        return NULL;
    }

    return &uart_descriptors[uart];
}

/* -------------------------------------------------------------------------- */
/* UCSRC / UBRRH Disambiguation                                                */
/* -------------------------------------------------------------------------- */

void UART_WriteControlC(
    const uart_descriptor_t *descriptor,
    uint8_t value)
{
    uint8_t out = value;

    if (descriptor->ursel_bit != UART_UNUSED_BIT)
    {
        out |= (uint8_t)(1U << descriptor->ursel_bit);
    }

    *descriptor->control_c = out;
}

void UART_WriteBaudHigh(
    const uart_descriptor_t *descriptor,
    uint8_t value)
{
    uint8_t out = value;

    if (descriptor->ursel_bit != UART_UNUSED_BIT)
    {
        out &= (uint8_t)~(1U << descriptor->ursel_bit);
    }

    *descriptor->baud_high = out;
}