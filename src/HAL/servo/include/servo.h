/* HAL/servo/include/servo.h */

#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

#include "gpio.h"
#include "pwm_driver.h"

typedef struct
{
    pwm_config_t pwm;
    gpio_pin_t pin;

    uint8_t min_angle;
    uint8_t max_angle;

    uint16_t min_pulse_us;
    uint16_t max_pulse_us;

    uint8_t initial_angle;
} servo_config_t;

typedef struct
{
    const servo_config_t *config;
    uint8_t current_angle;
    uint8_t initialized;
} servo_t;

void Servo_Init(
    servo_t *servo);

void Servo_SetAngle(
    servo_t *servo,
    uint8_t angle);

void Servo_IncreaseAngle(
    servo_t *servo,
    uint8_t amount);

void Servo_DecreaseAngle(
    servo_t *servo,
    uint8_t amount);

uint8_t Servo_GetAngle(
    const servo_t *servo);

#endif /* SERVO_H */