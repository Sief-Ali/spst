#ifndef BTN_H
#define BTN_H

#include <stdint.h>

#include "gpio.h"
#include "ext_int.h"
#include "ext_int_types.h"

typedef enum
{
    BTN_RELEASED,
    BTN_PRESSED
} btn_state_t;

typedef void (*btn_callback_t)(void);

typedef struct
{
    gpio_pin_t pin;
    pullup_state_t pullup;
} btn_t;

/* -------------------------------------------------------------------------- */
/* Basic Button                                                                */
/* -------------------------------------------------------------------------- */

void BTN_Init(const btn_t *btn);

btn_state_t BTN_GetState(const btn_t *btn);

/* -------------------------------------------------------------------------- */
/* Interrupt Support                                                           */
/* -------------------------------------------------------------------------- */

uint8_t BTN_InitInterrupt(
    const btn_t *btn,
    ext_int_trigger_t trigger,
    btn_callback_t callback);

void BTN_EnableInterrupt(const btn_t *btn);

void BTN_DisableInterrupt(const btn_t *btn);

#endif /* BTN_H */