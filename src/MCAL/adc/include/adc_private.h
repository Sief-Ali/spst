#ifndef ADC_PRIVATE_H
#define ADC_PRIVATE_H

#include <stdint.h>

typedef enum
{
    ADC_REGISTER_UNUSED,
    ADC_REGISTER_8_BIT,
    ADC_REGISTER_16_BIT
} adc_register_width_t;

typedef struct
{
    volatile void *address;
    adc_register_width_t width;
} adc_register_t;

typedef struct
{
    adc_register_t admux;
    adc_register_t adcsra;
    adc_register_t adc;
    adc_register_t sfior;

    uint8_t reference_mask;
    uint8_t alignment_mask;
    uint8_t channel_mask;
    uint8_t trigger_mask;
    uint8_t prescaler_mask;

    uint8_t enable_bit;
    uint8_t start_bit;
    uint8_t interrupt_enable_bit;
    uint8_t interrupt_flag_bit;
    uint8_t auto_trigger_bit;

} adc_descriptor_t;

#endif