#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H

#include <stdint.h>

#include "adc_types.h"

/* -------------------------------------------------------------------------- */
/* ADC Configuration                                                           */
/* -------------------------------------------------------------------------- */

uint8_t ADC_Init(const adc_config_t *config);

void ADC_Enable(void);

void ADC_Disable(void);

void ADC_SetReference(adc_reference_t reference);

void ADC_SetAlignment(adc_alignment_t alignment);

void ADC_SetPrescaler(adc_prescaler_t prescaler);

void ADC_SetTriggerSource(adc_trigger_t trigger);

uint8_t ADC_SetInput(const adc_input_t *input);

/* -------------------------------------------------------------------------- */
/* Conversion                                                                  */
/* -------------------------------------------------------------------------- */

void ADC_StartConversion(void);

uint8_t ADC_IsConversionComplete(void);

uint16_t ADC_Read(void);

/* -------------------------------------------------------------------------- */
/* Auto Trigger                                                                */
/* -------------------------------------------------------------------------- */

void ADC_EnableAutoTrigger(adc_trigger_t trigger);

void ADC_DisableAutoTrigger(void);

/* -------------------------------------------------------------------------- */
/* Interrupt                                                                    */
/* -------------------------------------------------------------------------- */

void ADC_RegisterCallback(adc_callback_t callback);

void ADC_EnableInterrupt(void);

void ADC_DisableInterrupt(void);

void ADC_ClearFlag(void);

#endif /* ADC_DRIVER_H */
