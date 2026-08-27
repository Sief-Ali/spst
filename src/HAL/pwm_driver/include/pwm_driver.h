#ifndef PWM_DRIVER_H
#define PWM_DRIVER_H

#include <stdint.h>

#include "timer_types.h"

/* -------------------------------------------------------------------------- */
/* PWM Driver                                                                  */
/*                                                                              */
/* HAL-layer wrapper around the MCAL timer driver's public API only. Drives  */
/* one compare channel of a timer as a PWM output.                           */
/*                                                                              */
/* Note: both PWM modes here use the timer's 8-bit-top waveform variant (see */
/* MCAL/timer's TIMER_MODE_FAST_PWM / TIMER_MODE_PHASE_CORRECT_PWM), so duty */
/* resolution is 0-255 on every timer, including Timer1. Full 16-bit/ICR1-  */
/* topped PWM on Timer1 isn't exposed by the driver yet.                     */
/* -------------------------------------------------------------------------- */

typedef enum
{
    PWM_MODE_FAST,
    PWM_MODE_PHASE_CORRECT
} pwm_mode_t;

typedef struct
{
    timer_id_t timer;
    timer_compare_t channel;
    pwm_mode_t mode;
    uint32_t frequency_hz;
} pwm_config_t;

/* Configures and starts a PWM signal at approximately `frequency_hz`,
 * duty cycle 0% until PWM_SetDutyCycle is called. */
void PWM_Init(const pwm_config_t *config);

/* Sets duty cycle as a percentage (0-100, clamped). */
void PWM_SetDutyCycle(
    timer_id_t timer,
    timer_compare_t channel,
    uint8_t duty_percent);

/* Stops the timer driving the PWM signal (both channels, if both were in
 * use on that timer). */
void PWM_Stop(timer_id_t timer);

#endif /* PWM_DRIVER_H */
