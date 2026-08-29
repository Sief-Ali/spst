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

#define EEPROM_TEST_ADDRESS 0U
#define EEPROM_TEST_VALUE   0xA5U

static void APP_TestEeprom(void)
{
    uint8_t read_value;
    char message[48];

    EEPROM_Clear();
    EEPROM_WriteByte(EEPROM_TEST_ADDRESS, EEPROM_TEST_VALUE);
    read_value = EEPROM_ReadByte(EEPROM_TEST_ADDRESS);

    if (read_value == EEPROM_TEST_VALUE)
    {
        snprintf(
            message,
            sizeof(message),
            "EEPROM test passed: 0x%02X",
            read_value);
        Logger_Log(LOG_INFO, message);
    }
    else
    {
        snprintf(
            message,
            sizeof(message),
            "EEPROM test failed: 0x%02X",
            read_value);
        Logger_Log(LOG_ERROR, message);
    }
}

static void APP_TestLeds(void)
{
    LED_On(&led.ready);
    _delay_ms(1000);
    LED_Off(&led.ready);

    LED_On(&led.processing);
    _delay_ms(1000);
    LED_Off(&led.processing);

    LED_On(&led.error);
    _delay_ms(1000);
    LED_Off(&led.error);
}

static void APP_ShowServoAngle(uint8_t angle)
{
    char message[17];

    snprintf(message, sizeof(message), "Servo angle: %u", angle);
    LCD_Clear(&lcd_display);
    LCD_SetCursor(&lcd_display, 0U, 0U);
    LCD_PrintString(&lcd_display, message);
}

static void APP_TestServo(uint8_t angle)
{
    char message[32];

    snprintf(message, sizeof(message), "Moving servo: %u", angle);
    Logger_Log(LOG_EVENT, message);

    Servo_SetAngle(&servo, angle);
    APP_ShowServoAngle(Servo_GetAngle(&servo));
    _delay_ms(2000);
}

static void APP_TestRtc(void)
{
    rtc_time_t time;
    char message[32];

    if (RTC_ReadTime(&time) == RTC_STATUS_OK)
    {
        snprintf(
            message,
            sizeof(message),
            "RTC: %02u:%02u:%02u",
            time.hours,
            time.minutes,
            time.seconds);
        Logger_Log(LOG_INFO, message);
    }
    else
    {
        Logger_Log(LOG_ERROR, "RTC read failed");
    }
}

void APP_Init(void)
{
    Logger_Log(
      LOG_BOOT,
      "System Ready");

    APP_TestEeprom();
    APP_TestLeds();

    if (RTC_Init() != RTC_STATUS_OK)
    {
        Logger_Log(LOG_ERROR, "RTC init failed");
    }

    LCD_Init(&lcd_display);
    LCD_Clear(&lcd_display);
    LCD_SetCursor(&lcd_display, 1, 0);
    LCD_PrintString(&lcd_display, "Peripheral test");
}

void APP_Run(void)
{

    Logger_Log(
      LOG_EVENT,
      "Application Running");
    
      while (1)
      {
          APP_TestRtc();
          APP_TestLeds();
          APP_TestServo(0U);
          APP_TestServo(90U);
          APP_TestServo(180U);
      }
}