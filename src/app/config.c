#include "app_config.h"

#include <stddef.h>

#include "eeprom.h"

static const app_runtime_config_t app_default_config =
{
    .dead_band = 30U,
    .travel_limit_lower = 15U,
    .travel_limit_upper = 165U,
    .east_park_angle = 20U,
    .cloud_entry_level = 400U,
    .cloud_exit_level = 550U,
    .cloud_confirmation_time_s = 3U,
    .night_start_hour = 18U,
    .night_start_minute = 30U,
    .night_end_hour = 6U,
    .night_end_minute = 30U,
    .day_start_hour = 6U,
    .day_start_minute = 30U,
    .day_end_hour = 18U,
    .day_end_minute = 30U
};

static const app_cfg_map_entry_t app_cfg_map[APP_CFG_COUNT] =
{
    { APP_CFG_DEAD_BAND,                 0U, sizeof(uint16_t) },
    { APP_CFG_TRAVEL_LIMIT_LOWER,        2U, sizeof(uint8_t) },
    { APP_CFG_TRAVEL_LIMIT_UPPER,        3U, sizeof(uint8_t) },
    { APP_CFG_EAST_PARK_ANGLE,           4U, sizeof(uint8_t) },
    { APP_CFG_CLOUD_ENTRY_LEVEL,         5U, sizeof(uint16_t) },
    { APP_CFG_CLOUD_EXIT_LEVEL,          7U, sizeof(uint16_t) },
    { APP_CFG_CLOUD_CONFIRMATION_TIME_S, 9U, sizeof(uint16_t) },
    { APP_CFG_NIGHT_START_HOUR,         11U, sizeof(uint8_t) },
    { APP_CFG_NIGHT_START_MINUTE,       12U, sizeof(uint8_t) },
    { APP_CFG_NIGHT_END_HOUR,           13U, sizeof(uint8_t) },
    { APP_CFG_NIGHT_END_MINUTE,         14U, sizeof(uint8_t) },
    { APP_CFG_DAY_START_HOUR,           15U, sizeof(uint8_t) },
    { APP_CFG_DAY_START_MINUTE,         16U, sizeof(uint8_t) },
    { APP_CFG_DAY_END_HOUR,             17U, sizeof(uint8_t) },
    { APP_CFG_DAY_END_MINUTE,           18U, sizeof(uint8_t) }
};

const app_runtime_config_t *APP_Config_GetDefault(void)
{
    return &app_default_config;
}

uint16_t APP_Config_GetValue(const app_runtime_config_t *config, app_cfg_id_t id)
{
    if (config == NULL)
    {
        return 0U;
    }

    switch (id)
    {
        case APP_CFG_DEAD_BAND:
            return config->dead_band;
        case APP_CFG_TRAVEL_LIMIT_LOWER:
            return config->travel_limit_lower;
        case APP_CFG_TRAVEL_LIMIT_UPPER:
            return config->travel_limit_upper;
        case APP_CFG_EAST_PARK_ANGLE:
            return config->east_park_angle;
        case APP_CFG_CLOUD_ENTRY_LEVEL:
            return config->cloud_entry_level;
        case APP_CFG_CLOUD_EXIT_LEVEL:
            return config->cloud_exit_level;
        case APP_CFG_CLOUD_CONFIRMATION_TIME_S:
            return config->cloud_confirmation_time_s;
        case APP_CFG_NIGHT_START_HOUR:
            return config->night_start_hour;
        case APP_CFG_NIGHT_START_MINUTE:
            return config->night_start_minute;
        case APP_CFG_NIGHT_END_HOUR:
            return config->night_end_hour;
        case APP_CFG_NIGHT_END_MINUTE:
            return config->night_end_minute;
        case APP_CFG_DAY_START_HOUR:
            return config->day_start_hour;
        case APP_CFG_DAY_START_MINUTE:
            return config->day_start_minute;
        case APP_CFG_DAY_END_HOUR:
            return config->day_end_hour;
        case APP_CFG_DAY_END_MINUTE:
            return config->day_end_minute;
        default:
            return 0U;
    }
}

void APP_Config_SetValue(app_runtime_config_t *config, app_cfg_id_t id, uint16_t value)
{
    if (config == NULL)
    {
        return;
    }

    switch (id)
    {
        case APP_CFG_DEAD_BAND:
            config->dead_band = (uint16_t)value;
            break;
        case APP_CFG_TRAVEL_LIMIT_LOWER:
            config->travel_limit_lower = (uint8_t)value;
            break;
        case APP_CFG_TRAVEL_LIMIT_UPPER:
            config->travel_limit_upper = (uint8_t)value;
            break;
        case APP_CFG_EAST_PARK_ANGLE:
            config->east_park_angle = (uint8_t)value;
            break;
        case APP_CFG_CLOUD_ENTRY_LEVEL:
            config->cloud_entry_level = (uint16_t)value;
            break;
        case APP_CFG_CLOUD_EXIT_LEVEL:
            config->cloud_exit_level = (uint16_t)value;
            break;
        case APP_CFG_CLOUD_CONFIRMATION_TIME_S:
            config->cloud_confirmation_time_s = (uint16_t)value;
            break;
        case APP_CFG_NIGHT_START_HOUR:
            config->night_start_hour = (uint8_t)value;
            break;
        case APP_CFG_NIGHT_START_MINUTE:
            config->night_start_minute = (uint8_t)value;
            break;
        case APP_CFG_NIGHT_END_HOUR:
            config->night_end_hour = (uint8_t)value;
            break;
        case APP_CFG_NIGHT_END_MINUTE:
            config->night_end_minute = (uint8_t)value;
            break;
        case APP_CFG_DAY_START_HOUR:
            config->day_start_hour = (uint8_t)value;
            break;
        case APP_CFG_DAY_START_MINUTE:
            config->day_start_minute = (uint8_t)value;
            break;
        case APP_CFG_DAY_END_HOUR:
            config->day_end_hour = (uint8_t)value;
            break;
        case APP_CFG_DAY_END_MINUTE:
            config->day_end_minute = (uint8_t)value;
            break;
        default:
            break;
    }
}

void APP_Config_WriteEeprom(const app_runtime_config_t *config)
{
    uint16_t index;
    uint8_t byte_index;
    uint16_t value;

    if (config == NULL)
    {
        return;
    }

    for (index = 0U; index < APP_CFG_COUNT; index++)
    {
        value = APP_Config_GetValue(config, app_cfg_map[index].id);

        for (byte_index = 0U; byte_index < app_cfg_map[index].width; byte_index++)
        {
            EEPROM_WriteByte(
                EEPROM_CONFIG_ADDRESS + app_cfg_map[index].eeprom_address + byte_index,
                (uint8_t)(value >> (8U * byte_index)));
        }
    }
}

void APP_Config_ReadEeprom(app_runtime_config_t *config)
{
    uint16_t index;
    uint8_t byte_index;
    uint16_t value;

    if (config == NULL)
    {
        return;
    }

    for (index = 0U; index < APP_CFG_COUNT; index++)
    {
        value = 0U;

        for (byte_index = 0U; byte_index < app_cfg_map[index].width; byte_index++)
        {
            value |= ((uint16_t)EEPROM_ReadByte(
                EEPROM_CONFIG_ADDRESS + app_cfg_map[index].eeprom_address + byte_index))
                << (8U * byte_index);
        }

        APP_Config_SetValue(config, app_cfg_map[index].id, value);
    }
}

uint8_t APP_Config_MatchesDefaults(const app_runtime_config_t *config)
{
    const app_runtime_config_t *defaults = APP_Config_GetDefault();

    if ((config == NULL) || (defaults == NULL))
    {
        return 0U;
    }

    return (
        (config->dead_band == defaults->dead_band) &&
        (config->travel_limit_lower == defaults->travel_limit_lower) &&
        (config->travel_limit_upper == defaults->travel_limit_upper) &&
        (config->east_park_angle == defaults->east_park_angle) &&
        (config->cloud_entry_level == defaults->cloud_entry_level) &&
        (config->cloud_exit_level == defaults->cloud_exit_level) &&
        (config->cloud_confirmation_time_s == defaults->cloud_confirmation_time_s) &&
        (config->night_start_hour == defaults->night_start_hour) &&
        (config->night_start_minute == defaults->night_start_minute) &&
        (config->night_end_hour == defaults->night_end_hour) &&
        (config->night_end_minute == defaults->night_end_minute) &&
        (config->day_start_hour == defaults->day_start_hour) &&
        (config->day_start_minute == defaults->day_start_minute) &&
        (config->day_end_hour == defaults->day_end_hour) &&
        (config->day_end_minute == defaults->day_end_minute)
    );
}

void APP_Config_ClearRegion(void)
{
    uint16_t index;

    for (index = 0U; index < EEPROM_CONFIG_SIZE; index++)
    {
        EEPROM_WriteByte(EEPROM_CONFIG_ADDRESS + index, 0xFFU);
    }
}
