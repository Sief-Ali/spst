#include "app.h"

#include <stdio.h>
#include <util/delay.h>

#include "logger.h"
#include "analog.h"
#include "board.h"
#include "lcd.h"
#include "lcd_config.h"
#include "eeprom.h"
#include "tracker.h"
#include "led.h"
#include "rtc.h"
#include "app_config.h"

static app_runtime_config_t app_live_config;

static void APP_InitializeEepromConfig(void)
{
    app_runtime_config_t config;
    char message[96];

    APP_Config_ReadEeprom(&config);

    if (APP_Config_MatchesDefaults(&config) == 0U)
    {
        APP_Config_ClearRegion();
        APP_Config_WriteEeprom(APP_Config_GetDefault());
        APP_Config_ReadEeprom(&config);
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
}

static void APP_LoadRuntimeConfig(void)
{
    APP_Config_ReadEeprom(&app_live_config);
    Servo_SetAngle(&servo, app_live_config.east_park_angle);
}

static void APP_UpdateTrackingState(void)
{
    analog_ldr_readings_t analog_readings;
    tracker_readings_t readings;
    tracker_direction_t direction;
    const adc_input_t east_input = {
        .mode = ADC_INPUT_SINGLE_ENDED,
        .positive = LDR_EAST,
        .negative = ADC_CHANNEL_0
    };
    const adc_input_t west_input = {
        .mode = ADC_INPUT_SINGLE_ENDED,
        .positive = LDR_WEST,
        .negative = ADC_CHANNEL_0
    };
    uint8_t current_angle;
    uint8_t next_angle;

    if (Analog_ReadLdrs(&east_input, &west_input, &analog_readings) == 0U)
    {
        return;
    }

    readings.east = analog_readings.east;
    readings.west = analog_readings.west;

    direction = Tracker_GetDirection(&readings, app_live_config.dead_band);
    current_angle = Servo_GetAngle(&servo);

    if ((direction == TRACKER_DIRECTION_EAST) || (direction == TRACKER_DIRECTION_WEST))
    {
        next_angle = Tracker_GetNextAngle(
            current_angle,
            direction,
            5U,
            app_live_config.travel_limit_lower,
            app_live_config.travel_limit_upper);
        Servo_SetAngle(&servo, next_angle);
    }
}

void APP_Init(void)
{
    Logger_Log(LOG_BOOT, "System Ready");

    APP_InitializeEepromConfig();
    APP_LoadRuntimeConfig();

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
    Logger_Log(LOG_EVENT, "Application Running");

    while (1)
    {
        APP_UpdateTrackingState();
        _delay_ms(100U);
    }
}