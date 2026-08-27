#include "twi.h"

#include <avr/io.h>
#include <stddef.h>

#include "bit_utils.h"

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

/* -------------------------------------------------------------------------- */
/* Private Helpers                                                             */
/* -------------------------------------------------------------------------- */

static void TWI_Wait(void)
{
    while (BIT_IS_CLEAR(TWCR, TWINT))
    {
    }
}

/* -------------------------------------------------------------------------- */
/* Initialization                                                              */
/* -------------------------------------------------------------------------- */

void TWI_Init(
    const twi_config_t *config)
{
    uint8_t prescaler_bits = 0U;
    uint16_t prescaler = 1U;

    if (config == NULL)
    {
        return;
    }

    switch (config->prescaler)
    {
        case TWI_PRESCALER_1:
            prescaler_bits = 0U;
            prescaler = 1U;
            break;

        case TWI_PRESCALER_4:
            prescaler_bits = 1U;
            prescaler = 4U;
            break;

        case TWI_PRESCALER_16:
            prescaler_bits = 2U;
            prescaler = 16U;
            break;

        case TWI_PRESCALER_64:
            prescaler_bits = 3U;
            prescaler = 64U;
            break;

        default:
            return;
    }

    TWSR = prescaler_bits;

    TWBR = (uint8_t)
        (((F_CPU / config->clock) - 16UL) /
         (2UL * prescaler));

    TWCR = (1 << TWEN);
}

/* -------------------------------------------------------------------------- */
/* Bus Control                                                                 */
/* -------------------------------------------------------------------------- */

twi_status_t TWI_Start(void)
{
    TWCR =
        (1 << TWINT) |
        (1 << TWSTA) |
        (1 << TWEN);

    TWI_Wait();

    return TWI_STATUS_OK;
}

twi_status_t TWI_RepeatedStart(void)
{
    TWCR =
        (1 << TWINT) |
        (1 << TWSTA) |
        (1 << TWEN);

    TWI_Wait();

    return TWI_STATUS_OK;
}

void TWI_Stop(void)
{
    TWCR =
        (1 << TWINT) |
        (1 << TWSTO) |
        (1 << TWEN);
}

/* -------------------------------------------------------------------------- */
/* Address                                                                     */
/* -------------------------------------------------------------------------- */

twi_status_t TWI_SendAddress(
    uint8_t address,
    twi_direction_t direction)
{
    TWDR = (address << 1) | direction;

    TWCR =
        (1 << TWINT) |
        (1 << TWEN);

    TWI_Wait();

    return TWI_STATUS_OK;
}

/* -------------------------------------------------------------------------- */
/* Data                                                                        */
/* -------------------------------------------------------------------------- */

twi_status_t TWI_WriteByte(
    uint8_t data)
{
    TWDR = data;

    TWCR =
        (1 << TWINT) |
        (1 << TWEN);

    TWI_Wait();

    return TWI_STATUS_OK;
}

twi_status_t TWI_ReadByteAck(
    uint8_t *data)
{
    if (data == NULL)
    {
        return TWI_STATUS_INVALID_PARAMETER;
    }

    TWCR =
        (1 << TWINT) |
        (1 <<TWEN) |
        (1 << TWEA);

    TWI_Wait();

    *data = TWDR;

    return TWI_STATUS_OK;
}

twi_status_t TWI_ReadByteNack(
    uint8_t *data)
{
    if (data == NULL)
    {
        return TWI_STATUS_INVALID_PARAMETER;
    }

    TWCR =
        (1 << TWINT) |
        (1 << TWEN);

    TWI_Wait();

    *data = TWDR;

    return TWI_STATUS_OK;
}