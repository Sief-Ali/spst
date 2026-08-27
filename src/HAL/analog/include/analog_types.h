#ifndef HAL_ANALOG_TYPES_H
#define HAL_ANALOG_TYPES_H

#include <stdint.h>

#include "adc.h"

/* -------------------------------------------------------------------------- */
/* Analog Configuration                                                       */
/* -------------------------------------------------------------------------- */

typedef struct
{
    adc_config_t adc;
} analog_config_t;

/* -------------------------------------------------------------------------- */
/* Analog Device                                                              */
/* -------------------------------------------------------------------------- */

typedef struct
{
    adc_input_t input;
} analog_t;

#endif /* HAL_ANALOG_TYPES_H */