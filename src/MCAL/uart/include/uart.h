#ifndef UART_H
#define UART_H

#include <stdint.h>

#include "uart_types.h"

/* -------------------------------------------------------------------------- */
/* Configuration                                                               */
/* -------------------------------------------------------------------------- */

void UART_Init(
    const uart_config_t *config);

/* -------------------------------------------------------------------------- */
/* Enable / Disable                                                            */
/* -------------------------------------------------------------------------- */

void UART_EnableTx(uart_id_t uart);
void UART_DisableTx(uart_id_t uart);

void UART_EnableRx(uart_id_t uart);
void UART_DisableRx(uart_id_t uart);

/* -------------------------------------------------------------------------- */
/* Raw Data Access                                                             */
/*                                                                              */
/* Non-blocking - neither of these wait. Caller must check                    */
/* UART_TransmitReady/UART_DataAvailable first (or use interrupts). Blocking  */
/* send/receive is HAL policy, not MCAL's - same split as Timer's raw         */
/* TIMER_Write/Read vs. TIMER_SERVICE_DelayMs.                                */
/* -------------------------------------------------------------------------- */

void UART_WriteByte(
    uart_id_t uart,
    uint8_t data);

uint8_t UART_ReadByte(
    uart_id_t uart);

uint8_t UART_TransmitReady(uart_id_t uart);
uint8_t UART_DataAvailable(uart_id_t uart);

/* -------------------------------------------------------------------------- */
/* Interrupts                                                                  */
/* -------------------------------------------------------------------------- */

void UART_EnableInterrupt(uart_id_t uart, uart_interrupt_t interrupt);
void UART_DisableInterrupt(uart_id_t uart, uart_interrupt_t interrupt);

void UART_ClearFlag(uart_id_t uart, uart_interrupt_t interrupt);
uint8_t UART_GetFlag(uart_id_t uart, uart_interrupt_t interrupt);

/* -------------------------------------------------------------------------- */
/* Errors                                                                      */
/*                                                                              */
/* Reflect the status of whichever byte is currently in UDR - check right     */
/* after UART_DataAvailable/before UART_ReadByte moves on to the next byte.   */
/* -------------------------------------------------------------------------- */

uint8_t UART_HasFrameError(uart_id_t uart);
uint8_t UART_HasOverrunError(uart_id_t uart);
uint8_t UART_HasParityError(uart_id_t uart);

/* -------------------------------------------------------------------------- */
/* Callback                                                                    */
/* -------------------------------------------------------------------------- */

/* Registers `callback` to be invoked from the driver's own ISR when
 * `interrupt` fires on `uart`. Pass NULL to detach. Mirrors
 * TIMER_RegisterCallback/EXT_INT_RegisterCallback. */
void UART_RegisterCallback(
    uart_id_t uart,
    uart_interrupt_t interrupt,
    uart_callback_t callback);

#endif /* UART_H */