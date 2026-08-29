#ifndef HAL_RTC_H
#define HAL_RTC_H

#include <stdint.h>

typedef enum
{
	RTC_STATUS_OK = 0U,
	RTC_STATUS_ERROR,
	RTC_STATUS_INVALID_TIME
} rtc_status_t;

typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
} rtc_time_t;

/* Clears the DS1307 clock-halt bit without reinitializing the TWI bus. */
rtc_status_t RTC_Init(void);

/* Reads the current time and converts DS1307 BCD values to binary. */
rtc_status_t RTC_ReadTime(rtc_time_t *time);

/* Converts binary time to BCD and writes it to the DS1307. */
rtc_status_t RTC_WriteTime(const rtc_time_t *time);

#endif /* HAL_RTC_H */
