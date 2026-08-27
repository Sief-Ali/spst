#ifndef HAL_ANALOG_H
#define HAL_ANALOG_H

#include <stdint.h>

#include "adc.h"
#include "adc_types.h"

/* Initializes the analog HAL using the supplied ADC configuration. */
void Analog_Init(const adc_config_t *config);

/* Reads the currently configured ADC input. */
uint16_t Analog_Read(void);

/* Selects an ADC input and returns one reading, or 0 if the input is invalid. */
uint16_t Analog_ReadChannel(adc_input_t input);

/* Converts the project potentiometer reading into a ticket quantity from 0 to 5. */
uint8_t Analog_Get_Quantity(void);

#endif
