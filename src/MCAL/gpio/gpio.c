#include "gpio.h"
#include "gpio_registers.h" 
#include "bit_utils.h"

void GPIO_InitPin_WithPullup(gpio_pin_t pin, pullup_state_t PULLUP_STATE) {
    switch (pin.dir) {
      case GPIO_OUT:
        SET_BIT(*gpio_ddr_register(pin), pin.bit); // set output
        CLEAR_BIT(*gpio_port_register(pin), pin.bit); // set off
        return;
      case GPIO_IN:
        CLEAR_BIT(*gpio_ddr_register(pin), pin.bit); // set input
        if (PULLUP_STATE == PULLUP_STATE_OFF) {
          CLEAR_BIT(*gpio_port_register(pin), pin.bit); // clear pull-up resistor
        } else {
          SET_BIT(*gpio_port_register(pin), pin.bit); // set pull-up resistor
        }
      default:
        return;
    }
}

void GPIO_InitPin(gpio_pin_t pin) {
    GPIO_InitPin_WithPullup(pin, PULLUP_STATE_OFF);
}

void GPIO_WritePin(gpio_pin_t pin, gpio_state_t value) {
    if (pin.dir == GPIO_IN) return;

    if (value == GPIO_HIGH) {
      SET_BIT(*gpio_port_register(pin), pin.bit);
    } else {
      CLEAR_BIT(*gpio_port_register(pin), pin.bit);
    }
}

void GPIO_TogglePin(gpio_pin_t pin) {
  if (pin.dir == GPIO_IN) return;

  TOGGLE_BIT(*gpio_port_register(pin), pin.bit);

}

gpio_state_t GPIO_ReadPin(gpio_pin_t pin) {
  gpio_state_t pin_value =
    (READ_BIT(*gpio_pin_register(pin), pin.bit) != 0U);

  return (pin_value ? GPIO_HIGH : GPIO_LOW);
}