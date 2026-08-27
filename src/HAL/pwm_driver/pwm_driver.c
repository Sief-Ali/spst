#include "pwm_driver.h"

#include <stddef.h>
#include <stdint.h>

#include "timer.h"

#ifndef F_CPU
/* The build should define F_CPU (e.g. via the avr_options target in
 * CMake). This fallback only exists so the file still compiles stand-alone. */
#define F_CPU 8000000UL
#endif

/* Duty resolution: both PWM waveform variants implemented by the MCAL
 * driver top out at 8 bits (0-255) - see the note in pwm_driver.h. */
#define PWM_TOP 255U

static const uint16_t pwm_prescale[TIMER_CLOCK_COUNT] =
{
    [TIMER_CLOCK_STOP] = 0U,
    [TIMER_CLOCK_1]    = 1U,
    [TIMER_CLOCK_8]    = 8U,
    [TIMER_CLOCK_64]   = 64U,
    [TIMER_CLOCK_256]  = 256U,
    [TIMER_CLOCK_1024] = 1024U
};

/* Fast PWM: f = F_CPU / (prescale * 256). Phase Correct PWM counts up then
 * down, so it's half the rate: f = F_CPU / (prescale * 510). */
static uint32_t PWM_ActualFrequency(pwm_mode_t mode, timer_clock_t clock)
{
    uint32_t divisor = (mode == PWM_MODE_FAST) ? 256UL : 510UL;

    return F_CPU / (pwm_prescale[clock] * divisor);
}

/* Picks the clock whose resulting frequency is closest to the requested one. */
static uint8_t PWM_ChooseClock(
    uint32_t frequency_hz,
    pwm_mode_t mode,
    timer_clock_t *out_clock)
{
    uint32_t best_diff = 0xFFFFFFFFUL;
    timer_clock_t best_clock = TIMER_CLOCK_1;
    uint8_t idx;

    if (frequency_hz == 0U)
    {
        return 0U;
    }

    for (idx = (uint8_t)TIMER_CLOCK_1; idx <= (uint8_t)TIMER_CLOCK_1024; idx++)
    {
        uint32_t actual = PWM_ActualFrequency(mode, (timer_clock_t)idx);
        uint32_t diff = (actual > frequency_hz)
            ? (actual - frequency_hz)
            : (frequency_hz - actual);

        if (diff < best_diff)
        {
            best_diff = diff;
            best_clock = (timer_clock_t)idx;
        }
    }

    *out_clock = best_clock;
    return 1U;
}

void PWM_Init(const pwm_config_t *config)
{
    timer_mode_t mode;
    timer_clock_t clock;

    if ((config == NULL) || (config->channel >= TIMER_COMPARE_COUNT))
    {
        return;
    }

    mode = (config->mode == PWM_MODE_FAST)
        ? TIMER_MODE_FAST_PWM
        : TIMER_MODE_PHASE_CORRECT_PWM;

    if (PWM_ChooseClock(config->frequency_hz, config->mode, &clock) == 0U)
    {
        return;
    }

    TIMER_Stop(config->timer);
    TIMER_SetMode(config->timer, mode);

    /* Non-inverted PWM: clear the output on compare match (counting up),
     * set it at BOTTOM. */
    TIMER_SetOutputMode(config->timer, config->channel, TIMER_OUTPUT_CLEAR);
    TIMER_SetCompare(config->timer, config->channel, 0U);

    TIMER_SetClock(config->timer, clock);
}

void PWM_SetDutyCycle(
    timer_id_t timer,
    timer_compare_t channel,
    uint8_t duty_percent)
{
    uint16_t compare;

    if ((timer >= TIMER_COUNT) || (channel >= TIMER_COMPARE_COUNT))
    {
        return;
    }

    if (duty_percent > 100U)
    {
        duty_percent = 100U;
    }

    compare = (uint16_t)(((uint32_t)PWM_TOP * duty_percent) / 100U);

    TIMER_SetCompare(timer, channel, compare);
}

void PWM_Stop(timer_id_t timer)
{
    TIMER_Stop(timer);
}
