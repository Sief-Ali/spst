#include "timer_service.h"

#include <stddef.h>

#include "timer.h"

#ifndef F_CPU
/* The build should define F_CPU (e.g. via the avr_options target in
 * CMake). This fallback only exists so the file still compiles stand-alone. */
#define F_CPU 8000000UL
#endif

/* -------------------------------------------------------------------------- */
/* Prescaler Selection                                                         */
/* -------------------------------------------------------------------------- */

static const uint16_t timer_service_prescale[TIMER_CLOCK_COUNT] =
{
    [TIMER_CLOCK_STOP] = 0U,
    [TIMER_CLOCK_1]    = 1U,
    [TIMER_CLOCK_8]    = 8U,
    [TIMER_CLOCK_64]   = 64U,
    [TIMER_CLOCK_256]  = 256U,
    [TIMER_CLOCK_1024] = 1024U
};

/* Picks the finest-resolution clock (smallest prescaler) that can still
 * express `milliseconds` as a CTC compare value within `timer`'s counter
 * width (found via TIMER_GetMaxCount - works for both 8-bit and 16-bit
 * timers without this file knowing which is which). */
static uint8_t TIMER_SERVICE_ComputeTicks(
    timer_id_t timer,
    uint16_t milliseconds,
    timer_clock_t *out_clock,
    uint16_t *out_compare)
{
    uint16_t max_count = TIMER_GetMaxCount(timer);
    uint8_t idx;

    for (idx = (uint8_t)TIMER_CLOCK_1; idx <= (uint8_t)TIMER_CLOCK_1024; idx++)
    {
        uint32_t freq_hz = F_CPU / timer_service_prescale[idx];
        uint32_t ticks = (freq_hz / 1000UL) * milliseconds;

        if ((ticks >= 1U) && ((ticks - 1U) <= max_count))
        {
            *out_clock = (timer_clock_t)idx;
            *out_compare = (uint16_t)(ticks - 1U);
            return 1U;
        }
    }

    /* Requested period doesn't fit even at the slowest prescaler on this
     * timer - caller picked too long a delay for an 8-bit timer, or needs
     * to chain several shorter intervals instead. */
    return 0U;
}

/* -------------------------------------------------------------------------- */
/* Timeout / Interval Dispatch                                                 */
/* -------------------------------------------------------------------------- */

typedef struct
{
    timer_callback_t callback;
    uint8_t one_shot;
} timer_service_slot_t;

static timer_service_slot_t timer_service_slots[TIMER_COUNT];

static void TIMER_SERVICE_Dispatch(timer_id_t timer)
{
    timer_service_slot_t *slot = &timer_service_slots[timer];
    timer_callback_t callback = slot->callback;

    if (slot->one_shot != 0U)
    {
        TIMER_Stop(timer);
        TIMER_DisableInterrupt(timer, TIMER_INTERRUPT_COMPARE_A);
        TIMER_RegisterCallback(timer, TIMER_INTERRUPT_COMPARE_A, NULL);
        slot->callback = NULL;
    }

    if (callback != NULL)
    {
        callback();
    }
}

/* One trampoline per timer: TIMER_RegisterCallback only stores a plain
 * void(void) function pointer, so each trampoline closes over which timer
 * it belongs to and forwards into the shared dispatcher above. */
static void TIMER_SERVICE_Dispatch0(void) { TIMER_SERVICE_Dispatch(TIMER_0); }
static void TIMER_SERVICE_Dispatch1(void) { TIMER_SERVICE_Dispatch(TIMER_1); }
static void TIMER_SERVICE_Dispatch2(void) { TIMER_SERVICE_Dispatch(TIMER_2); }

static const timer_callback_t timer_service_trampolines[TIMER_COUNT] =
{
    TIMER_SERVICE_Dispatch0,
    TIMER_SERVICE_Dispatch1,
    TIMER_SERVICE_Dispatch2
};

static uint8_t TIMER_SERVICE_Arm(
    timer_id_t timer,
    uint16_t milliseconds,
    timer_callback_t callback,
    uint8_t one_shot)
{
    timer_clock_t clock;
    uint16_t compare;

    if ((timer >= TIMER_COUNT) ||
        (callback == NULL) ||
        (milliseconds == 0U))
    {
        return 0U;
    }

    if (TIMER_SERVICE_ComputeTicks(timer, milliseconds, &clock, &compare) == 0U)
    {
        return 0U;
    }

    timer_service_slots[timer].callback = callback;
    timer_service_slots[timer].one_shot = one_shot;

    TIMER_Stop(timer);
    TIMER_SetMode(timer, TIMER_MODE_CTC);
    TIMER_SetCounter(timer, 0U);
    TIMER_SetCompare(timer, TIMER_COMPARE_A, compare);
    TIMER_ClearFlag(timer, TIMER_INTERRUPT_COMPARE_A);
    TIMER_RegisterCallback(timer, TIMER_INTERRUPT_COMPARE_A, timer_service_trampolines[timer]);
    TIMER_EnableInterrupt(timer, TIMER_INTERRUPT_COMPARE_A);
    TIMER_SetClock(timer, clock);

    return 1U;
}

/* -------------------------------------------------------------------------- */
/* Public API                                                                  */
/* -------------------------------------------------------------------------- */

uint8_t TIMER_SERVICE_DelayMs(
    timer_id_t timer,
    uint16_t milliseconds)
{
    timer_clock_t clock;
    uint16_t compare;
    uint16_t elapsed;

    if ((timer >= TIMER_COUNT) || (milliseconds == 0U))
    {
        return 0U;
    }

    /* One fixed 1ms tick, repeated `milliseconds` times by polling - this
     * keeps the math simple and works the same on 8-bit and 16-bit timers. */
    if (TIMER_SERVICE_ComputeTicks(timer, 1U, &clock, &compare) == 0U)
    {
        return 0U;
    }

    TIMER_Stop(timer);
    TIMER_DisableInterrupt(timer, TIMER_INTERRUPT_COMPARE_A);
    TIMER_SetMode(timer, TIMER_MODE_CTC);
    TIMER_SetCounter(timer, 0U);
    TIMER_SetCompare(timer, TIMER_COMPARE_A, compare);
    TIMER_ClearFlag(timer, TIMER_INTERRUPT_COMPARE_A);
    TIMER_SetClock(timer, clock);

    for (elapsed = 0U; elapsed < milliseconds; elapsed++)
    {
        while (TIMER_GetFlag(timer, TIMER_INTERRUPT_COMPARE_A) == 0U)
        {
            /* busy-wait for the next 1ms compare match */
        }

        TIMER_ClearFlag(timer, TIMER_INTERRUPT_COMPARE_A);
    }

    TIMER_Stop(timer);

    return 1U;
}

uint8_t TIMER_SERVICE_SetTimeout(
    timer_id_t timer,
    uint16_t milliseconds,
    timer_callback_t callback)
{
    return TIMER_SERVICE_Arm(timer, milliseconds, callback, 1U);
}

uint8_t TIMER_SERVICE_SetInterval(
    timer_id_t timer,
    uint16_t milliseconds,
    timer_callback_t callback)
{
    return TIMER_SERVICE_Arm(timer, milliseconds, callback, 0U);
}

void TIMER_SERVICE_Cancel(timer_id_t timer)
{
    if (timer >= TIMER_COUNT)
    {
        return;
    }

    TIMER_Stop(timer);
    TIMER_DisableInterrupt(timer, TIMER_INTERRUPT_COMPARE_A);
    TIMER_RegisterCallback(timer, TIMER_INTERRUPT_COMPARE_A, NULL);
    timer_service_slots[timer].callback = NULL;
}