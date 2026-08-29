#include "rtc.h"

#include <stddef.h>

#include "bit_utils.h"
#include "twi.h"

#define RTC_ADDRESS       0x68U
#define RTC_SECONDS_REG   0x00U
#define RTC_MINUTES_REG   0x01U
#define RTC_HOURS_REG     0x02U

#define RTC_SECONDS_CH_BIT  7U
#define RTC_HOURS_12H_BIT   6U

static uint8_t RTC_BcdToBinary(uint8_t value)
{
	return (uint8_t)(((value >> 4U) * 10U) + (value & 0x0FU));
}

static uint8_t RTC_BinaryToBcd(uint8_t value)
{
	return (uint8_t)(((value / 10U) << 4U) | (value % 10U));
}

static rtc_status_t RTC_SelectRegister(uint8_t reg)
{
	if (TWI_Start() != TWI_STATUS_OK)
	{
		return RTC_STATUS_ERROR;
	}

	if (TWI_SendAddress(RTC_ADDRESS, TWI_DIRECTION_WRITE) != TWI_STATUS_OK)
	{
		TWI_Stop();
		return RTC_STATUS_ERROR;
	}

	if (TWI_WriteByte(reg) != TWI_STATUS_OK)
	{
		TWI_Stop();
		return RTC_STATUS_ERROR;
	}

	return RTC_STATUS_OK;
}

rtc_status_t RTC_Init(void)
{
	uint8_t seconds;

	if (RTC_SelectRegister(RTC_SECONDS_REG) != RTC_STATUS_OK)
	{
		return RTC_STATUS_ERROR;
	}

	if (TWI_RepeatedStart() != TWI_STATUS_OK ||
		TWI_SendAddress(RTC_ADDRESS, TWI_DIRECTION_READ) != TWI_STATUS_OK ||
		TWI_ReadByteNack(&seconds) != TWI_STATUS_OK)
	{
		TWI_Stop();
		return RTC_STATUS_ERROR;
	}

	TWI_Stop();
	CLEAR_BIT(seconds, RTC_SECONDS_CH_BIT);

	if (RTC_SelectRegister(RTC_SECONDS_REG) != RTC_STATUS_OK ||
		TWI_WriteByte(seconds) != TWI_STATUS_OK)
	{
		TWI_Stop();
		return RTC_STATUS_ERROR;
	}

	TWI_Stop();
	return RTC_STATUS_OK;
}

rtc_status_t RTC_ReadTime(rtc_time_t *time)
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;

	if (time == NULL)
	{
		return RTC_STATUS_INVALID_TIME;
	}

	if (RTC_SelectRegister(RTC_SECONDS_REG) != RTC_STATUS_OK ||
		TWI_RepeatedStart() != TWI_STATUS_OK ||
		TWI_SendAddress(RTC_ADDRESS, TWI_DIRECTION_READ) != TWI_STATUS_OK ||
		TWI_ReadByteAck(&seconds) != TWI_STATUS_OK ||
		TWI_ReadByteAck(&minutes) != TWI_STATUS_OK ||
		TWI_ReadByteNack(&hours) != TWI_STATUS_OK)
	{
		TWI_Stop();
		return RTC_STATUS_ERROR;
	}

	TWI_Stop();

	CLEAR_BIT(seconds, RTC_SECONDS_CH_BIT);
	CLEAR_BIT(hours, RTC_HOURS_12H_BIT);

	time->seconds = RTC_BcdToBinary(seconds);
	time->minutes = RTC_BcdToBinary(minutes);
	time->hours = RTC_BcdToBinary(hours);

	if ((time->seconds >= 60U) ||
		(time->minutes >= 60U) ||
		(time->hours >= 24U))
	{
		return RTC_STATUS_INVALID_TIME;
	}

	return RTC_STATUS_OK;
}

rtc_status_t RTC_WriteTime(const rtc_time_t *time)
{
	if ((time == NULL) ||
		(time->seconds >= 60U) ||
		(time->minutes >= 60U) ||
		(time->hours >= 24U))
	{
		return RTC_STATUS_INVALID_TIME;
	}

	if (RTC_SelectRegister(RTC_SECONDS_REG) != RTC_STATUS_OK ||
		TWI_WriteByte(RTC_BinaryToBcd(time->seconds)) != TWI_STATUS_OK ||
		TWI_WriteByte(RTC_BinaryToBcd(time->minutes)) != TWI_STATUS_OK ||
		TWI_WriteByte(RTC_BinaryToBcd(time->hours)) != TWI_STATUS_OK)
	{
		TWI_Stop();
		return RTC_STATUS_ERROR;
	}

	TWI_Stop();
	return RTC_STATUS_OK;
}
