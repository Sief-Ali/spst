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
#include "panel.h"
#include "controller.h"
#include "controller_types.h"

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

void APP_Init(void)
{
    Logger_Log(LOG_BOOT, "System Ready");

    APP_InitializeEepromConfig();
    APP_LoadRuntimeConfig();

    if (RTC_Init() != RTC_STATUS_OK)
    {
        Logger_Log(LOG_ERROR, "RTC init failed");
    }

    Panel_Init();

    Controller_Init(
        app_live_config.dead_band,
        app_live_config.travel_limit_lower,
        app_live_config.travel_limit_upper,
        app_live_config.east_park_angle,
        app_live_config.cloud_entry_level,
        app_live_config.cloud_exit_level,
        app_live_config.cloud_confirmation_time_s,
        app_live_config.night_start_hour,
        app_live_config.night_start_minute,
        app_live_config.night_end_hour,
        app_live_config.night_end_minute);

    LCD_Init(&lcd_display);
    LCD_Clear(&lcd_display);
    LCD_SetCursor(&lcd_display, 1, 0);
    LCD_PrintString(&lcd_display, "SPST ready");
}

static void APP_HandleStateChange(controller_state_t new_state)
{
    const char *state_name = controller_state_names[new_state];
    char message[96];

    snprintf(message, sizeof(message), "State change: %s", state_name);
    Logger_Log(LOG_EVENT, message);
}

static void APP_UpdateDisplay(
    controller_state_t state,
    uint8_t current_angle,
    uint16_t east_reading,
    uint16_t west_reading)
{
    char line1[17];
    char line2[17];
    const char *state_name = controller_state_names[state];

    snprintf(line1, sizeof(line1), "%s %u", state_name, current_angle);
    LCD_Clear(&lcd_display);
    LCD_SetCursor(&lcd_display, 0, 0);
    LCD_PrintString(&lcd_display, line1);

    snprintf(line2, sizeof(line2), "E:%u W:%u", east_reading, west_reading);
    LCD_SetCursor(&lcd_display, 1, 0);
    LCD_PrintString(&lcd_display, line2);
}

static void APP_UpdateLeds(controller_state_t state)
{
    LED_Off(&led.ready);
    LED_Off(&led.processing);
    LED_Off(&led.error);

    switch (state)
    {
        case STATE_TRACKING:
            LED_On(&led.ready);
            break;

        case STATE_CLOUD_HOLD:
        case STATE_NIGHT_PARKING:
            LED_On(&led.processing);
            break;

        case STATE_FAULT:
            LED_On(&led.error);
            break;

        default:
            break;
    }
}

void APP_Run(void)
{
    analog_ldr_readings_t ldr_readings;
    tracker_readings_t readings;
    tracker_direction_t direction;
    controller_state_t current_state;
    uint8_t current_angle;
    uint8_t next_angle;
    rtc_time_t current_time;
    uint32_t display_counter = 0U;
    uint32_t rtc_counter = 0U;

    Logger_Log(LOG_EVENT, "Application Running");

    while (1)
    {
        if (Analog_ReadLdrs(&east_ldr_adc_config.input, &west_ldr_adc_config.input, &ldr_readings) == 0U)
        {
            Logger_Log(LOG_ERROR, "Failed to read LDRs");
            _delay_ms(100U);
            continue;
        }

        if (rtc_counter == 0U)
        {
            if (RTC_ReadTime(&current_time) != RTC_STATUS_OK)
            {
                Logger_Log(LOG_ERROR, "RTC read failed");
                current_time.hours = 12U;
                current_time.minutes = 0U;
            }
        }

        readings.east = ldr_readings.east;
        readings.west = ldr_readings.west;

        current_state = Controller_Update(
            readings.east,
            readings.west,
            current_time.hours,
            current_time.minutes);

        if (Controller_StateChanged())
        {
            APP_HandleStateChange(current_state);
        }

        current_angle = Panel_GetAngle();

        if (current_state == STATE_TRACKING)
        {
            direction = Tracker_GetDirection(&readings, app_live_config.dead_band);

            if ((direction == TRACKER_DIRECTION_EAST) || (direction == TRACKER_DIRECTION_WEST))
            {
                next_angle = Tracker_GetNextAngle(
                    current_angle,
                    direction,
                    5U,
                    app_live_config.travel_limit_lower,
                    app_live_config.travel_limit_upper);
                Panel_SetAngle(next_angle);
            }
        }
        else if (current_state == STATE_CLOUD_HOLD)
        {
            Panel_Hold();
        }
        else if (current_state == STATE_NIGHT_PARKING)
        {
            Panel_Park(app_live_config.east_park_angle);
        }
        else if (current_state == STATE_FAULT)
        {
            Panel_Hold();
        }

        if (display_counter == 0U)
        {
            APP_UpdateDisplay(current_state, current_angle, readings.east, readings.west);
            APP_UpdateLeds(current_state);
        }

        display_counter++;
        if (display_counter >= 10U)
        {
            display_counter = 0U;
        }

        rtc_counter++;
        if (rtc_counter >= 10U)
        {
            rtc_counter = 0U;
        }

        _delay_ms(100U);
    }
}