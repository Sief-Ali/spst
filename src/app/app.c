#include "app.h"

#include <stdio.h>
#include <util/delay.h>

#include "logger.h"
#include "analog.h"
#include "board.h"
#include "lcd.h"
#include "lcd_config.h"
#include "eeprom.h"
#include "led.h"
#include "rtc.h"

#define EEPROM_CONFIG_ADDRESS 0U
#define EEPROM_CONFIG_SIZE    19U

typedef struct
{
    uint16_t dead_band;
    uint8_t travel_limit_lower;
    uint8_t travel_limit_upper;
    uint8_t east_park_angle;
    uint16_t cloud_entry_level;
    uint16_t cloud_exit_level;
    uint16_t cloud_confirmation_time_s;
    uint8_t night_start_hour;
    uint8_t night_start_minute;
    uint8_t night_end_hour;
    uint8_t night_end_minute;
    uint8_t day_start_hour;
    uint8_t day_start_minute;
    uint8_t day_end_hour;
    uint8_t day_end_minute;
} app_runtime_config_t;

typedef enum
{
    APP_CFG_DEAD_BAND = 0U,
    APP_CFG_TRAVEL_LIMIT_LOWER,
    APP_CFG_TRAVEL_LIMIT_UPPER,
    APP_CFG_EAST_PARK_ANGLE,
    APP_CFG_CLOUD_ENTRY_LEVEL,
    APP_CFG_CLOUD_EXIT_LEVEL,
    APP_CFG_CLOUD_CONFIRMATION_TIME_S,
    APP_CFG_NIGHT_START_HOUR,
    APP_CFG_NIGHT_START_MINUTE,
    APP_CFG_NIGHT_END_HOUR,
    APP_CFG_NIGHT_END_MINUTE,
    APP_CFG_DAY_START_HOUR,
    APP_CFG_DAY_START_MINUTE,
    APP_CFG_DAY_END_HOUR,
    APP_CFG_DAY_END_MINUTE,
    APP_CFG_COUNT
} app_cfg_id_t;

typedef struct
{
    app_cfg_id_t id;
    uint16_t eeprom_address;
    uint8_t width;
} app_cfg_map_entry_t;

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
    { APP_CFG_DEAD_BAND,                0U,  sizeof(uint16_t) },
    { APP_CFG_TRAVEL_LIMIT_LOWER,      2U,  sizeof(uint8_t) },
    { APP_CFG_TRAVEL_LIMIT_UPPER,      3U,  sizeof(uint8_t) },
    { APP_CFG_EAST_PARK_ANGLE,         4U,  sizeof(uint8_t) },
    { APP_CFG_CLOUD_ENTRY_LEVEL,       5U,  sizeof(uint16_t) },
    { APP_CFG_CLOUD_EXIT_LEVEL,        7U,  sizeof(uint16_t) },
    { APP_CFG_CLOUD_CONFIRMATION_TIME_S, 9U, sizeof(uint16_t) },
    { APP_CFG_NIGHT_START_HOUR,       11U, sizeof(uint8_t) },
    { APP_CFG_NIGHT_START_MINUTE,     12U, sizeof(uint8_t) },
    { APP_CFG_NIGHT_END_HOUR,         13U, sizeof(uint8_t) },
    { APP_CFG_NIGHT_END_MINUTE,       14U, sizeof(uint8_t) },
    { APP_CFG_DAY_START_HOUR,         15U, sizeof(uint8_t) },
    { APP_CFG_DAY_START_MINUTE,       16U, sizeof(uint8_t) },
    { APP_CFG_DAY_END_HOUR,           17U, sizeof(uint8_t) },
    { APP_CFG_DAY_END_MINUTE,         18U, sizeof(uint8_t) }
};

static uint16_t APP_GetConfigValue(
    const app_runtime_config_t *config,
    app_cfg_id_t id)
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

static void APP_SetConfigValue(
    app_runtime_config_t *config,
    app_cfg_id_t id,
    uint16_t value)
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

static void APP_WriteConfigToEeprom(const app_runtime_config_t *config)
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
        value = APP_GetConfigValue(config, app_cfg_map[index].id);

        for (byte_index = 0U; byte_index < app_cfg_map[index].width; byte_index++)
        {
            EEPROM_WriteByte(
                EEPROM_CONFIG_ADDRESS + app_cfg_map[index].eeprom_address + byte_index,
                (uint8_t)(value >> (8U * byte_index)));
        }
    }
}

static void APP_ReadConfigFromEeprom(app_runtime_config_t *config)
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

        APP_SetConfigValue(config, app_cfg_map[index].id, value);
    }
}

static uint8_t APP_ConfigMatchesDefaults(const app_runtime_config_t *config)
{
    if (config == NULL)
    {
        return 0U;
    }

    return (
        (config->dead_band == app_default_config.dead_band) &&
        (config->travel_limit_lower == app_default_config.travel_limit_lower) &&
        (config->travel_limit_upper == app_default_config.travel_limit_upper) &&
        (config->east_park_angle == app_default_config.east_park_angle) &&
        (config->cloud_entry_level == app_default_config.cloud_entry_level) &&
        (config->cloud_exit_level == app_default_config.cloud_exit_level) &&
        (config->cloud_confirmation_time_s == app_default_config.cloud_confirmation_time_s) &&
        (config->night_start_hour == app_default_config.night_start_hour) &&
        (config->night_start_minute == app_default_config.night_start_minute) &&
        (config->night_end_hour == app_default_config.night_end_hour) &&
        (config->night_end_minute == app_default_config.night_end_minute) &&
        (config->day_start_hour == app_default_config.day_start_hour) &&
        (config->day_start_minute == app_default_config.day_start_minute) &&
        (config->day_end_hour == app_default_config.day_end_hour) &&
        (config->day_end_minute == app_default_config.day_end_minute)
    );
}

static void APP_ClearConfigRegion(void)
{
    uint16_t index;

    for (index = 0U; index < EEPROM_CONFIG_SIZE; index++)
    {
        EEPROM_WriteByte(EEPROM_CONFIG_ADDRESS + index, 0xFFU);
    }
}

static void APP_InitializeEepromConfig(void)
{
    app_runtime_config_t config;
    char message[96];

    APP_ReadConfigFromEeprom(&config);

    if (APP_ConfigMatchesDefaults(&config) == 0U)
    {
        APP_ClearConfigRegion();
        APP_WriteConfigToEeprom(&app_default_config);
        APP_ReadConfigFromEeprom(&config);
        snprintf(
            message,
            sizeof(message),
            "EEPROM config initialized: dead=%u, limits=%u-%u, park=%u",
            config.dead_band,
            config.travel_limit_lower,
            config.travel_limit_upper,
            config.east_park_angle);
        Logger_Log(LOG_INFO, message);
    }
    else
    {
        snprintf(
            message,
            sizeof(message),
            "EEPROM config loaded: dead=%u, limits=%u-%u, park=%u",
            config.dead_band,
            config.travel_limit_lower,
            config.travel_limit_upper,
            config.east_park_angle);
        Logger_Log(LOG_INFO, message);
    }

    snprintf(
        message,
        sizeof(message),
        "CFG: cloud in=%u out=%u, confirm=%us, night=%02u:%02u-%02u:%02u",
        config.cloud_entry_level,
        config.cloud_exit_level,
        config.cloud_confirmation_time_s,
        config.night_start_hour,
        config.night_start_minute,
        config.night_end_hour,
        config.night_end_minute);
    Logger_Log(LOG_INFO, message);

    snprintf(
        message,
        sizeof(message),
        "CFG: day=%02u:%02u-%02u:%02u",
        config.day_start_hour,
        config.day_start_minute,
        config.day_end_hour,
        config.day_end_minute);
    Logger_Log(LOG_INFO, message);
}

void APP_Init(void)
{
    Logger_Log(
      LOG_BOOT,
      "System Ready");

    APP_InitializeEepromConfig();

    if (RTC_Init() != RTC_STATUS_OK)
    {
        Logger_Log(LOG_ERROR, "RTC init failed");
    }

    LCD_Init(&lcd_display);
    LCD_Clear(&lcd_display);
    LCD_SetCursor(&lcd_display, 1, 0);
    LCD_PrintString(&lcd_display, "SPST ready");
}

void APP_Run(void)
{
    Logger_Log(
      LOG_EVENT,
      "Application Running");

    while (1)
    {
        _delay_ms(100U);
    }
}