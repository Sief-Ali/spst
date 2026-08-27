#ifndef ADC_TYPES_H
#define ADC_TYPES_H

#include <stdint.h>

/* -------------------------------------------------------------------------- */
/* Input Channels                                                              */
/* -------------------------------------------------------------------------- */

typedef enum
{
    ADC_CHANNEL_0,
    ADC_CHANNEL_1,
    ADC_CHANNEL_2,
    ADC_CHANNEL_3,
    ADC_CHANNEL_4,
    ADC_CHANNEL_5,
    ADC_CHANNEL_6,
    ADC_CHANNEL_7,

    ADC_CHANNEL_COUNT
} adc_channel_t;

/* -------------------------------------------------------------------------- */
/* Voltage Reference                                                           */
/* -------------------------------------------------------------------------- */

typedef enum
{
    ADC_REFERENCE_AREF,
    ADC_REFERENCE_AVCC,
    ADC_REFERENCE_INTERNAL_2V56,

    ADC_REFERENCE_COUNT
} adc_reference_t;

/* -------------------------------------------------------------------------- */
/* Result Alignment                                                            */
/* -------------------------------------------------------------------------- */

typedef enum
{
    ADC_ALIGNMENT_RIGHT,
    ADC_ALIGNMENT_LEFT,

    ADC_ALIGNMENT_COUNT
} adc_alignment_t;

/* -------------------------------------------------------------------------- */
/* Input Mode                                                                  */
/* -------------------------------------------------------------------------- */

typedef enum
{
    ADC_INPUT_SINGLE_ENDED,
    ADC_INPUT_DIFFERENTIAL,

    ADC_INPUT_MODE_COUNT
} adc_input_mode_t;

/* -------------------------------------------------------------------------- */
/* Differential Gain                                                           */
/* -------------------------------------------------------------------------- */

typedef enum
{
    ADC_GAIN_1X,
    ADC_GAIN_10X,
    ADC_GAIN_200X,

    ADC_GAIN_COUNT
} adc_gain_t;

/* -------------------------------------------------------------------------- */
/* ADC Input Configuration                                                     */
/* -------------------------------------------------------------------------- */

typedef struct
{
    adc_input_mode_t mode;

    adc_channel_t positive;
    adc_channel_t negative;

    adc_gain_t gain;

} adc_input_t;

/* -------------------------------------------------------------------------- */
/* Clock Prescaler                                                             */
/* -------------------------------------------------------------------------- */

typedef enum
{
    ADC_PRESCALER_2,
    ADC_PRESCALER_4,
    ADC_PRESCALER_8,
    ADC_PRESCALER_16,
    ADC_PRESCALER_32,
    ADC_PRESCALER_64,
    ADC_PRESCALER_128,

    ADC_PRESCALER_COUNT
} adc_prescaler_t;

/* -------------------------------------------------------------------------- */
/* Auto Trigger Source                                                         */
/* -------------------------------------------------------------------------- */

typedef enum
{
    ADC_TRIGGER_FREE_RUNNING,
    ADC_TRIGGER_ANALOG_COMPARATOR,
    ADC_TRIGGER_EXTERNAL_INTERRUPT_0,
    ADC_TRIGGER_TIMER0_COMPARE_MATCH,
    ADC_TRIGGER_TIMER0_OVERFLOW,
    ADC_TRIGGER_TIMER1_COMPARE_MATCH_B,
    ADC_TRIGGER_TIMER1_OVERFLOW,
    ADC_TRIGGER_TIMER1_CAPTURE_EVENT,

    ADC_TRIGGER_COUNT
} adc_trigger_t;

/* -------------------------------------------------------------------------- */
/* Callback                                                                    */
/* -------------------------------------------------------------------------- */

typedef void (*adc_callback_t)(void);


typedef struct
{
    adc_input_t input;
    uint8_t mux_bits;
} adc_mux_map_t;

typedef enum
{
    ADC_AUTO_TRIGGER_DISABLE = 0U,
    ADC_AUTO_TRIGGER_ENABLE
} adc_auto_trigger_t;

typedef enum
{
    ADC_INTERRUPT_DISABLE = 0U,
    ADC_INTERRUPT_ENABLE
} adc_interrupt_t;

typedef struct
{
    adc_input_t input;

    adc_reference_t reference;

    adc_alignment_t alignment;

    adc_prescaler_t prescaler;

    adc_auto_trigger_t auto_trigger;

    adc_trigger_t trigger;

    adc_interrupt_t interrupt;
} adc_config_t;

#endif /* ADC_TYPES_H */