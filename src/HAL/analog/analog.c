#include "analog.h"
#include "adc.h"
#include <stddef.h>
#include <stdint.h>

void Analog_Init(const adc_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    ADC_Init(config);
}

uint16_t Analog_Read(void)
{
    return ADC_Read();
}

uint16_t Analog_ReadChannel(adc_input_t input)
{
    if (ADC_SetInput(&input) == 0U)
    {
        return 0U;
    }

    return ADC_Read();
}

uint8_t Analog_ReadLdrs(
    const adc_input_t *east_input,
    const adc_input_t *west_input,
    analog_ldr_readings_t *readings)
{
    if ((east_input == NULL) || (west_input == NULL) || (readings == NULL))
    {
        return 0U;
    }

    if (ADC_SetInput(east_input) == 0U)
    {
        return 0U;
    }

    readings->east = ADC_Read();

    if (ADC_SetInput(west_input) == 0U)
    {
        return 0U;
    }

    readings->west = ADC_Read();

    return 1U;
}

uint8_t Analog_Get_Quantity(void) {
  uint16_t pot_value = ADC_Read();
  
  // Formula from diagram: quantity = (ADC * 6) / 1024.
  // The multiply is widened to 32 bits before scaling down.
  uint8_t quantity = (uint8_t)(((uint32_t)pot_value * 6U) >> 10);
  
  // Clamp values above 5
  if (quantity > 5U) {
      quantity = 5U;
  }
  
  return quantity;
}
