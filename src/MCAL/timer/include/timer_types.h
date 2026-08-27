#ifndef TIMER_TYPES_H
#define TIMER_TYPES_H

#include <stdint.h>

/* -------------------------------------------------------------------------- */
/* Timer Instances                                                             */
/* -------------------------------------------------------------------------- */

typedef enum
{
    TIMER_0,
    TIMER_1,
    TIMER_2,

    TIMER_COUNT
} timer_id_t;

/* -------------------------------------------------------------------------- */
/* Waveform Generation Mode                                                    */
/* -------------------------------------------------------------------------- */

typedef enum
{
    TIMER_MODE_NORMAL,
    TIMER_MODE_PHASE_CORRECT_PWM,
    TIMER_MODE_CTC,
    TIMER_MODE_FAST_PWM,

    TIMER_MODE_COUNT
} timer_mode_t;

/* -------------------------------------------------------------------------- */
/* Clock Source                                                                */
/* -------------------------------------------------------------------------- */

typedef enum
{
    TIMER_CLOCK_STOP = 0,
    TIMER_CLOCK_1,
    TIMER_CLOCK_8,
    TIMER_CLOCK_64,
    TIMER_CLOCK_256,
    TIMER_CLOCK_1024,

    TIMER_CLOCK_COUNT
} timer_clock_t;

/* -------------------------------------------------------------------------- */
/* Compare Channels                                                            */
/* -------------------------------------------------------------------------- */

typedef enum
{
    TIMER_COMPARE_A,
    TIMER_COMPARE_B,

    TIMER_COMPARE_COUNT
} timer_compare_t;

/* -------------------------------------------------------------------------- */
/* Output Compare Mode (COM bits)                                              */
/* -------------------------------------------------------------------------- */

typedef enum
{
    TIMER_OUTPUT_DISCONNECTED,
    TIMER_OUTPUT_TOGGLE,
    TIMER_OUTPUT_CLEAR,
    TIMER_OUTPUT_SET
} timer_output_mode_t;

/* -------------------------------------------------------------------------- */
/* Interrupt Sources                                                           */
/* -------------------------------------------------------------------------- */

typedef enum
{
    TIMER_INTERRUPT_OVERFLOW,
    TIMER_INTERRUPT_COMPARE_A,
    TIMER_INTERRUPT_COMPARE_B,
    TIMER_INTERRUPT_INPUT_CAPTURE,

    TIMER_INTERRUPT_COUNT
} timer_interrupt_t;

/* -------------------------------------------------------------------------- */
/* Input Capture Edge                                                          */
/* -------------------------------------------------------------------------- */

typedef enum
{
    TIMER_CAPTURE_FALLING_EDGE,
    TIMER_CAPTURE_RISING_EDGE
} timer_capture_edge_t;

/* -------------------------------------------------------------------------- */
/* Callback Type                                                               */
/* -------------------------------------------------------------------------- */

typedef void (*timer_callback_t)(void);

/* -------------------------------------------------------------------------- */
/* Timer Configuration                                                         */
/* -------------------------------------------------------------------------- */

typedef struct
{
    timer_id_t timer;
    timer_mode_t mode;
    timer_clock_t clock;
} timer_config_t;

#endif /* TIMER_TYPES_H */