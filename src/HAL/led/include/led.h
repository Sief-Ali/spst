#ifndef LED_H
#define LED_H

#include "gpio.h"

#define BLINK_DELAY 500U

typedef struct
{
    gpio_pin_t pin;
} led_t;

void LED_Init(led_t *led);

void LED_On(led_t *led);
void LED_Off(led_t *led);
void LED_Toggle(led_t *led);
void LED_Blink(led_t *led, uint16_t blink_delay);


#endif