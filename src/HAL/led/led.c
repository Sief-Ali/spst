#include "led.h"
#include "timer_types.h"

#include <timer_service.h>


void LED_Init(led_t *led)
{
    GPIO_InitPin(led->pin);
}

void LED_On(led_t *led)
{
    GPIO_WritePin(led->pin, GPIO_HIGH);
}

void LED_Off(led_t *led)
{
    GPIO_WritePin(led->pin, GPIO_LOW);
}

void LED_Toggle(led_t *led)
{
    GPIO_TogglePin(led->pin);
}