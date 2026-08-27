#include "adc.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>

#include "adc_types.h"
#include "adc_private.h"
#include "adc_registers.h"
#include "bit_utils.h"

/* -------------------------------------------------------------------------- */
/* Register Helpers                                                           */
/* -------------------------------------------------------------------------- */

static uint8_t ADC_RegisterIsValid(const adc_register_t *reg)
{
    if (reg == NULL)
    {
        return 0U;
    }

    if (reg->address == NULL)
    {
        return 0U;
    }

    if (reg->width == ADC_REGISTER_UNUSED)
    {
        return 0U;
    }

    return 1U;
}

static uint8_t ADC_Read8(const adc_register_t *reg)
{
    if (ADC_RegisterIsValid(reg) == 0U)
    {
        return 0U;
    }

    return *(volatile uint8_t *)reg->address;
}

static uint16_t ADC_Read16(const adc_register_t *reg)
{
    if (ADC_RegisterIsValid(reg) == 0U)
    {
        return 0U;
    }

    return *(volatile uint16_t *)reg->address;
}

static void ADC_Write8(
  const adc_register_t *reg,
  uint8_t value)
{
  if (ADC_RegisterIsValid(reg) == 0U)
  {
      return;
  }

  *(volatile uint8_t *)reg->address = value;
}

static void ADC_Update8(
  const adc_register_t *reg,
  uint8_t mask,
  uint8_t value)
{
  uint8_t current;

  current = ADC_Read8(reg);

  current &= (uint8_t)(~mask);
  current |= (uint8_t)(value & mask);

  ADC_Write8(reg, current);
}

/* -------------------------------------------------------------------------- */
/* ADC Input MUX Table                                                        */
/* -------------------------------------------------------------------------- */

#define ADC_MUX_TABLE_SIZE \
    (sizeof(adc_mux_table) / sizeof(adc_mux_table[0]))

static const adc_mux_map_t adc_mux_table[] =
{
    /* ---------------------------------------------------------------------- */
    /* Single Ended                                                            */
    /* ---------------------------------------------------------------------- */

    {
        .input =
        {
            .mode = ADC_INPUT_SINGLE_ENDED,
            .positive = ADC_CHANNEL_0
        },
        .mux_bits = 0x00
    },

    {
        .input =
        {
            .mode = ADC_INPUT_SINGLE_ENDED,
            .positive = ADC_CHANNEL_1
        },
        .mux_bits = 0x01
    },

    {
        .input =
        {
            .mode = ADC_INPUT_SINGLE_ENDED,
            .positive = ADC_CHANNEL_2
        },
        .mux_bits = 0x02
    },

    {
        .input =
        {
            .mode = ADC_INPUT_SINGLE_ENDED,
            .positive = ADC_CHANNEL_3
        },
        .mux_bits = 0x03
    },

    {
        .input =
        {
            .mode = ADC_INPUT_SINGLE_ENDED,
            .positive = ADC_CHANNEL_4
        },
        .mux_bits = 0x04
    },

    {
        .input =
        {
            .mode = ADC_INPUT_SINGLE_ENDED,
            .positive = ADC_CHANNEL_5
        },
        .mux_bits = 0x05
    },

    {
        .input =
        {
            .mode = ADC_INPUT_SINGLE_ENDED,
            .positive = ADC_CHANNEL_6
        },
        .mux_bits = 0x06
    },

    {
        .input =
        {
            .mode = ADC_INPUT_SINGLE_ENDED,
            .positive = ADC_CHANNEL_7
        },
        .mux_bits = 0x07
    },

    /* ---------------------------------------------------------------------- */
    /* Differential                                                           */
    /* ---------------------------------------------------------------------- */

    {
        .input =
        {
            .mode = ADC_INPUT_DIFFERENTIAL,
            .positive = ADC_CHANNEL_0,
            .negative = ADC_CHANNEL_0,
            .gain = ADC_GAIN_10X
        },
        .mux_bits = 0x08
    },

    {
        .input =
        {
            .mode = ADC_INPUT_DIFFERENTIAL,
            .positive = ADC_CHANNEL_1,
            .negative = ADC_CHANNEL_0,
            .gain = ADC_GAIN_10X
        },
        .mux_bits = 0x09
    },

    {
        .input =
        {
            .mode = ADC_INPUT_DIFFERENTIAL,
            .positive = ADC_CHANNEL_0,
            .negative = ADC_CHANNEL_0,
            .gain = ADC_GAIN_200X
        },
        .mux_bits = 0x0A
    },

    {
        .input =
        {
            .mode = ADC_INPUT_DIFFERENTIAL,
            .positive = ADC_CHANNEL_1,
            .negative = ADC_CHANNEL_0,
            .gain = ADC_GAIN_200X
        },
        .mux_bits = 0x0B
    },

    {
        .input =
        {
            .mode = ADC_INPUT_DIFFERENTIAL,
            .positive = ADC_CHANNEL_2,
            .negative = ADC_CHANNEL_2,
            .gain = ADC_GAIN_10X
        },
        .mux_bits = 0x0C
    },

    {
        .input =
        {
            .mode = ADC_INPUT_DIFFERENTIAL,
            .positive = ADC_CHANNEL_3,
            .negative = ADC_CHANNEL_2,
            .gain = ADC_GAIN_10X
        },
        .mux_bits = 0x0D
    },

    {
        .input =
        {
            .mode = ADC_INPUT_DIFFERENTIAL,
            .positive = ADC_CHANNEL_2,
            .negative = ADC_CHANNEL_2,
            .gain = ADC_GAIN_200X
        },
        .mux_bits = 0x0E
    },

    {
        .input =
        {
            .mode = ADC_INPUT_DIFFERENTIAL,
            .positive = ADC_CHANNEL_3,
            .negative = ADC_CHANNEL_2,
            .gain = ADC_GAIN_200X
        },
        .mux_bits = 0x0F
    },

    {
        .input =
        {
            .mode = ADC_INPUT_DIFFERENTIAL,
            .positive = ADC_CHANNEL_0,
            .negative = ADC_CHANNEL_1,
            .gain = ADC_GAIN_1X
        },
        .mux_bits = 0x10
    },

    {
        .input =
        {
            .mode = ADC_INPUT_DIFFERENTIAL,
            .positive = ADC_CHANNEL_1,
            .negative = ADC_CHANNEL_1,
            .gain = ADC_GAIN_1X
        },
        .mux_bits = 0x11
    },

    {
        .input =
        {
            .mode = ADC_INPUT_DIFFERENTIAL,
            .positive = ADC_CHANNEL_2,
            .negative = ADC_CHANNEL_1,
            .gain = ADC_GAIN_1X
        },
        .mux_bits = 0x12
    },

    {
        .input =
        {
            .mode = ADC_INPUT_DIFFERENTIAL,
            .positive = ADC_CHANNEL_3,
            .negative = ADC_CHANNEL_1,
            .gain = ADC_GAIN_1X
        },
        .mux_bits = 0x13
    },

    {
        .input =
        {
            .mode = ADC_INPUT_DIFFERENTIAL,
            .positive = ADC_CHANNEL_4,
            .negative = ADC_CHANNEL_1,
            .gain = ADC_GAIN_1X
        },
        .mux_bits = 0x14
    },

    {
        .input =
        {
            .mode = ADC_INPUT_DIFFERENTIAL,
            .positive = ADC_CHANNEL_5,
            .negative = ADC_CHANNEL_1,
            .gain = ADC_GAIN_1X
        },
        .mux_bits = 0x15
    },

    {
        .input =
        {
            .mode = ADC_INPUT_DIFFERENTIAL,
            .positive = ADC_CHANNEL_6,
            .negative = ADC_CHANNEL_1,
            .gain = ADC_GAIN_1X
        },
        .mux_bits = 0x16
    },

    {
        .input =
        {
            .mode = ADC_INPUT_DIFFERENTIAL,
            .positive = ADC_CHANNEL_7,
            .negative = ADC_CHANNEL_1,
            .gain = ADC_GAIN_1X
        },
        .mux_bits = 0x17
    }
};

/* -------------------------------------------------------------------------- */
/* MUX Helpers                                                                */
/* -------------------------------------------------------------------------- */

static uint8_t ADC_InputEquals(
  const adc_input_t *lhs,
  const adc_input_t *rhs)
{
  if ((lhs == NULL) || (rhs == NULL))
  {
      return 0U;
  }

  if (lhs->mode != rhs->mode)
  {
      return 0U;
  }

  if (lhs->positive != rhs->positive)
  {
      return 0U;
  }

  if (lhs->mode == ADC_INPUT_SINGLE_ENDED)
  {
      return 1U;
  }

  if (lhs->negative != rhs->negative)
  {
      return 0U;
  }

  if (lhs->gain != rhs->gain)
  {
      return 0U;
  }

  return 1U;
}

static const adc_mux_map_t *ADC_FindMuxEntry(
    const adc_input_t *input)
{
    uint8_t i;

    if (input == NULL)
    {
        return NULL;
    }

    for (i = 0U; i < ADC_MUX_TABLE_SIZE; i++)
    {
        if (ADC_InputEquals(input, &adc_mux_table[i].input) != 0U)
        {
            return &adc_mux_table[i];
        }
    }

    return NULL;
}

/* -------------------------------------------------------------------------- */
/* Configuration Helpers                                                      */
/* -------------------------------------------------------------------------- */


static const uint8_t adc_reference_bits[ADC_REFERENCE_COUNT] =
{
  [ADC_REFERENCE_AREF] = 0,
  [ADC_REFERENCE_AVCC] = 1,
  [ADC_REFERENCE_INTERNAL_2V56] = 3
};

static const uint8_t adc_prescaler_bits[ADC_PRESCALER_COUNT] =
{
  [ADC_PRESCALER_2]   = 1,
  [ADC_PRESCALER_4]   = 2,
  [ADC_PRESCALER_8]   = 3,
  [ADC_PRESCALER_16]  = 4,
  [ADC_PRESCALER_32]  = 5,
  [ADC_PRESCALER_64]  = 6,
  [ADC_PRESCALER_128] = 7
};

static const uint8_t adc_trigger_bits[ADC_TRIGGER_COUNT] =
{
  [ADC_TRIGGER_FREE_RUNNING]           = 0,
  [ADC_TRIGGER_ANALOG_COMPARATOR]      = 1,
  [ADC_TRIGGER_EXTERNAL_INTERRUPT_0]   = 2,
  [ADC_TRIGGER_TIMER0_COMPARE_MATCH]   = 3,
  [ADC_TRIGGER_TIMER0_OVERFLOW]        = 4,
  [ADC_TRIGGER_TIMER1_COMPARE_MATCH_B] = 5,
  [ADC_TRIGGER_TIMER1_OVERFLOW]        = 6,
  [ADC_TRIGGER_TIMER1_CAPTURE_EVENT]   = 7
};

void ADC_SetReference(adc_reference_t reference)
{
    if(reference >= ADC_REFERENCE_COUNT)
    {
        return;
    }
    const adc_descriptor_t *adc = ADC_RegistersGet();

    ADC_Update8(
        &adc->admux,
        adc->reference_mask,
        (uint8_t)(adc_reference_bits[reference] << REFS0));
}

void ADC_SetPrescaler(adc_prescaler_t prescaler)
{
    const adc_descriptor_t *adc = ADC_RegistersGet();

    ADC_Update8(
        &adc->adcsra,
        adc->prescaler_mask,
        adc_prescaler_bits[prescaler]);
}

void ADC_SetAlignment(adc_alignment_t alignment)
{
    const adc_descriptor_t *adc = ADC_RegistersGet();

    ADC_Update8(
        &adc->admux,
        adc->alignment_mask,
        (uint8_t)(alignment << ADLAR));
}

void ADC_SetTriggerSource(adc_trigger_t trigger)
{
    const adc_descriptor_t *adc = ADC_RegistersGet();

    ADC_Update8(
        &adc->sfior,
        adc->trigger_mask,
        (uint8_t)(adc_trigger_bits[trigger] << ADTS0));
}

/* -------------------------------------------------------------------------- */
/* Driver State                                                               */
/* -------------------------------------------------------------------------- */

static adc_callback_t adc_callback = NULL;


uint8_t ADC_SetInput(
  const adc_input_t *input)
{
  const adc_descriptor_t *adc = ADC_RegistersGet();
  const adc_mux_map_t *entry;

  if (input == NULL)
  {
      return 0U;
  }

  entry = ADC_FindMuxEntry(input);

  if (entry == NULL)
  {
      return 0U;
  }

  ADC_Update8(
      &adc->admux,
      adc->channel_mask,
      entry->mux_bits);

  return 1U;
}

void ADC_Enable(void)
{
    const adc_descriptor_t *adc = ADC_RegistersGet();

    ADC_Update8(
        &adc->adcsra,
        (uint8_t)(1U << adc->enable_bit),
        (uint8_t)(1U << adc->enable_bit));
}

void ADC_Disable(void)
{
    const adc_descriptor_t *adc = ADC_RegistersGet();

    ADC_Update8(
        &adc->adcsra,
        (uint8_t)(1U << adc->enable_bit),
        0U);
}

void ADC_EnableInterrupt(void)
{
    const adc_descriptor_t *adc = ADC_RegistersGet();

    ADC_Update8(
      &adc->adcsra,
      (uint8_t)(1U << adc->interrupt_enable_bit),
      (uint8_t)(1U << adc->interrupt_enable_bit));
}

void ADC_DisableInterrupt(void)
{
    const adc_descriptor_t *adc = ADC_RegistersGet();

    ADC_Update8(
      &adc->adcsra,
      (uint8_t)(1U << adc->interrupt_enable_bit),
      0U);
}

void ADC_EnableAutoTrigger(adc_trigger_t trigger)
{
    const adc_descriptor_t *adc = ADC_RegistersGet();

    ADC_SetTriggerSource(trigger);

    SET_BIT(
        *(volatile uint8_t *)adc->adcsra.address,
        adc->auto_trigger_bit);
}

void ADC_DisableAutoTrigger(void)
{
    const adc_descriptor_t *adc = ADC_RegistersGet();

    CLEAR_BIT(
        *(volatile uint8_t *)adc->adcsra.address,
        adc->auto_trigger_bit);
}

uint8_t ADC_Init(const adc_config_t *config)
{
    if (config == NULL)
    {
        return 0U;
    }

    ADC_Disable();

    ADC_SetReference(config->reference);

    ADC_SetAlignment(config->alignment);

    if (ADC_SetInput(&config->input) == 0U)
    {
        return 0U;
    }

    ADC_SetPrescaler(config->prescaler);

    if (config->auto_trigger == ADC_AUTO_TRIGGER_ENABLE)
    {
        ADC_EnableAutoTrigger(config->trigger);
    }
    else
    {
        ADC_DisableAutoTrigger();
    }

    if (config->interrupt == ADC_INTERRUPT_ENABLE)
    {
        ADC_EnableInterrupt();
    }
    else
    {
        ADC_DisableInterrupt();
    }

    ADC_Enable();

    return 1U;
}

void ADC_StartConversion(void)
{
    const adc_descriptor_t *adc = ADC_RegistersGet();

    ADC_Update8(
        &adc->adcsra,
        (uint8_t)(1U << adc->start_bit),
        (uint8_t)(1U << adc->start_bit));
}

uint8_t ADC_IsConversionComplete(void)
{
    const adc_descriptor_t *adc = ADC_RegistersGet();

    return
        (ADC_Read8(&adc->adcsra) &
         (uint8_t)(1U << adc->interrupt_flag_bit))
        ? 1U
        : 0U;
}

void ADC_ClearFlag(void)
{
    const adc_descriptor_t *adc = ADC_RegistersGet();

    ADC_Update8(
        &adc->adcsra,
        (uint8_t)(1U << adc->interrupt_flag_bit),
        (uint8_t)(1U << adc->interrupt_flag_bit));
}

uint16_t ADC_Read(void)
{
    const adc_descriptor_t *adc = ADC_RegistersGet();

    ADC_StartConversion();

    while (ADC_IsConversionComplete() == 0U)
    {
    }

    ADC_ClearFlag();

    return ADC_Read16(&adc->adc);
}

void ADC_RegisterCallback(
  adc_callback_t callback)
{
  adc_callback = callback;
}

ISR(ADC_vect)
{
    ADC_ClearFlag();

    if (adc_callback != NULL)
    {
        adc_callback();
    }
}