#ifndef EXT_INT_H
#define EXT_INT_H

#include <stdint.h>

#include "ext_int_types.h"

/* -------------------------------------------------------------------------- */
/* Global Interrupt Enable / Disable                                           */
/* -------------------------------------------------------------------------- */

void EXT_INT_GlobalEnable(void);

void EXT_INT_GlobalDisable(void);

/* -------------------------------------------------------------------------- */
/* Configuration                                                               */
/* -------------------------------------------------------------------------- */

void EXT_INT_Init(
    const ext_int_config_t *config);

void EXT_INT_SetTrigger(
    ext_int_id_t line,
    ext_int_trigger_t trigger);

/* -------------------------------------------------------------------------- */
/* Enable / Disable                                                            */
/* -------------------------------------------------------------------------- */

void EXT_INT_Enable(
    ext_int_id_t line);

void EXT_INT_Disable(
    ext_int_id_t line);

/* -------------------------------------------------------------------------- */
/* Flags                                                                       */
/* -------------------------------------------------------------------------- */

void EXT_INT_ClearFlag(
    ext_int_id_t line);

uint8_t EXT_INT_GetFlag(
    ext_int_id_t line);

/* -------------------------------------------------------------------------- */
/* Callback                                                                    */
/* -------------------------------------------------------------------------- */

/* Registers `callback` to be invoked from the driver's own ISR when `line`
 * fires. Pass NULL to detach. Mirrors TIMER_RegisterCallback - callers
 * never need to know AVR vector names. */
void EXT_INT_RegisterCallback(
    ext_int_id_t line,
    ext_int_callback_t callback);

#endif /* EXT_INT_H */