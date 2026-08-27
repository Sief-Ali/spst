#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

#include "uart_types.h"

/* -------------------------------------------------------------------------- */
/* Initialization                                                              */
/* -------------------------------------------------------------------------- */

void Serial_Init(uint32_t baud_rate);

/* -------------------------------------------------------------------------- */
/* Read                                                                        */
/* -------------------------------------------------------------------------- */

/* Returns 1 if a byte was read, 0 otherwise. */
uint8_t Serial_Read(uint8_t *data);

/* Blocks until one byte is received. */
uint8_t Serial_ReadWait(void);

/* -------------------------------------------------------------------------- */
/* Write                                                                       */
/* -------------------------------------------------------------------------- */

void Serial_WriteByte(uint8_t data);

void Serial_Write(const char *text);

void Serial_WriteUInt16(uint16_t value);

/* -------------------------------------------------------------------------- */
/* Interrupts                                                                  */
/* -------------------------------------------------------------------------- */

void Serial_RegisterReceiveCallback(
    uart_callback_t callback);

void Serial_EnableReceiveInterrupt(void);

void Serial_DisableReceiveInterrupt(void);

#endif