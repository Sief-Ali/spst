#ifndef UART_TYPES_H
#define UART_TYPES_H

#include <stdint.h>

/* -------------------------------------------------------------------------- */
/* UART Instances                                                              */
/*                                                                              */
/* The ATmega32 only has one USART, but this still follows the same           */
/* id/COUNT pattern as timer_id_t/ext_int_id_t for consistency, and so a      */
/* chip with more than one USART would only need a bigger descriptor table -  */
/* no public API changes.                                                     */
/* -------------------------------------------------------------------------- */

typedef enum
{
    UART_0,

    UART_COUNT
} uart_id_t;

/* -------------------------------------------------------------------------- */
/* Frame Format                                                                */
/* -------------------------------------------------------------------------- */

typedef enum
{
    UART_DATA_BITS_5,
    UART_DATA_BITS_6,
    UART_DATA_BITS_7,
    UART_DATA_BITS_8,
    UART_DATA_BITS_9,

    UART_DATA_BITS_COUNT
} uart_data_bits_t;

typedef enum
{
    UART_PARITY_NONE,
    UART_PARITY_EVEN,
    UART_PARITY_ODD,

    UART_PARITY_COUNT
} uart_parity_t;

typedef enum
{
    UART_STOP_BITS_1,
    UART_STOP_BITS_2,

    UART_STOP_BITS_COUNT
} uart_stop_bits_t;

/* -------------------------------------------------------------------------- */
/* Interrupt Sources                                                           */
/* -------------------------------------------------------------------------- */

typedef enum
{
    UART_INTERRUPT_RX_COMPLETE,
    UART_INTERRUPT_TX_COMPLETE,
    UART_INTERRUPT_DATA_EMPTY,

    UART_INTERRUPT_COUNT
} uart_interrupt_t;

/* -------------------------------------------------------------------------- */
/* Callback Type                                                               */
/* -------------------------------------------------------------------------- */

typedef void (*uart_callback_t)(void);

/* -------------------------------------------------------------------------- */
/* UART Configuration                                                          */
/* -------------------------------------------------------------------------- */

typedef struct
{
    uart_id_t uart;
    uint32_t baud_rate;
    uart_data_bits_t data_bits;
    uart_parity_t parity;
    uart_stop_bits_t stop_bits;
} uart_config_t;

#endif /* UART_TYPES_H */