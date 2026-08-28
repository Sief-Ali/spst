/* HAL/servo/servo.c */

#include "servo.h"

#include <stddef.h>


static uint16_t Servo_AngleToPulseUs(
    const servo_config_t *config,
    uint8_t angle)
{
    uint16_t angle_range;
    uint16_t pulse_range;
    uint16_t angle_offset;

    angle_range = (uint16_t)(config->max_angle - config->min_angle);
    pulse_range = (uint16_t)(
        config->max_pulse_us - config->min_pulse_us);

    angle_offset = (uint16_t)(angle - config->min_angle);

    return (uint16_t)(
        config->min_pulse_us +
        (((uint32_t)angle_offset * pulse_range) / angle_range));
}

static void Servo_SetPulseUs(
    servo_t *servo,
    uint16_t pulse_us)
{
    const servo_config_t *config = servo->config;
    uint32_t period_us;
    uint8_t duty_percent;

    period_us = 1000000UL / config->pwm.frequency_hz;

    duty_percent = (uint8_t)(
        ((uint32_t)pulse_us * 100UL) / period_us);

    PWM_SetDutyCycle(
        config->pwm.timer,
        config->pwm.channel,
        duty_percent);
}

void Servo_Init(
    servo_t *servo)
{
    const servo_config_t *config = servo->config;

    if ((servo == NULL) || (config == NULL))
    {
        return;
    }

    GPIO_InitPin(config->pin);
    PWM_Init(&config->pwm);

    servo->pin = config->pin;
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

    /*
     * The configuration would need to be associated with the servo
     * object, or stored privately by the module.
     */
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