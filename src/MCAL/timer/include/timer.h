#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#include "timer_types.h"

void TIMER_Init(const timer_config_t *config);

void TIMER_Reset(timer_id_t timer);

void TIMER_Start(timer_id_t timer);

void TIMER_Stop(timer_id_t timer);

/* Counter */

void TIMER_SetCounter(
    timer_id_t timer,
    uint16_t value);

uint16_t TIMER_GetCounter(
    timer_id_t timer);

/* Compare */

void TIMER_SetCompare(
    timer_id_t timer,
    timer_compare_t compare,
    uint16_t value);

uint16_t TIMER_GetCompare(
    timer_id_t timer,
    timer_compare_t compare);

/* Waveform */

void TIMER_SetMode(
    timer_id_t timer,
    timer_mode_t mode);

/* Clock */

void TIMER_SetClock(
    timer_id_t timer,
    timer_clock_t clock);

/* Output Compare */

void TIMER_SetOutputMode(
    timer_id_t timer,
    timer_compare_t compare,
    timer_output_mode_t mode);

/* Interrupt */

void TIMER_EnableInterrupt(
    timer_id_t timer,
    timer_interrupt_t interrupt);

void TIMER_DisableInterrupt(
    timer_id_t timer,
    timer_interrupt_t interrupt);

void TIMER_ClearFlag(
    timer_id_t timer,
    timer_interrupt_t interrupt);

uint8_t TIMER_GetFlag(
    timer_id_t timer,
    timer_interrupt_t interrupt);

/* Registers `callback` to be invoked from the driver's own ISR when
 * `interrupt` fires on `timer`. Pass NULL to detach. This is what keeps
 * HAL/application code from ever needing to know AVR vector names. */
void TIMER_RegisterCallback(
    timer_id_t timer,
    timer_interrupt_t interrupt,
    timer_callback_t callback);

/* Top value of the timer's counter/compare registers (0xFF for the 8-bit
 * timers, 0xFFFF for Timer1). Lets HAL code pick a prescaler/compare value
 * without needing to know which timers are 8-bit vs 16-bit. */
uint16_t TIMER_GetMaxCount(timer_id_t timer);

#endif