#include "adc_registers.h"

#include <avr/io.h>

/* -------------------------------------------------------------------------- */
/* Register Helpers                                                            */
/* -------------------------------------------------------------------------- */

#define ADC_REG8(REG) \
    { (volatile void *)&(REG), ADC_REGISTER_8_BIT }

#define ADC_REG16(REG) \
    { (volatile void *)&(REG), ADC_REGISTER_16_BIT }

/* -------------------------------------------------------------------------- */
/* ADC Descriptor                                                              */
/* -------------------------------------------------------------------------- */

static const adc_descriptor_t adc_descriptor =
{
    .admux  = ADC_REG8(ADMUX),
    .adcsra = ADC_REG8(ADCSRA),
    .adc    = ADC_REG16(ADC),
    .sfior  = ADC_REG8(SFIOR),

    /* ADMUX */
    .reference_mask = (uint8_t)((1U << REFS1) | (1U << REFS0)),
    .alignment_mask = (uint8_t)(1U << ADLAR),
    .channel_mask   = (uint8_t)0x1FU,

    /* SFIOR */
    .trigger_mask = (uint8_t)((1U << ADTS2) |
                              (1U << ADTS1) |
                              (1U << ADTS0)),

    /* ADCSRA */
    .prescaler_mask = (uint8_t)((1U << ADPS2) |
                                (1U << ADPS1) |
                                (1U << ADPS0)),

    .enable_bit            = ADEN,
    .start_bit             = ADSC,
    .interrupt_enable_bit  = ADIE,
    .interrupt_flag_bit    = ADIF,
    .auto_trigger_bit      = ADATE
};

/* -------------------------------------------------------------------------- */
/* Public Functions                                                            */
/* -------------------------------------------------------------------------- */

const adc_descriptor_t *ADC_RegistersGet(void)
{
    return &adc_descriptor;
}