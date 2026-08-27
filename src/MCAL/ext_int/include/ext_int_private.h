#ifndef EXT_INT_PRIVATE_H
#define EXT_INT_PRIVATE_H

#include <stdint.h>

#include "ext_int_types.h"

/* -------------------------------------------------------------------------- */
/* Sizing                                                                      */
/* -------------------------------------------------------------------------- */

/* Marks a control bit as not present, or a trigger mode as unsupported, on
 * a given line (mirrors TIMER_UNUSED_BIT in the Timer driver). */
#define EXT_INT_UNUSED_BIT 0xFFU

/* -------------------------------------------------------------------------- */
/* Line Descriptor                                                             */
/* -------------------------------------------------------------------------- */

typedef struct
{
    /* Trigger control (ISCn1:ISCn0 on INT0/INT1, ISC2 alone on INT2) */

    volatile uint8_t *control_reg;

    uint8_t trigger_bit0;
    uint8_t trigger_bit1; /* EXT_INT_UNUSED_BIT on lines with only one control bit (INT2) */

    /* 2-bit code (0-3) per trigger mode, or EXT_INT_UNUSED_BIT if that mode
     * isn't representable on this line at all (e.g. LOW_LEVEL on INT2). */
    uint8_t trigger_code[EXT_INT_TRIGGER_COUNT];

    /* Enable (GICR) */

    volatile uint8_t *enable_reg;
    uint8_t enable_bit;

    /* Flag (GIFR) */

    volatile uint8_t *flag_reg;
    uint8_t flag_bit;

} ext_int_descriptor_t;

#endif /* EXT_INT_PRIVATE_H */