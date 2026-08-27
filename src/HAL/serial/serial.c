#include "serial.h"

#include <stddef.h>
#include <stdlib.h>

#include "uart.h"

static const uart_config_t serial_config =
{
    .uart = UART_0,

    .baud_rate = 9600,

    .data_bits = UART_DATA_BITS_8,
    .parity    = UART_PARITY_NONE,
    .stop_bits = UART_STOP_BITS_1
};

void Serial_Init(uint32_t baud_rate)
{
    uart_config_t config = serial_config;

    config.baud_rate = baud_rate;

    UART_Init(&config);

    UART_EnableTx(UART_0);
    UART_EnableRx(UART_0);
}

uint8_t Serial_Read(uint8_t *data)
{
    if (data == NULL)
    {
        return 0U;
    }

    if (UART_DataAvailable(UART_0) == 0U)
    {
        return 0U;
    }

    *data = UART_ReadByte(UART_0);

    return 1U;
}

uint8_t Serial_ReadWait(void)
{
    while (UART_DataAvailable(UART_0) == 0U)
    {
    }

    return UART_ReadByte(UART_0);
}

void Serial_WriteByte(uint8_t data)
{
    while (UART_TransmitReady(UART_0) == 0U)
    {
    }

    UART_WriteByte(UART_0, data);
}

void Serial_Write(const char *text)
{
    if (text == NULL)
    {
        return;
    }

    while (*text != '\0')
    {
        Serial_WriteByte((uint8_t)*text);
        text++;
    }
}

void Serial_WriteUInt16(uint16_t value)
{
    char buffer[6];

    itoa(value, buffer, 10);

    Serial_Write(buffer);
}

void Serial_RegisterReceiveCallback(
    uart_callback_t callback)
{
    UART_RegisterCallback(
        UART_0,
        UART_INTERRUPT_RX_COMPLETE,
        callback);
}

void Serial_EnableReceiveInterrupt(void)
{
    UART_EnableInterrupt(
        UART_0,
        UART_INTERRUPT_RX_COMPLETE);
}

void Serial_DisableReceiveInterrupt(void)
{
    UART_DisableInterrupt(
        UART_0,
        UART_INTERRUPT_RX_COMPLETE);
}