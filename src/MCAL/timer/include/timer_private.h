#ifndef TIMER_PRIVATE_H
#define TIMER_PRIVATE_H

#include <stdint.h>

#include "timer_types.h"

/* -------------------------------------------------------------------------- */
/* Sizing                                                                      */
/* -------------------------------------------------------------------------- */

#define TIMER_CONTROL_REGISTERS 2U
#define TIMER_WAVEFORM_MODES    TIMER_MODE_COUNT

/* Marks an interrupt/clock bit position as not present on a given timer */
#define TIMER_UNUSED_BIT 0xFFU

/* -------------------------------------------------------------------------- */
/* Register Width                                                              */
/* -------------------------------------------------------------------------- */

typedef enum
{
    TIMER_REGISTER_UNUSED,
    TIMER_REGISTER_8_BIT,
    TIMER_REGISTER_16_BIT
} timer_register_width_t;

/* -------------------------------------------------------------------------- */
/* Register Descriptor                                                         */
/* -------------------------------------------------------------------------- */

typedef struct
{
    volatile void *address;
    timer_register_width_t width;
} timer_register_t;

/* -------------------------------------------------------------------------- */
/* Timer Capabilities                                                          */
/* -------------------------------------------------------------------------- */

typedef enum
{
    TIMER_CAP_COMPARE_A      = (1U << 0),
    TIMER_CAP_COMPARE_B      = (1U << 1),
    TIMER_CAP_INPUT_CAPTURE  = (1U << 2),
    TIMER_CAP_ASYNC          = (1U << 3)
} timer_capability_t;

/* -------------------------------------------------------------------------- */
/* Timer Descriptor                                                            */
/* -------------------------------------------------------------------------- */

typedef struct
{
    /* Registers */

    timer_register_t control[2];

    timer_register_t counter;

    timer_register_t compare[TIMER_COMPARE_COUNT];

    timer_register_t interrupt_mask;

    timer_register_t interrupt_flag;

    /* WGM */

    uint8_t waveform_mask[2];

    uint8_t waveform_value[4][2];

    /* COM */

    uint8_t compare_output_mask[TIMER_COMPARE_COUNT];

    uint8_t compare_output_shift[TIMER_COMPARE_COUNT];

    /* Clock */

    uint8_t clock_register;

    uint8_t clock_control_index;

    uint8_t clock_mask;

    uint8_t clock_shift;

    uint8_t clock_bits[TIMER_CLOCK_COUNT];

    /* Interrupts */

    uint8_t interrupt_bits[TIMER_INTERRUPT_COUNT];

    /* Counter */

    uint16_t max_counter;

    /* Capabilities */

    uint8_t capabilities;

} timer_descriptor_t;

#endif