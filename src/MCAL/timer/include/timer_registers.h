#ifndef TIMER_REGISTERS_H
#define TIMER_REGISTERS_H

#include <stdint.h>

#include "timer_private.h"

/* -------------------------------------------------------------------------- */
/* Timer Descriptor                                                            */
/* -------------------------------------------------------------------------- */

const timer_descriptor_t *TIMER_RegistersGet(
    timer_id_t timer);

/* -------------------------------------------------------------------------- */
/* Register Access                                                             */
/* -------------------------------------------------------------------------- */

uint16_t TIMER_Read(
    const timer_register_t *reg);

void TIMER_Write(
    const timer_register_t *reg,
    uint16_t value);

void TIMER_Update(
    const timer_register_t *reg,
    uint8_t mask,
    uint8_t value);

#endif /* TIMER_REGISTERS_H */