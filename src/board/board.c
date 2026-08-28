#include "board.h"

/* =========================
 * LEDs
 * ========================= */

board_leds_t led =
{
    .ready =
    {
        .pin = LED_READY
    },

    .processing =
    {
        .pin = LED_PROCESSING
    },

    .error =
    {
        .pin = LED_ERROR
    }
};


/* =========================
 * East LDR
 * ========================= */

adc_config_t east_ldr_adc_config =
{
    .reference = ADC_REFERENCE_AREF,

    .alignment = ADC_ALIGNMENT_RIGHT,

    .prescaler = ADC_PRESCALER_128,

    .input =
    {
        .mode = ADC_INPUT_SINGLE_ENDED,
        .positive = LDR_EAST
    },

    .auto_trigger = ADC_AUTO_TRIGGER_DISABLE,

    .interrupt = ADC_INTERRUPT_DISABLE,

    .trigger = ADC_TRIGGER_FREE_RUNNING
};


/* =========================
 * West LDR
 * ========================= */

adc_config_t west_ldr_adc_config =
{
    .reference = ADC_REFERENCE_AREF,

    .alignment = ADC_ALIGNMENT_RIGHT,

    .prescaler = ADC_PRESCALER_128,

    .input =
    {
        .mode = ADC_INPUT_SINGLE_ENDED,
        .positive = LDR_WEST
    },

    .auto_trigger = ADC_AUTO_TRIGGER_DISABLE,

    .interrupt = ADC_INTERRUPT_DISABLE,

    .trigger = ADC_TRIGGER_FREE_RUNNING
};


/* =========================
 * TWI / I²C
 * ========================= */

twi_config_t board_twi_config =
{
    .clock = 100000UL,
    .prescaler = TWI_PRESCALER_1
};

/* =========================
 *  Servo PWM
 * ========================= */
pwm_config_t servo_pwm_config = {
    .timer = TIMER_1,
    .channel = TIMER_COMPARE_A,
    .mode = PWM_MODE_FAST,
    .frequency_hz = 50U
};