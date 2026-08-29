/* HAL/servo/servo.c */

#include "servo.h"

#include <avr/io.h>
#include <stddef.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

/* Timer1 servo mode:
 * Fast PWM with TOP = ICR1, prescaler = 8.
 * For F_CPU = 16 MHz:
 *   frequency = F_CPU / (8 * (1 + ICR1))
 *   set ICR1 = 39999 to get 50 Hz exactly.
 */
#define SERVO_TIMER1_TOP 39999U
#define SERVO_TIMER1_PRESCALER 8U

static uint16_t Servo_AngleToPulseUs(
    const servo_config_t *config,
    uint8_t angle)
{
    uint16_t angle_range;
    uint16_t pulse_range;
    uint16_t angle_offset;

    angle_range = (uint16_t)(config->max_angle - config->min_angle);
    pulse_range = (uint16_t)(config->max_pulse_us - config->min_pulse_us);
    angle_offset = (uint16_t)(angle - config->min_angle);

    return (uint16_t)(
        config->min_pulse_us +
        (((uint32_t)angle_offset * pulse_range) / angle_range));
}

static uint16_t Servo_PulseUsToTicks(uint16_t pulse_us)
{
    uint32_t ticks;

    /* Timer1 tick time with prescaler=8: F_CPU / 8 = 2 MHz => 0.5 us/tick.
     * So pulse ticks = pulse_us * 2.
     */
    ticks = ((uint32_t)pulse_us * (F_CPU / 1000000UL)) / SERVO_TIMER1_PRESCALER;

    return (uint16_t)ticks;
}

static void Servo_ConfigureTimer1(void)
{
    /* Fast PWM, TOP = ICR1, non-inverted output on OC1A.
     * WGM13:0 = 1110 (mode 14)
     * COM1A1:0 = 10 (clear on compare match, set at BOTTOM)
     * CS11 = 1 => prescaler /8
     */
    TCCR1A = (1U << COM1A1) | (1U << WGM11);
    TCCR1B = (1U << WGM13) | (1U << WGM12) | (1U << CS11);

    ICR1 = SERVO_TIMER1_TOP;
    OCR1A = 3000U; /* 1.5 ms at 0.5 us/tick */
    TCNT1 = 0U;
}

static void Servo_SetPulseUs(
    servo_t *servo,
    uint16_t pulse_us)
{
    const servo_config_t *config = servo->config;
    uint16_t compare_ticks;

    if (pulse_us < config->min_pulse_us)
    {
        pulse_us = config->min_pulse_us;
    }

    if (pulse_us > config->max_pulse_us)
    {
        pulse_us = config->max_pulse_us;
    }

    compare_ticks = Servo_PulseUsToTicks(pulse_us);
    OCR1A = compare_ticks;
}

void Servo_Init(
    servo_t *servo)
{
    const servo_config_t *config;

    if (servo == NULL)
    {
        return;
    }

    config = servo->config;

    if (config == NULL)
    {
        return;
    }

    GPIO_InitPin(config->pin);
    Servo_ConfigureTimer1();

    servo->config = config;
    servo->current_angle = config->initial_angle;
    servo->initialized = 1U;

    Servo_SetAngle(servo, config->initial_angle);
}

void Servo_SetAngle(
    servo_t *servo,
    uint8_t angle)
{
    const servo_config_t *config;
    uint16_t pulse_us;

    if ((servo == NULL) || (servo->initialized == 0U))
    {
        return;
    }

    config = servo->config;

    if (angle < config->min_angle)
    {
        angle = config->min_angle;
    }

    if (angle > config->max_angle)
    {
        angle = config->max_angle;
    }

    pulse_us = Servo_AngleToPulseUs(config, angle);
    Servo_SetPulseUs(servo, pulse_us);

    servo->current_angle = angle;
}

void Servo_IncreaseAngle(
    servo_t *servo,
    uint8_t amount)
{
    uint16_t new_angle;

    if ((servo == NULL) || (servo->initialized == 0U))
    {
        return;
    }

    new_angle = (uint16_t)servo->current_angle + amount;

    if (new_angle > 180U)
    {
        new_angle = 180U;
    }

    Servo_SetAngle(servo, (uint8_t)new_angle);
}

void Servo_DecreaseAngle(
    servo_t *servo,
    uint8_t amount)
{
    if ((servo == NULL) || (servo->initialized == 0U))
    {
        return;
    }

    if (amount > servo->current_angle)
    {
        Servo_SetAngle(servo, 0U);
    }
    else
    {
        Servo_SetAngle(
            servo,
            (uint8_t)(servo->current_angle - amount));
    }
}

uint8_t Servo_GetAngle(
    const servo_t *servo)
{
    if ((servo == NULL) || (servo->initialized == 0U))
    {
        return 0U;
    }

    return servo->current_angle;
}