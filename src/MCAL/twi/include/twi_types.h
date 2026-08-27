#ifndef TWI_TYPES_H
#define TWI_TYPES_H

#include <stdint.h>

/* -------------------------------------------------------------------------- */
/* Configuration                                                               */
/* -------------------------------------------------------------------------- */

typedef enum
{
    TWI_PRESCALER_1 = 0,
    TWI_PRESCALER_4,
    TWI_PRESCALER_16,
    TWI_PRESCALER_64
} twi_prescaler_t;

typedef enum
{
    TWI_DIRECTION_WRITE = 0,
    TWI_DIRECTION_READ  = 1
} twi_direction_t;

typedef struct
{
    uint32_t clock;
    twi_prescaler_t prescaler;
} twi_config_t;

/* -------------------------------------------------------------------------- */
/* Status                                                                      */
/* -------------------------------------------------------------------------- */

typedef enum
{
    TWI_STATUS_OK = 0,

    TWI_STATUS_START_ERROR,
    TWI_STATUS_REPEATED_START_ERROR,

    TWI_STATUS_SLA_W_NACK,
    TWI_STATUS_SLA_R_NACK,

    TWI_STATUS_DATA_NACK,

    TWI_STATUS_ARBITRATION_LOST,

    TWI_STATUS_BUS_ERROR,

    TWI_STATUS_INVALID_PARAMETER
} twi_status_t;

#endif