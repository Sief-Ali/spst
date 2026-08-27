#include "ext_int.h"

#include <stddef.h>

#include <avr/interrupt.h>

#include "bit_utils.h"
#include "ext_int_private.h"
#include "ext_int_registers.h"

/* -------------------------------------------------------------------------- */
/* Callback Dispatch                                                           */
/*                                                                              */
/* One callback slot per line. The driver's own ISRs dispatch into these, so */
/* callers of EXT_INT_RegisterCallback never need to know AVR vector names   */
/* or touch avr/interrupt.h themselves (mirrors the Timer driver).           */
/* -------------------------------------------------------------------------- */

static ext_int_callback_t ext_int_callbacks[EXT_INT_COUNT];

static void EXT_INT_Dispatch(ext_int_id_t line)
{
    ext_int_callback_t callback = ext_int_callbacks[line];

    if (callback != NULL)
    {
        callback();
    }
}

void EXT_INT_RegisterCallback(
    ext_int_id_t line,
    ext_int_callback_t callback)
{
    if (line >= EXT_INT_COUNT)
    {
        return;
    }

    ext_int_callbacks[line] = callback;
}

/* -------------------------------------------------------------------------- */
/* Global Enable / Disable                                                     */
/* -------------------------------------------------------------------------- */

void EXT_INT_GlobalEnable(void)
{
    sei();
}

void EXT_INT_GlobalDisable(void)
{
    cli();
}

/* -------------------------------------------------------------------------- */
/* Trigger Configuration                                                       */
/* -------------------------------------------------------------------------- */

static void EXT_INT_ApplyBit(
    volatile uint8_t *reg,
    uint8_t bit,
    uint8_t value)
{
    if ((reg == NULL) || (bit == EXT_INT_UNUSED_BIT))
    {
        return;
    }

    if (value != 0U)
    {
        SET_BIT(*reg, bit);
    }
    else
    {
        CLEAR_BIT(*reg, bit);
    }
}

void EXT_INT_SetTrigger(
    ext_int_id_t line,
    ext_int_trigger_t trigger)
{
    const ext_int_descriptor_t *descriptor = EXT_INT_RegistersGet(line);
    uint8_t code;

    if ((descriptor == NULL) || (trigger >= EXT_INT_TRIGGER_COUNT))
    {
        return;
    }

    code = descriptor->trigger_code[trigger];

    if (code == EXT_INT_UNUSED_BIT)
    {
        return; /* trigger mode not supported on this line */
    }

    EXT_INT_ApplyBit(descriptor->control_reg, descriptor->trigger_bit0, (uint8_t)(code & 0x01U));
    EXT_INT_ApplyBit(descriptor->control_reg, descriptor->trigger_bit1, (uint8_t)((code >> 1) & 0x01U));
}

void EXT_INT_Init(const ext_int_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    EXT_INT_SetTrigger(config->line, config->trigger);
}

/* -------------------------------------------------------------------------- */
/* Enable / Disable                                                            */
/* -------------------------------------------------------------------------- */

void EXT_INT_Enable(ext_int_id_t line)
{
    const ext_int_descriptor_t *descriptor = EXT_INT_RegistersGet(line);

    if (descriptor == NULL)
    {
        return;
    }

    SET_BIT(*descriptor->enable_reg, descriptor->enable_bit);
}

void EXT_INT_Disable(ext_int_id_t line)
{
    const ext_int_descriptor_t *descriptor = EXT_INT_RegistersGet(line);

    if (descriptor == NULL)
    {
        return;
    }

    CLEAR_BIT(*descriptor->enable_reg, descriptor->enable_bit);
}

/* -------------------------------------------------------------------------- */
/* Flags                                                                       */
/* -------------------------------------------------------------------------- */

void EXT_INT_ClearFlag(ext_int_id_t line)
{
    const ext_int_descriptor_t *descriptor = EXT_INT_RegistersGet(line);

    if (descriptor == NULL)
    {
        return;
    }

    /* GIFR is write-1-to-clear, exactly like TIFR in the Timer driver.
     * SET_BIT does a read-modify-write (|=), which would read back any
     * OTHER pending flags in GIFR and write them back as 1, clearing them
     * too. Only a direct write of just this bit is correct here - same
     * reasoning as TIMER_ClearFlag. */
    *descriptor->flag_reg = (uint8_t)(1U << descriptor->flag_bit);
}

uint8_t EXT_INT_GetFlag(ext_int_id_t line)
{
    const ext_int_descriptor_t *descriptor = EXT_INT_RegistersGet(line);

    if (descriptor == NULL)
    {
        return 0U;
    }

    return (uint8_t)READ_BIT(*descriptor->flag_reg, descriptor->flag_bit);
}

/* -------------------------------------------------------------------------- */
/* ISRs - the only place in this driver that knows AVR vector names.          */
/* For edge-triggered modes, hardware clears the flag automatically when the */
/* ISR executes, so no ClearFlag call is needed here. ClearFlag exists for   */
/* polling-based use instead.                                                */
/* -------------------------------------------------------------------------- */

ISR(INT0_vect)
{
    EXT_INT_Dispatch(EXT_INT_0);
}

ISR(INT1_vect)
{
    EXT_INT_Dispatch(EXT_INT_1);
}

ISR(INT2_vect)
{
    EXT_INT_Dispatch(EXT_INT_2);
}