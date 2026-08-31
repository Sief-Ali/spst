#include "app.h"

#include <stdio.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "analog.h"
#include "app_config.h"
#include "board.h"
#include "controller.h"
#include "controller_types.h"
#include "eeprom.h"
#include "lcd.h"
#include "lcd_config.h"
#include "led.h"
#include "logger.h"
#include "panel.h"
#include "rtc.h"
#include "tracker.h"

#define SENSOR_PERIOD_MS       100U
#define DISPLAY_PERIOD_MS      500U
#define RTC_PERIOD_MS          1000U
#define LDR_QUEUE_LENGTH       4U
#define APP_TASK_STACK_WORDS   100U

typedef struct
{
    uint16_t east;
    uint16_t west;
} app_ldr_message_t;

/*
 * Shared application status.
 *
 * Sensor -> Control communication uses the LDR queue.
 * RTC time is published by the Display/RTC owner.
 * Control state is published for display/monitoring.
 */
typedef struct
{
    uint16_t east;
    uint16_t west;
    uint8_t angle;
    controller_state_t state;
    rtc_time_t time;
} app_status_t;

static app_runtime_config_t app_live_config;

static QueueHandle_t ldr_queue;

static SemaphoreHandle_t i2c_bus_semaphore;

/*
 * Start with an invalid/uninitialized time.
 *
 * APP_Init() replaces this with the actual DS1307 time
 * before the scheduler starts.
 */
static app_status_t app_status =
{
    0U,
    0U,
    20U,
    STATE_INIT,
    { 0U, 0U, 0U }
};

static void APP_InitializeEepromConfig(void)
{
    app_runtime_config_t config;
    char message[96];

    APP_Config_ReadEeprom(&config);

    if (APP_Config_MatchesDefaults(&config) == 0U)
    {
        APP_Config_ClearRegion();

        APP_Config_WriteEeprom(
            APP_Config_GetDefault());

        APP_Config_ReadEeprom(&config);

        snprintf(
            message,
            sizeof(message),
            "EEPROM config initialized: dead=%u, limits=%u-%u, park=%u",
            config.dead_band,
            config.travel_limit_lower,
            config.travel_limit_upper,
            config.east_park_angle);
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
    }

    Logger_Log(LOG_INFO, message);
}

static void APP_CopyStatus(app_status_t *copy)
{
    taskENTER_CRITICAL();

    *copy = app_status;

    taskEXIT_CRITICAL();
}

static void APP_PublishControl(
    controller_state_t state,
    uint16_t east,
    uint16_t west)
{
    taskENTER_CRITICAL();

    app_status.east = east;
    app_status.west = west;
    app_status.angle = Panel_GetAngle();
    app_status.state = state;

    taskEXIT_CRITICAL();
}

static void APP_PublishTime(const rtc_time_t *time)
{
    taskENTER_CRITICAL();

    app_status.time = *time;

    taskEXIT_CRITICAL();
}

static void APP_UpdateLeds(controller_state_t state)
{
    /*
     * Exactly one state LED is allowed to be ON.
     */
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

static const char *APP_DisplayStateName(
    controller_state_t state)
{
    switch (state)
    {
        case STATE_TRACKING:
            return "TRACK";

        case STATE_NIGHT_PARKING:
            return "NIGHT";

        case STATE_CLOUD_HOLD:
            return "CLOUD";

        case STATE_FAULT:
            return "FAULT";

        default:
            return "INIT";
    }
}

/*
 * Avoid snprintf() in the Display task.
 *
 * AVR libc formatting requires much more stack than this
 * RAM-constrained FreeRTOS application should spend.
 */
static void APP_LCD_PrintUInt16(uint16_t value)
{
    char digits[5];
    uint8_t count = 0U;

    do
    {
        digits[count] =
            (char)('0' + (value % 10U));

        value /= 10U;
        count++;

    } while (value != 0U);

    while (count > 0U)
    {
        count--;

        LCD_PrintChar(
            &lcd_display,
            digits[count]);
    }
}

/*
 * SENSOR TASK
 *
 * Responsibility:
 *   Read both LDR channels and send the pair to Control.
 */
static void APP_SensorTask(void *parameters)
{
    TickType_t last_wake =
        xTaskGetTickCount();

    app_ldr_message_t message;

    analog_ldr_readings_t readings;

    (void)parameters;

    for (;;)
    {
        if (Analog_ReadLdrs(
                &east_ldr_adc_config.input,
                &west_ldr_adc_config.input,
                &readings) != 0U)
        {
            message.east = readings.east;
            message.west = readings.west;

            (void)xQueueSend(
                ldr_queue,
                &message,
                portMAX_DELAY);
        }
        else
        {
            Logger_Log(
                LOG_ERROR,
                "Failed to read LDRs");
        }

        vTaskDelayUntil(
            &last_wake,
            pdMS_TO_TICKS(SENSOR_PERIOD_MS));
    }
}

/*
 * CONTROL TASK
 *
 * Responsibility:
 *   Own the FSM and issue panel movement commands.
 */
static void APP_ControlTask(void *parameters)
{
    app_ldr_message_t message;

    app_status_t snapshot;

    tracker_readings_t readings;

    controller_state_t state;

    tracker_direction_t direction;

    (void)parameters;

    for (;;)
    {
        (void)xQueueReceive(
            ldr_queue,
            &message,
            portMAX_DELAY);

        /*
         * RTC time was initialized before the scheduler
         * started and is periodically refreshed by Display.
         */
        APP_CopyStatus(&snapshot);

        state =
            Controller_Update(
                message.east,
                message.west,
                snapshot.time.hours,
                snapshot.time.minutes);

        readings.east = message.east;
        readings.west = message.west;

        if (state == STATE_TRACKING)
        {
            direction =
                Tracker_GetDirection(
                    &readings,
                    app_live_config.dead_band);

            if (direction ==
                TRACKER_DIRECTION_EAST)
            {
                Panel_Move(5U, 1);
            }
            else if (direction ==
                     TRACKER_DIRECTION_WEST)
            {
                Panel_Move(5U, -1);
            }
        }
        else if (state == STATE_NIGHT_PARKING)
        {
            Panel_Park(
                app_live_config.east_park_angle);
        }
        else
        {
            /*
             * CLOUD_HOLD and FAULT both freeze
             * the current panel angle.
             */
            Panel_Hold();
        }

        APP_PublishControl(
            state,
            message.east,
            message.west);
    }
}

/*
 * DISPLAY TASK
 *
 * Responsibility:
 *   Own the LCD, RTC and monitoring outputs.
 */
static void APP_DisplayTask(void *parameters)
{
    TickType_t last_wake =
        xTaskGetTickCount();

    TickType_t last_rtc_read =
        last_wake -
        pdMS_TO_TICKS(RTC_PERIOD_MS);

    controller_state_t last_logged_state =
        STATE_COUNT;

    app_status_t snapshot;

    rtc_time_t time;

    (void)parameters;

    for (;;)
    {
        /*
         * Read RTC once per second.
         */
        if ((xTaskGetTickCount() -
             last_rtc_read) >=
            pdMS_TO_TICKS(RTC_PERIOD_MS))
        {
            if (xSemaphoreTake(
                    i2c_bus_semaphore,
                    portMAX_DELAY) == pdTRUE)
            {
                if (RTC_ReadTime(&time) ==
                    RTC_STATUS_OK)
                {
                    APP_PublishTime(&time);
                }
                else
                {
                    Logger_Log(
                        LOG_ERROR,
                        "RTC read failed");
                }

                xSemaphoreGive(
                    i2c_bus_semaphore);
            }

            last_rtc_read =
                xTaskGetTickCount();
        }

        APP_CopyStatus(&snapshot);

        /*
         * LCD access is protected by the same
         * I2C semaphore used by the RTC.
         */
        if (xSemaphoreTake(
                i2c_bus_semaphore,
                portMAX_DELAY) == pdTRUE)
        {
            LCD_Clear(&lcd_display);

            LCD_SetCursor(
                &lcd_display,
                0U,
                0U);

            LCD_PrintString(
                &lcd_display,
                APP_DisplayStateName(
                    snapshot.state));

            LCD_PrintString(
                &lcd_display,
                " A:");

            APP_LCD_PrintUInt16(
                snapshot.angle);

            LCD_SetCursor(
                &lcd_display,
                1U,
                0U);

            LCD_PrintString(
                &lcd_display,
                "E:");

            APP_LCD_PrintUInt16(
                snapshot.east);

            LCD_PrintString(
                &lcd_display,
                " W:");

            APP_LCD_PrintUInt16(
                snapshot.west);

            xSemaphoreGive(
                i2c_bus_semaphore);
        }

        APP_UpdateLeds(
            snapshot.state);

        /*
         * One UART message for every state transition.
         */
        if (snapshot.state !=
            last_logged_state)
        {
            Logger_Log(
                LOG_EVENT,
                controller_state_names[
                    snapshot.state]);

            last_logged_state =
                snapshot.state;
        }

        vTaskDelayUntil(
            &last_wake,
            pdMS_TO_TICKS(DISPLAY_PERIOD_MS));
    }
}

void APP_Init(void)
{
    rtc_time_t startup_time;

    Logger_Log(
        LOG_BOOT,
        "System Ready");

    APP_InitializeEepromConfig();

    APP_Config_ReadEeprom(
        &app_live_config);

    ldr_queue =
        xQueueCreate(
            LDR_QUEUE_LENGTH,
            sizeof(app_ldr_message_t));

    i2c_bus_semaphore =
        xSemaphoreCreateBinary();

    if ((ldr_queue == NULL) ||
        (i2c_bus_semaphore == NULL))
    {
        Logger_Log(
            LOG_ERROR,
            "FreeRTOS object creation failed");

        return;
    }

    /*
     * Binary semaphores start unavailable.
     */
    xSemaphoreGive(
        i2c_bus_semaphore);

    /*
     * Initialize all I2C devices before starting tasks.
     */
    if (xSemaphoreTake(
            i2c_bus_semaphore,
            portMAX_DELAY) == pdTRUE)
    {
        if (RTC_Init() !=
            RTC_STATUS_OK)
        {
            Logger_Log(
                LOG_ERROR,
                "RTC init failed");
        }

        if (LCD_Init(
                &lcd_display) !=
            LCD_STATUS_OK)
        {
            Logger_Log(
                LOG_ERROR,
                "LCD init failed");
        }

        /*
         * IMPORTANT:
         *
         * Read the real RTC time before the
         * FreeRTOS scheduler starts.
         *
         * This allows INIT to correctly select
         * NIGHT_PARKING when the simulation
         * starts in the middle of the night.
         */
        if (RTC_ReadTime(
                &startup_time) ==
            RTC_STATUS_OK)
        {
            APP_PublishTime(
                &startup_time);

            Logger_Log(
                LOG_INFO,
                "RTC startup time loaded");
        }
        else
        {
            Logger_Log(
                LOG_ERROR,
                "RTC startup read failed");
        }

        xSemaphoreGive(
            i2c_bus_semaphore);
    }

    Panel_Init(
        app_live_config.travel_limit_lower,
        app_live_config.travel_limit_upper,
        app_live_config.east_park_angle);

    Controller_Init(
        app_live_config.dead_band,
        app_live_config.cloud_entry_level,
        app_live_config.cloud_exit_level,
        app_live_config.cloud_confirmation_time_s,
        app_live_config.night_start_hour,
        app_live_config.night_start_minute,
        app_live_config.night_end_hour,
        app_live_config.night_end_minute);
}

void APP_StartScheduler(void)
{
    if ((ldr_queue == NULL) ||
        (i2c_bus_semaphore == NULL))
    {
        return;
    }

    (void)xTaskCreate(
        APP_SensorTask,
        "Sensor",
        APP_TASK_STACK_WORDS,
        NULL,
        3U,
        NULL);

    (void)xTaskCreate(
        APP_ControlTask,
        "Control",
        APP_TASK_STACK_WORDS,
        NULL,
        3U,
        NULL);

    (void)xTaskCreate(
        APP_DisplayTask,
        "Display",
        APP_TASK_STACK_WORDS,
        NULL,
        2U,
        NULL);

    vTaskStartScheduler();

    Logger_Log(
        LOG_ERROR,
        "Scheduler failed to start");
}