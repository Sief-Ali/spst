#include "timer_registers.h"

#include <avr/io.h>
#include <stddef.h>

/* -------------------------------------------------------------------------- */
/* Register Helpers                                                            */
/* -------------------------------------------------------------------------- */

#define TIMER_REG8(REG) \
    { (volatile void *)&(REG), TIMER_REGISTER_8_BIT }

#define TIMER_REG16(REG) \
    { (volatile void *)&(REG), TIMER_REGISTER_16_BIT }

#define TIMER_NO_REG \
    { NULL, TIMER_REGISTER_UNUSED }

/* -------------------------------------------------------------------------- */
/* Register Access                                                             */
/* -------------------------------------------------------------------------- */

static uint8_t TIMER_RegisterIsValid(const timer_register_t *reg)
{
    return ((reg != NULL) &&
            (reg->address != NULL) &&
            (reg->width != TIMER_REGISTER_UNUSED));
}

uint16_t TIMER_Read(const timer_register_t *reg)
{
    if (TIMER_RegisterIsValid(reg) == 0U)
    {
        return 0U;
    }

    if (reg->width == TIMER_REGISTER_16_BIT)
    {
        return *(volatile uint16_t *)reg->address;
    }

    return *(volatile uint8_t *)reg->address;
}

void TIMER_Write(
    const timer_register_t *reg,
    uint16_t value)
{
    if (TIMER_RegisterIsValid(reg) == 0U)
    {
        return;
    }

    if (reg->width == TIMER_REGISTER_16_BIT)
    {
        *(volatile uint16_t *)reg->address = value;
    }
    else
    {
        *(volatile uint8_t *)reg->address = (uint8_t)value;
    }
}

void TIMER_Update(
    const timer_register_t *reg,
    uint8_t mask,
    uint8_t value)
{
    uint8_t current;

    if (TIMER_RegisterIsValid(reg) == 0U)
    {
        return;
    }

    current = TIMER_Read(reg);

    current &= (uint8_t)~mask;
    current |= (uint8_t)(value & mask);

    TIMER_Write(reg, current);
}

/* -------------------------------------------------------------------------- */
/* Timer Descriptors                                                           */
/* -------------------------------------------------------------------------- */

static const timer_descriptor_t timer_descriptors[TIMER_COUNT] =
{
    [TIMER_0] =
    {
        /* Registers */

        .control =
        {
            TIMER_REG8(TCCR0),
            TIMER_NO_REG
        },

        .counter = TIMER_REG8(TCNT0),

        .compare =
        {
            TIMER_REG8(OCR0),
            TIMER_NO_REG
        },

        .interrupt_mask = TIMER_REG8(TIMSK),
        .interrupt_flag = TIMER_REG8(TIFR),

        /* Waveform Generation */

        .waveform_mask =
        {
            (uint8_t)((1U << WGM00) | (1U << WGM01)),
            0U
        },

        .waveform_value =
        {
            /* NORMAL */
            { 0U, 0U },

            /* PHASE CORRECT PWM */
            { (uint8_t)(1U << WGM00), 0U },

            /* CTC */
            { (uint8_t)(1U << WGM01), 0U },

            /* FAST PWM */
            {
                (uint8_t)((1U << WGM00) | (1U << WGM01)),
                0U
            }
        },

        /* Compare Output */

        .compare_output_mask =
        {
            (uint8_t)((1U << COM00) | (1U << COM01)),
            0U
        },

        .compare_output_shift =
        {
            COM00,
            0U
        },

        /* Clock */

        .clock_register = 0U,

        .clock_control_index = 0U,

        .clock_mask =
            (uint8_t)((1U << CS00) |
                      (1U << CS01) |
                      (1U << CS02)),

        .clock_shift = CS00,

        .clock_bits =
        {
            [TIMER_CLOCK_STOP] = 0U,
            [TIMER_CLOCK_1]    = 1U,
            [TIMER_CLOCK_8]    = 2U,
            [TIMER_CLOCK_64]   = 3U,
            [TIMER_CLOCK_256]  = 4U,
            [TIMER_CLOCK_1024] = 5U
        },

        /* Interrupts */

        .interrupt_bits =
        {
            [TIMER_INTERRUPT_OVERFLOW]     = TOIE0,
            [TIMER_INTERRUPT_COMPARE_A]    = OCIE0,
            [TIMER_INTERRUPT_COMPARE_B]    = TIMER_UNUSED_BIT,
            [TIMER_INTERRUPT_INPUT_CAPTURE]= TIMER_UNUSED_BIT
        },

        /* Hardware */

        .max_counter = UINT8_MAX,

        .capabilities = TIMER_CAP_COMPARE_A
    },

    [TIMER_1] =
    {
        /* Registers */

        .control =
        {
            TIMER_REG8(TCCR1A),
            TIMER_REG8(TCCR1B)
        },

        .counter = TIMER_REG16(TCNT1),

        .compare =
        {
            TIMER_REG16(OCR1A),
            TIMER_REG16(OCR1B)
        },

        .interrupt_mask = TIMER_REG8(TIMSK),
        .interrupt_flag = TIMER_REG8(TIFR),

        /* Waveform Generation
         *
         * Only the four "generic" modes are exposed here (Normal, Phase
         * Correct PWM, CTC and Fast PWM), each in their simplest 8-bit-top
         * form. TCCR1A carries WGM11:WGM10, TCCR1B carries WGM13:WGM12.
         */

        .waveform_mask =
        {
            (uint8_t)((1U << WGM10) | (1U << WGM11)),
            (uint8_t)((1U << WGM12) | (1U << WGM13))
        },

        .waveform_value =
        {
            /* NORMAL */
            { 0U, 0U },

            /* PHASE CORRECT PWM (8-bit) */
            { (uint8_t)(1U << WGM10), 0U },

            /* CTC (TOP = OCR1A) */
            { 0U, (uint8_t)(1U << WGM12) },

            /* FAST PWM (8-bit) */
            { (uint8_t)(1U << WGM10), (uint8_t)(1U << WGM12) }
        },

        /* Compare Output (both channels live in TCCR1A) */

        .compare_output_mask =
        {
            (uint8_t)((1U << COM1A0) | (1U << COM1A1)),
            (uint8_t)((1U << COM1B0) | (1U << COM1B1))
        },

        .compare_output_shift =
        {
            COM1A0,
            COM1B0
        },

        /* Clock */

        .clock_register = 0U,

        .clock_control_index = 1U,

        .clock_mask =
            (uint8_t)((1U << CS10) |
                      (1U << CS11) |
                      (1U << CS12)),

        .clock_shift = CS10,

        .clock_bits =
        {
            [TIMER_CLOCK_STOP] = 0U,
            [TIMER_CLOCK_1]    = 1U,
            [TIMER_CLOCK_8]    = 2U,
            [TIMER_CLOCK_64]   = 3U,
            [TIMER_CLOCK_256]  = 4U,
            [TIMER_CLOCK_1024] = 5U
        },

        /* Interrupts */

        .interrupt_bits =
        {
            [TIMER_INTERRUPT_OVERFLOW]     = TOIE1,
            [TIMER_INTERRUPT_COMPARE_A]    = OCIE1A,
            [TIMER_INTERRUPT_COMPARE_B]    = OCIE1B,
            [TIMER_INTERRUPT_INPUT_CAPTURE]= TICIE1
        },

        /* Hardware */

        .max_counter = UINT16_MAX,

        .capabilities = (uint8_t)(TIMER_CAP_COMPARE_A |
                                   TIMER_CAP_COMPARE_B |
                                   TIMER_CAP_INPUT_CAPTURE)
    },

    [TIMER_2] =
    {
        /* Registers */

        .control =
        {
            TIMER_REG8(TCCR2),
            TIMER_NO_REG
        },

        .counter = TIMER_REG8(TCNT2),

        .compare =
        {
            TIMER_REG8(OCR2),
            TIMER_NO_REG
        },

        .interrupt_mask = TIMER_REG8(TIMSK),
        .interrupt_flag = TIMER_REG8(TIFR),

        /* Waveform Generation (WGM21/WGM20 are not adjacent on TCCR2) */

        .waveform_mask =
        {
            (uint8_t)((1U << WGM20) | (1U << WGM21)),
            0U
        },

        .waveform_value =
        {
            /* NORMAL */
            { 0U, 0U },

            /* PHASE CORRECT PWM */
            { (uint8_t)(1U << WGM20), 0U },

            /* CTC */
            { (uint8_t)(1U << WGM21), 0U },

            /* FAST PWM */
            {
                (uint8_t)((1U << WGM20) | (1U << WGM21)),
                0U
            }
        },

        /* Compare Output */

        .compare_output_mask =
        {
            (uint8_t)((1U << COM20) | (1U << COM21)),
            0U
        },

        .compare_output_shift =
        {
            COM20,
            0U
        },

        /* Clock
         *
         * Timer2's prescaler has its own encoding (it can also run from
         * the external 32kHz crystal), so /1, /8, /64, /256 and /1024
         * live on different bit patterns than Timer0/Timer1. The /32 and
         * /128 taps aren't reachable through timer_clock_t.
         */

        .clock_register = 0U,

        .clock_control_index = 0U,

        .clock_mask =
            (uint8_t)((1U << CS20) |
                      (1U << CS21) |
                      (1U << CS22)),

        .clock_shift = CS20,

        .clock_bits =
        {
            [TIMER_CLOCK_STOP] = 0U,
            [TIMER_CLOCK_1]    = 1U,
            [TIMER_CLOCK_8]    = 2U,
            [TIMER_CLOCK_64]   = 4U,
            [TIMER_CLOCK_256]  = 6U,
            [TIMER_CLOCK_1024] = 7U
        },

        /* Interrupts */

        .interrupt_bits =
        {
            [TIMER_INTERRUPT_OVERFLOW]     = TOIE2,
            [TIMER_INTERRUPT_COMPARE_A]    = OCIE2,
            [TIMER_INTERRUPT_COMPARE_B]    = TIMER_UNUSED_BIT,
            [TIMER_INTERRUPT_INPUT_CAPTURE]= TIMER_UNUSED_BIT
        },

        /* Hardware */

        .max_counter = UINT8_MAX,

        .capabilities = (uint8_t)(TIMER_CAP_COMPARE_A | TIMER_CAP_ASYNC)
    }
};

/* -------------------------------------------------------------------------- */
/* Timer Descriptor Access                                                     */
/* -------------------------------------------------------------------------- */

const timer_descriptor_t *TIMER_RegistersGet(timer_id_t timer)
{
    if (timer >= TIMER_COUNT)
    {
        return NULL;
    }

    return &timer_descriptors[timer];
}
