#include "rtc.h"

#include <stddef.h>

#include "bit_utils.h"
#include "twi.h"

#define RTC_ADDRESS          0x68U

#define RTC_SECONDS_REG      0x00U
#define RTC_MINUTES_REG      0x01U
#define RTC_HOURS_REG        0x02U

#define RTC_SECONDS_CH_BIT   7U

#define RTC_HOURS_12H_BIT    6U
#define RTC_HOURS_PM_BIT     5U

static uint8_t RTC_BcdToBinary(uint8_t value)
{
    return (uint8_t)(
        ((value >> 4U) * 10U) +
        (value & 0x0FU));
}

static uint8_t RTC_BinaryToBcd(uint8_t value)
{
    return (uint8_t)(
        ((value / 10U) << 4U) |
        (value % 10U));
}

static rtc_status_t RTC_SelectRegister(
    uint8_t reg)
{
    if (TWI_Start() != TWI_STATUS_OK)
    {
        return RTC_STATUS_ERROR;
    }

    if (TWI_SendAddress(
            RTC_ADDRESS,
            TWI_DIRECTION_WRITE) !=
        TWI_STATUS_OK)
    {
        TWI_Stop();
        return RTC_STATUS_ERROR;
    }

    if (TWI_WriteByte(reg) !=
        TWI_STATUS_OK)
    {
        TWI_Stop();
        return RTC_STATUS_ERROR;
    }

    return RTC_STATUS_OK;
}

static rtc_status_t RTC_ReadRegisters(
    uint8_t *seconds,
    uint8_t *minutes,
    uint8_t *hours)
{
    if ((seconds == NULL) ||
        (minutes == NULL) ||
        (hours == NULL))
    {
        return RTC_STATUS_INVALID_TIME;
    }

    if (RTC_SelectRegister(
            RTC_SECONDS_REG) !=
        RTC_STATUS_OK)
    {
        return RTC_STATUS_ERROR;
    }

    if (TWI_RepeatedStart() !=
            TWI_STATUS_OK ||
        TWI_SendAddress(
            RTC_ADDRESS,
            TWI_DIRECTION_READ) !=
            TWI_STATUS_OK ||
        TWI_ReadByteAck(seconds) !=
            TWI_STATUS_OK ||
        TWI_ReadByteAck(minutes) !=
            TWI_STATUS_OK ||
        TWI_ReadByteNack(hours) !=
            TWI_STATUS_OK)
    {
        TWI_Stop();
        return RTC_STATUS_ERROR;
    }

    TWI_Stop();

    return RTC_STATUS_OK;
}

/*
 * Converts the DS1307 hour register into 24-hour binary time.
 *
 * 24-hour mode:
 *   bit 6 = 0
 *   bits 5:0 = BCD hour
 *
 * 12-hour mode:
 *   bit 6 = 1
 *   bit 5 = PM
 *   bits 4:0 = BCD hour
 */
static rtc_status_t RTC_DecodeHours(
    uint8_t raw_hours,
    uint8_t *hours)
{
    uint8_t hour_12;
    uint8_t is_pm;

    if (hours == NULL)
    {
        return RTC_STATUS_INVALID_TIME;
    }

    /*
     * 24-hour mode.
     */
    if ((raw_hours & (1U << RTC_HOURS_12H_BIT)) == 0U)
    {
        uint8_t hour_24 =
            RTC_BcdToBinary(
                (uint8_t)(raw_hours & 0x3FU));

        if (hour_24 >= 24U)
        {
            return RTC_STATUS_INVALID_TIME;
        }

        *hours = hour_24;

        return RTC_STATUS_OK;
    }

    /*
     * 12-hour mode.
     */
    hour_12 =
        RTC_BcdToBinary(
            (uint8_t)(raw_hours & 0x1FU));

    is_pm = (uint8_t)(
        (raw_hours & (1U << RTC_HOURS_PM_BIT)) != 0U);

    if ((hour_12 < 1U) ||
        (hour_12 > 12U))
    {
        return RTC_STATUS_INVALID_TIME;
    }

    /*
     * Convert:
     *
     * 12 AM -> 00
     * 1 AM  -> 01
     * ...
     * 11 AM -> 11
     * 12 PM -> 12
     * 1 PM  -> 13
     * ...
     * 11 PM -> 23
     */
    if (is_pm != 0U)
    {
        if (hour_12 == 12U)
        {
            *hours = 12U;
        }
        else
        {
            *hours =
                (uint8_t)(hour_12 + 12U);
        }
    }
    else
    {
        if (hour_12 == 12U)
        {
            *hours = 0U;
        }
        else
        {
            *hours = hour_12;
        }
    }

    return RTC_STATUS_OK;
}

rtc_status_t RTC_Init(void)
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;

    /*
     * Read the current registers first.
     *
     * This lets us clear the clock-halt bit without
     * destroying the current time.
     */
    if (RTC_ReadRegisters(
            &seconds,
            &minutes,
            &hours) !=
        RTC_STATUS_OK)
    {
        return RTC_STATUS_ERROR;
    }

    /*
     * Stop the oscillator if CH=1.
     */
    CLEAR_BIT(
        seconds,
        RTC_SECONDS_CH_BIT);

    /*
     * Convert the current time to binary.
     * This also handles a device that was configured
     * in 12-hour mode.
     */
    rtc_time_t current_time;

    current_time.seconds =
        RTC_BcdToBinary(
            (uint8_t)(seconds & 0x7FU));

    current_time.minutes =
        RTC_BcdToBinary(minutes);

    if (RTC_DecodeHours(
            hours,
            &current_time.hours) !=
        RTC_STATUS_OK)
    {
        return RTC_STATUS_INVALID_TIME;
    }

    if ((current_time.seconds >= 60U) ||
        (current_time.minutes >= 60U) ||
        (current_time.hours >= 24U))
    {
        return RTC_STATUS_INVALID_TIME;
    }

    /*
     * Rewrite the time using 24-hour mode.
     *
     * RTC_WriteTime() also writes the cleared CH bit,
     * therefore the oscillator is guaranteed to run.
     */
    return RTC_WriteTime(
        &current_time);
}

rtc_status_t RTC_ReadTime(
    rtc_time_t *time)
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;

    if (time == NULL)
    {
        return RTC_STATUS_INVALID_TIME;
    }

    if (RTC_ReadRegisters(
            &seconds,
            &minutes,
            &hours) !=
        RTC_STATUS_OK)
    {
        return RTC_STATUS_ERROR;
    }

    /*
     * CH must never be interpreted as part of BCD seconds.
     */
    CLEAR_BIT(
        seconds,
        RTC_SECONDS_CH_BIT);

    time->seconds =
        RTC_BcdToBinary(seconds);

    time->minutes =
        RTC_BcdToBinary(minutes);

    if (RTC_DecodeHours(
            hours,
            &time->hours) !=
        RTC_STATUS_OK)
    {
        return RTC_STATUS_INVALID_TIME;
    }

    /*
     * Final sanity validation.
     */
    if ((time->seconds >= 60U) ||
        (time->minutes >= 60U) ||
        (time->hours >= 24U))
    {
        return RTC_STATUS_INVALID_TIME;
    }

    return RTC_STATUS_OK;
}

rtc_status_t RTC_WriteTime(
    const rtc_time_t *time)
{
    if ((time == NULL) ||
        (time->seconds >= 60U) ||
        (time->minutes >= 60U) ||
        (time->hours >= 24U))
    {
        return RTC_STATUS_INVALID_TIME;
    }

    /*
     * Writing hours with bit 6 clear selects
     * DS1307 24-hour mode.
     */
    if (RTC_SelectRegister(
            RTC_SECONDS_REG) !=
        RTC_STATUS_OK)
    {
        return RTC_STATUS_ERROR;
    }

    if (TWI_WriteByte(
            RTC_BinaryToBcd(
                time->seconds)) !=
        TWI_STATUS_OK)
    {
        TWI_Stop();
        return RTC_STATUS_ERROR;
    }

    if (TWI_WriteByte(
            RTC_BinaryToBcd(
                time->minutes)) !=
        TWI_STATUS_OK)
    {
        TWI_Stop();
        return RTC_STATUS_ERROR;
    }

    /*
     * Binary hours are always written in 24-hour mode.
     * Bit 6 remains zero.
     */
    if (TWI_WriteByte(
            RTC_BinaryToBcd(
                time->hours)) !=
        TWI_STATUS_OK)
    {
        TWI_Stop();
        return RTC_STATUS_ERROR;
    }

    TWI_Stop();

    return RTC_STATUS_OK;
}