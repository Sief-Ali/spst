#ifndef GPIO_REGISTERS_H
#define GPIO_REGISTERS_H

#include <stdint.h>
#include "gpio.h"

volatile uint8_t *gpio_ddr_register(gpio_pin_t pin);
volatile uint8_t *gpio_port_register(gpio_pin_t pin);
volatile uint8_t *gpio_pin_register(gpio_pin_t pin);

#endif