#ifndef UART_REGISTERS_H
#define UART_REGISTERS_H

#include <stdint.h>

#include "uart_private.h"
#include "uart_types.h"

/* -------------------------------------------------------------------------- */
/* Descriptor Access                                                           */
/* -------------------------------------------------------------------------- */

const uart_descriptor_t *UART_RegistersGet(
    uart_id_t uart);

/* -------------------------------------------------------------------------- */
/* UCSRC / UBRRH Disambiguation                                                */
/*                                                                              */
/* On the ATmega32, UCSRC and UBRRH share one physical I/O address; a write   */
/* is routed to one or the other depending on the URSEL bit set *in that      */
/* same write*. Read access always returns UBRRH's contents - UCSRC cannot    */
/* be read back at all. These two functions are the only places in the       */
/* driver that need to know that; everywhere else just sees "the frame       */
/* format register" and "the baud rate high byte" as if they were ordinary,  */
/* independent registers.                                                    */
/* -------------------------------------------------------------------------- */

void UART_WriteControlC(
    const uart_descriptor_t *descriptor,
    uint8_t value);

void UART_WriteBaudHigh(
    const uart_descriptor_t *descriptor,
    uint8_t value);

#endif /* UART_REGISTERS_H */