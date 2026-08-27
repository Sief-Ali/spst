#include "ext_int_registers.h"

#include <avr/io.h>
#include <stddef.h>

/* -------------------------------------------------------------------------- */
/* Descriptor Table                                                            */
/*                                                                              */
/* Unlike the Timer driver's register layer, there's no generic Read/Write/   */
/* Update here: every register touched by this driver (MCUCR, MCUCSR, GICR,  */
/* GIFR) is a plain 8-bit AVR register, so bit_utils.h's SET_BIT/CLEAR_BIT/   */
/* READ_BIT are used directly in ext_int.c against the addresses stored      */
/* here - no width polymorphism to abstract over.                            */
/* -------------------------------------------------------------------------- */

static const ext_int_descriptor_t ext_int_descriptors[EXT_INT_COUNT] =
{
    [EXT_INT_0] =
    {
        .control_reg = &MCUCR,
        .trigger_bit0 = ISC00,
        .trigger_bit1 = ISC01,

        .trigger_code =
        {
            [EXT_INT_TRIGGER_LOW_LEVEL]    = 0U,
            [EXT_INT_TRIGGER_ANY_CHANGE]   = 1U,
            [EXT_INT_TRIGGER_FALLING_EDGE] = 2U,
            [EXT_INT_TRIGGER_RISING_EDGE]  = 3U
        },

        .enable_reg = &GICR,
        .enable_bit = INT0,

        .flag_reg = &GIFR,
        .flag_bit = INTF0
    },

    [EXT_INT_1] =
    {
        .control_reg = &MCUCR,
        .trigger_bit0 = ISC10,
        .trigger_bit1 = ISC11,

        .trigger_code =
        {
            [EXT_INT_TRIGGER_LOW_LEVEL]    = 0U,
            [EXT_INT_TRIGGER_ANY_CHANGE]   = 1U,
            [EXT_INT_TRIGGER_FALLING_EDGE] = 2U,
            [EXT_INT_TRIGGER_RISING_EDGE]  = 3U
        },

        .enable_reg = &GICR,
        .enable_bit = INT1,

        .flag_reg = &GIFR,
        .flag_bit = INTF1
    },

    [EXT_INT_2] =
    {
        /* INT2 has only one control bit (ISC2, and it lives in MCUCSR, not
         * MCUCR) and only supports falling/rising edge - no low-level or
         * any-change on this line. */
        .control_reg = &MCUCSR,
        .trigger_bit0 = ISC2,
        .trigger_bit1 = EXT_INT_UNUSED_BIT,

        .trigger_code =
        {
            [EXT_INT_TRIGGER_LOW_LEVEL]    = EXT_INT_UNUSED_BIT,
            [EXT_INT_TRIGGER_ANY_CHANGE]   = EXT_INT_UNUSED_BIT,
            [EXT_INT_TRIGGER_FALLING_EDGE] = 0U,
            [EXT_INT_TRIGGER_RISING_EDGE]  = 1U
        },

        .enable_reg = &GICR,
        .enable_bit = INT2,

        .flag_reg = &GIFR,
        .flag_bit = INTF2
    }
};

/* -------------------------------------------------------------------------- */
/* Descriptor Access                                                           */
/* -------------------------------------------------------------------------- */

const ext_int_descriptor_t *EXT_INT_RegistersGet(ext_int_id_t line)
{
    if (line >= EXT_INT_COUNT)
    {
        return NULL;
    }

    return &ext_int_descriptors[line];
}