#include "btn.h"

#include <stddef.h>

/* -------------------------------------------------------------------------- */
/* Static Data                                                                 */
/* -------------------------------------------------------------------------- */

typedef struct
{
    gpio_port_t port;
    uint8_t bit;
    ext_int_id_t line;
} btn_interrupt_map_t;

static const btn_interrupt_map_t btn_interrupt_map[] =
{
    { GPIO_PORT_D, 2U, EXT_INT_0 },
    { GPIO_PORT_D, 3U, EXT_INT_1 },
    { GPIO_PORT_B, 2U, EXT_INT_2 }
};

#define BTN_INTERRUPT_MAP_COUNT \
    (sizeof(btn_interrupt_map) / \
     sizeof(btn_interrupt_map[0]))

static btn_callback_t btn_callbacks[EXT_INT_COUNT] = { NULL };

/* -------------------------------------------------------------------------- */
/* Dispatchers                                                                 */
/* -------------------------------------------------------------------------- */

static void BTN_Dispatch0(void)
{
    if (btn_callbacks[EXT_INT_0] != NULL)
    {
        btn_callbacks[EXT_INT_0]();
    }
}

static void BTN_Dispatch1(void)
{
    if (btn_callbacks[EXT_INT_1] != NULL)
    {
        btn_callbacks[EXT_INT_1]();
    }
}

static void BTN_Dispatch2(void)
{
    if (btn_callbacks[EXT_INT_2] != NULL)
    {
        btn_callbacks[EXT_INT_2]();
    }
}

static const ext_int_callback_t btn_dispatchers[EXT_INT_COUNT] =
{
    BTN_Dispatch0,
    BTN_Dispatch1,
    BTN_Dispatch2
};

/* -------------------------------------------------------------------------- */
/* Private Helpers                                                             */
/* -------------------------------------------------------------------------- */

static uint8_t BTN_GetInterruptLine(
    const btn_t *btn,
    ext_int_id_t *line)
{
    uint8_t index;

    if ((btn == NULL) || (line == NULL))
    {
        return 0U;
    }

    for (index = 0U;
         index < BTN_INTERRUPT_MAP_COUNT;
         index++)
    {
        if ((btn->pin.port ==
             btn_interrupt_map[index].port) &&
            (btn->pin.bit ==
             btn_interrupt_map[index].bit))
        {
            *line = btn_interrupt_map[index].line;
            return 1U;
        }
    }

    return 0U;
}

/* -------------------------------------------------------------------------- */
/* Public API                                                                  */
/* -------------------------------------------------------------------------- */

void BTN_Init(const btn_t *btn)
{
    if (btn == NULL)
    {
        return;
    }

    GPIO_InitPin_WithPullup(
        btn->pin,
        btn->pullup);
}

btn_state_t BTN_GetState(const btn_t *btn)
{
    if (btn == NULL)
    {
        return BTN_RELEASED;
    }

    return (GPIO_ReadPin(btn->pin) == GPIO_LOW)
        ? BTN_PRESSED
        : BTN_RELEASED;
}

uint8_t BTN_InitInterrupt(
    const btn_t *btn,
    ext_int_trigger_t trigger,
    btn_callback_t callback)
{
    ext_int_id_t line;
    ext_int_config_t config;

    if ((btn == NULL) ||
        (callback == NULL))
    {
        return 0U;
    }

    if (BTN_GetInterruptLine(btn, &line) == 0U)
    {
        return 0U;
    }

    config.line = line;
    config.trigger = trigger;

    btn_callbacks[line] = callback;

    EXT_INT_Init(&config);
    EXT_INT_RegisterCallback(
        line,
        btn_dispatchers[line]);

    EXT_INT_ClearFlag(line);
    EXT_INT_Enable(line);

    return 1U;
}

void BTN_EnableInterrupt(const btn_t *btn)
{
    ext_int_id_t line;

    if ((btn == NULL) ||
        (BTN_GetInterruptLine(btn, &line) == 0U))
    {
        return;
    }

    EXT_INT_ClearFlag(line);
    EXT_INT_Enable(line);
}

void BTN_DisableInterrupt(const btn_t *btn)
{
    ext_int_id_t line;

    if ((btn == NULL) ||
        (BTN_GetInterruptLine(btn, &line) == 0U))
    {
        return;
    }

    EXT_INT_Disable(line);
}