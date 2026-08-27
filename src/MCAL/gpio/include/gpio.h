#ifndef GPIO_H
#define GPIO_H

#include <avr/io.h>
#include <stdint.h>

typedef enum {
  GPIO_IN = 0U,
  GPIO_OUT = 1U
} gpio_dir_t;

typedef enum {
  GPIO_LOW = 0U,
  GPIO_HIGH = 1U
} gpio_state_t;

typedef enum {
  PULLUP_STATE_OFF = 0U,
  PULLUP_STATE_ON = 1U
} pullup_state_t;

typedef enum {
  GPIO_PORT_A = 0U,
  GPIO_PORT_B = 1U,
  GPIO_PORT_C = 2U,
  GPIO_PORT_D = 3U
} gpio_port_t;

typedef struct {
    gpio_port_t port;
    gpio_dir_t dir;
    uint8_t bit;
} gpio_pin_t;

#define GPIO_PIN(PORT, BIT, DIR) \
    ((gpio_pin_t){GPIO_PORT_##PORT, DIR, P##PORT##BIT})

void GPIO_InitPin_WithPullup(gpio_pin_t, pullup_state_t);
void GPIO_InitPin(gpio_pin_t);
void GPIO_WritePin(gpio_pin_t, gpio_state_t);
void GPIO_TogglePin(gpio_pin_t);
gpio_state_t GPIO_ReadPin(gpio_pin_t);

#endif