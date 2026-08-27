#ifndef UART_PRIVATE_H
#define UART_PRIVATE_H

#include <stdint.h>

#include "uart_types.h"

/* -------------------------------------------------------------------------- */
/* Sizing                                                                      */
/* -------------------------------------------------------------------------- */

/* Marks a control bit as not present, or a mode as unsupported, on a given
 * UART (mirrors TIMER_UNUSED_BIT / EXT_INT_UNUSED_BIT). */
#define UART_UNUSED_BIT 0xFFU

/* -------------------------------------------------------------------------- */
/* UART Descriptor                                                             */
/* -------------------------------------------------------------------------- */

typedef struct
{
    /* Registers */

    volatile uint8_t *control_a; /* UCSRA */
    volatile uint8_t *control_b; /* UCSRB */
    volatile uint8_t *control_c; /* UCSRC - on the ATmega32 this is the SAME
                                     physical register as baud_high (UBRRH);
                                     see the note in uart_registers.c. */
    volatile uint8_t *baud_low;  /* UBRRL */
    volatile uint8_t *baud_high; /* UBRRH */
    volatile uint8_t *data_reg;  /* UDR */

    /* UART_UNUSED_BIT on a chip where control_c/baud_high genuinely don't
     * share an address (no disambiguation bit needed there). */
    uint8_t ursel_bit;

    uint8_t u2x_bit; /* UCSRA */

    /* Data bits: UCSZ2 lives in control_b, UCSZ1:UCSZ0 live in control_c -
     * split across two registers, same idea as Timer1's waveform mode
     * being split across TCCR1A/TCCR1B. */
    uint8_t data_bits_b_bit;                          /* UCSZ2 position in control_b */
    uint8_t data_bits_c_shift;                         /* UCSZ0 position in control_c */
    uint8_t data_bits_value_b[UART_DATA_BITS_COUNT];   /* 0 or 1 */
    uint8_t data_bits_value_c[UART_DATA_BITS_COUNT];   /* 2-bit code, pre-shift */

    /* Parity: UPM1:UPM0 in control_c */
    uint8_t parity_shift;
    uint8_t parity_value[UART_PARITY_COUNT]; /* 2-bit code, pre-shift */

    /* Stop bits: USBS in control_c */
    uint8_t stop_bits_bit;
    uint8_t stop_bits_value[UART_STOP_BITS_COUNT]; /* 0 or 1 */

    /* Enable */

    uint8_t tx_enable_bit; /* control_b */
    uint8_t rx_enable_bit; /* control_b */

    /* Interrupts (enable bits in control_b, flags in control_a) */

    uint8_t interrupt_enable_bit[UART_INTERRUPT_COUNT];
    uint8_t flag_bit[UART_INTERRUPT_COUNT];
    uint8_t flag_clearable[UART_INTERRUPT_COUNT]; /* 1 only for TX_COMPLETE - see uart.c */

    /* Errors (control_a, read-only) */

    uint8_t frame_error_bit;
    uint8_t overrun_error_bit;
    uint8_t parity_error_bit;

} uart_descriptor_t;

#endif /* UART_PRIVATE_H */