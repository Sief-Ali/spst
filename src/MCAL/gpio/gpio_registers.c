#include <avr/io.h>
#include "gpio_registers.h"

volatile uint8_t *gpio_ddr_register(gpio_pin_t pin)
{
    switch (pin.port) {
    case GPIO_PORT_A: return &DDRA;
    case GPIO_PORT_B: return &DDRB;
    case GPIO_PORT_C: return &DDRC;
    case GPIO_PORT_D: return &DDRD;
    default:          return &DDRA;
    }
}

volatile uint8_t *gpio_port_register(gpio_pin_t pin)
{
    switch (pin.port) {
    case GPIO_PORT_A: return &PORTA;
    case GPIO_PORT_B: return &PORTB;
    case GPIO_PORT_C: return &PORTC;
    case GPIO_PORT_D: return &PORTD;
    default:          return &PORTA;
    }
}

volatile uint8_t *gpio_pin_register(gpio_pin_t pin)
{
    switch (pin.port) {
      case GPIO_PORT_A: return &PINA;
      case GPIO_PORT_B: return &PINB;
      case GPIO_PORT_C: return &PINC;
      case GPIO_PORT_D: return &PIND;
      default:          return &PINA;
    }
}