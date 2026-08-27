#include "timer.h"

#include <stddef.h>

#include <avr/interrupt.h>

#include "timer_private.h"
#include "timer_registers.h"

/* One callback slot per (timer, interrupt source). The driver's own ISRs
 * dispatch into these, so callers of TIMER_RegisterCallback never need to
 * know AVR vector names or touch avr/interrupt.h themselves. */
static timer_callback_t timer_callbacks[TIMER_COUNT][TIMER_INTERRUPT_COUNT];

static void TIMER_Dispatch(timer_id_t timer, timer_interrupt_t interrupt)
{
    timer_callback_t callback = timer_callbacks[timer][interrupt];

    if (callback != NULL)
    {
        callback();
    }
}

void TIMER_RegisterCallback(
    timer_id_t timer,
    timer_interrupt_t interrupt,
    timer_callback_t callback)
{
    if ((timer >= TIMER_COUNT) ||
        (interrupt >= TIMER_INTERRUPT_COUNT))
    {
        return;
    }

    timer_callbacks[timer][interrupt] = callback;
}

uint16_t TIMER_GetMaxCount(timer_id_t timer)
{
    const timer_descriptor_t *descriptor = TIMER_RegistersGet(timer);

    if (descriptor == NULL)
    {
        return 0U;
    }

    return descriptor->max_counter;
}

/* -------------------------------------------------------------------------- */
/* ISRs - the only place in the codebase that knows AVR vector names.         */
/* AVR clears these flags in hardware on ISR entry, so no ClearFlag call is   */
/* needed here (ClearFlag exists for the polling use case).                  */
/* -------------------------------------------------------------------------- */

ISR(TIMER0_OVF_vect)
{
    TIMER_Dispatch(TIMER_0, TIMER_INTERRUPT_OVERFLOW);
}

ISR(TIMER0_COMP_vect)
{
    TIMER_Dispatch(TIMER_0, TIMER_INTERRUPT_COMPARE_A);
}

ISR(TIMER1_OVF_vect)
{
    TIMER_Dispatch(TIMER_1, TIMER_INTERRUPT_OVERFLOW);
}

ISR(TIMER1_COMPA_vect)
{
    TIMER_Dispatch(TIMER_1, TIMER_INTERRUPT_COMPARE_A);
}

ISR(TIMER1_COMPB_vect)
{
    TIMER_Dispatch(TIMER_1, TIMER_INTERRUPT_COMPARE_B);
}

ISR(TIMER1_CAPT_vect)
{
    TIMER_Dispatch(TIMER_1, TIMER_INTERRUPT_INPUT_CAPTURE);
}

ISR(TIMER2_OVF_vect)
{
    TIMER_Dispatch(TIMER_2, TIMER_INTERRUPT_OVERFLOW);
}

ISR(TIMER2_COMP_vect)
{
    TIMER_Dispatch(TIMER_2, TIMER_INTERRUPT_COMPARE_A);
}

static const timer_descriptor_t *TIMER_GetDescriptor(timer_id_t timer)
{
    return TIMER_RegistersGet(timer);
}

/* Remembers the last clock source configured via TIMER_SetClock so that
 * TIMER_Start can restore it after TIMER_Stop has zeroed the prescaler
 * bits. Defaults to TIMER_CLOCK_STOP until TIMER_SetClock is called. */
static timer_clock_t timer_configured_clock[TIMER_COUNT];

static void TIMER_WriteClockBits(
    const timer_descriptor_t *descriptor,
    timer_clock_t clock)
{
    TIMER_Update(
        &descriptor->control[descriptor->clock_control_index],
        descriptor->clock_mask,
        (uint8_t)(descriptor->clock_bits[clock] << descriptor->clock_shift));
}

void TIMER_Reset(timer_id_t timer)
{
    const timer_descriptor_t *descriptor = TIMER_GetDescriptor(timer);

    if (descriptor == NULL)
    {
        return;
    }

    for (uint8_t i = 0U; i < TIMER_CONTROL_REGISTERS; i++)
    {
        TIMER_Write(&descriptor->control[i], 0U);
    }

    TIMER_Write(&descriptor->counter, 0U);
}

void TIMER_Init(const timer_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    TIMER_Reset(config->timer);
    TIMER_SetMode(config->timer, config->mode);
    TIMER_SetClock(config->timer, config->clock);
}

void TIMER_SetMode(
    timer_id_t timer,
    timer_mode_t mode)
{
    const timer_descriptor_t *descriptor = TIMER_GetDescriptor(timer);

    if ((descriptor == NULL) ||
        (mode >= TIMER_WAVEFORM_MODES))
    {
        return;
    }

    for (uint8_t i = 0U; i < TIMER_CONTROL_REGISTERS; i++)
    {
        TIMER_Update(
            &descriptor->control[i],
            descriptor->waveform_mask[i],
            descriptor->waveform_value[mode][i]);
    }
}

void TIMER_SetClock(
    timer_id_t timer,
    timer_clock_t clock)
{
    const timer_descriptor_t *descriptor = TIMER_GetDescriptor(timer);

    if ((descriptor == NULL) ||
        (clock >= TIMER_CLOCK_COUNT))
    {
        return;
    }

    timer_configured_clock[timer] = clock;

    TIMER_WriteClockBits(descriptor, clock);
}

void TIMER_Start(timer_id_t timer)
{
    const timer_descriptor_t *descriptor = TIMER_GetDescriptor(timer);

    if (descriptor == NULL)
    {
        return;
    }

    TIMER_WriteClockBits(descriptor, timer_configured_clock[timer]);
}

void TIMER_Stop(timer_id_t timer)
{
    const timer_descriptor_t *descriptor = TIMER_GetDescriptor(timer);

    if (descriptor == NULL)
    {
        return;
    }

    /* Zero the prescaler bits directly (not via TIMER_SetClock) so the
     * remembered clock survives for the next TIMER_Start. */
    TIMER_WriteClockBits(descriptor, TIMER_CLOCK_STOP);
}

void TIMER_SetCounter(
    timer_id_t timer,
    uint16_t value)
{
    const timer_descriptor_t *descriptor = TIMER_GetDescriptor(timer);

    if (descriptor == NULL)
    {
        return;
    }

    TIMER_Write(&descriptor->counter, value);
}

uint16_t TIMER_GetCounter(timer_id_t timer)
{
    const timer_descriptor_t *descriptor = TIMER_GetDescriptor(timer);

    if (descriptor == NULL)
    {
        return 0U;
    }

    return TIMER_Read(&descriptor->counter);
}

/* -------------------------------------------------------------------------- */
/* Compare                                                                     */
/* -------------------------------------------------------------------------- */

void TIMER_SetCompare(
    timer_id_t timer,
    timer_compare_t compare,
    uint16_t value)
{
    const timer_descriptor_t *descriptor = TIMER_GetDescriptor(timer);

    if ((descriptor == NULL) ||
        (compare >= TIMER_COMPARE_COUNT))
    {
        return;
    }

    TIMER_Write(&descriptor->compare[compare], value);
}

uint16_t TIMER_GetCompare(
    timer_id_t timer,
    timer_compare_t compare)
{
    const timer_descriptor_t *descriptor = TIMER_GetDescriptor(timer);

    if ((descriptor == NULL) ||
        (compare >= TIMER_COMPARE_COUNT))
    {
        return 0U;
    }

    return TIMER_Read(&descriptor->compare[compare]);
}

/* -------------------------------------------------------------------------- */
/* Output Compare                                                              */
/* -------------------------------------------------------------------------- */

void TIMER_SetOutputMode(
    timer_id_t timer,
    timer_compare_t compare,
    timer_output_mode_t mode)
{
    const timer_descriptor_t *descriptor = TIMER_GetDescriptor(timer);

    if ((descriptor == NULL) ||
        (compare >= TIMER_COMPARE_COUNT))
    {
        return;
    }

    /* COM bits for both compare channels live in control[0] on every
     * ATmega32 timer (TCCR0 / TCCR1A / TCCR2). */
    TIMER_Update(
        &descriptor->control[0],
        descriptor->compare_output_mask[compare],
        (uint8_t)((uint8_t)mode << descriptor->compare_output_shift[compare]));
}

/* -------------------------------------------------------------------------- */
/* Interrupts                                                                  */
/* -------------------------------------------------------------------------- */

void TIMER_EnableInterrupt(
    timer_id_t timer,
    timer_interrupt_t interrupt)
{
    const timer_descriptor_t *descriptor = TIMER_GetDescriptor(timer);
    uint8_t bit;

    if ((descriptor == NULL) ||
        (interrupt >= TIMER_INTERRUPT_COUNT))
    {
        return;
    }

    bit = descriptor->interrupt_bits[interrupt];

    if (bit == TIMER_UNUSED_BIT)
    {
        return;
    }

    TIMER_Update(
        &descriptor->interrupt_mask,
        (uint8_t)(1U << bit),
        (uint8_t)(1U << bit));
}

void TIMER_DisableInterrupt(
    timer_id_t timer,
    timer_interrupt_t interrupt)
{
    const timer_descriptor_t *descriptor = TIMER_GetDescriptor(timer);
    uint8_t bit;

    if ((descriptor == NULL) ||
        (interrupt >= TIMER_INTERRUPT_COUNT))
    {
        return;
    }

    bit = descriptor->interrupt_bits[interrupt];

    if (bit == TIMER_UNUSED_BIT)
    {
        return;
    }

    TIMER_Update(
        &descriptor->interrupt_mask,
        (uint8_t)(1U << bit),
        0U);
}

void TIMER_ClearFlag(
    timer_id_t timer,
    timer_interrupt_t interrupt)
{
    const timer_descriptor_t *descriptor = TIMER_GetDescriptor(timer);
    uint8_t bit;

    if ((descriptor == NULL) ||
        (interrupt >= TIMER_INTERRUPT_COUNT))
    {
        return;
    }

    bit = descriptor->interrupt_bits[interrupt];

    if (bit == TIMER_UNUSED_BIT)
    {
        return;
    }

    /* TIFR is write-1-to-clear: write ONLY the target bit, not a
     * read-modify-write, or any other pending flags would be cleared too. */
    TIMER_Write(&descriptor->interrupt_flag, (uint16_t)(1U << bit));
}

uint8_t TIMER_GetFlag(
    timer_id_t timer,
    timer_interrupt_t interrupt)
{
    const timer_descriptor_t *descriptor = TIMER_GetDescriptor(timer);
    uint8_t bit;

    if ((descriptor == NULL) ||
        (interrupt >= TIMER_INTERRUPT_COUNT))
    {
        return 0U;
    }

    bit = descriptor->interrupt_bits[interrupt];

    if (bit == TIMER_UNUSED_BIT)
    {
        return 0U;
    }

    return (uint8_t)((TIMER_Read(&descriptor->interrupt_flag) >> bit) & 0x01U);
}