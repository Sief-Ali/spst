#include "lcd.h"

#include <util/delay.h>
#include <stddef.h>

#include "twi.h"

/* -------------------------------------------------------------------------- */
/* Private Definitions                                                         */
/* -------------------------------------------------------------------------- */

#define LCD_CONTROL_COMMAND    0x00U
#define LCD_CONTROL_DATA       0x40U

/* -------------------------------------------------------------------------- */
/* Private Functions                                                           */
/* -------------------------------------------------------------------------- */

static lcd_status_t LCD_Write(
    const lcd_t *lcd,
    uint8_t control,
    uint8_t value);

static lcd_status_t LCD_Command(
    const lcd_t *lcd,
    uint8_t command);

static lcd_status_t LCD_Data(
    const lcd_t *lcd,
    uint8_t data);

/* -------------------------------------------------------------------------- */
/* Private Functions                                                           */
/* -------------------------------------------------------------------------- */

static lcd_status_t LCD_Write(
    const lcd_t *lcd,
    uint8_t control,
    uint8_t value)
{
    if (lcd == NULL)
    {
        return LCD_STATUS_INVALID_PARAMETER;
    }

    if (TWI_Start() != TWI_STATUS_OK)
    {
        return LCD_STATUS_ERROR;
    }

    if (TWI_SendAddress(
            lcd->address,
            TWI_DIRECTION_WRITE) != TWI_STATUS_OK)
    {
        TWI_Stop();
        return LCD_STATUS_ERROR;
    }

    if (TWI_WriteByte(control) != TWI_STATUS_OK)
    {
        TWI_Stop();
        return LCD_STATUS_ERROR;
    }

    if (TWI_WriteByte(value) != TWI_STATUS_OK)
    {
        TWI_Stop();
        return LCD_STATUS_ERROR;
    }

    TWI_Stop();

    _delay_us(50);

    return LCD_STATUS_OK;
}

static lcd_status_t LCD_Command(
    const lcd_t *lcd,
    uint8_t command)
{
    return LCD_Write(
        lcd,
        LCD_CONTROL_COMMAND,
        command);
}

static lcd_status_t LCD_Data(
    const lcd_t *lcd,
    uint8_t data)
{
    return LCD_Write(
        lcd,
        LCD_CONTROL_DATA,
        data);
}

/* -------------------------------------------------------------------------- */
/* Public API                                                                  */
/* -------------------------------------------------------------------------- */

lcd_status_t LCD_Init(
    const lcd_t *lcd)
{
    if (lcd == NULL)
    {
        return LCD_STATUS_INVALID_PARAMETER;
    }

    _delay_ms(40);

    if (LCD_Command(lcd, 0x38) != LCD_STATUS_OK)
        return LCD_STATUS_ERROR;

    _delay_ms(5);

    if (LCD_Command(lcd, 0x39) != LCD_STATUS_OK)
        return LCD_STATUS_ERROR;

    if (LCD_Command(lcd, 0x14) != LCD_STATUS_OK)
        return LCD_STATUS_ERROR;

    if (LCD_Command(lcd, 0x70) != LCD_STATUS_OK)
        return LCD_STATUS_ERROR;

    if (LCD_Command(lcd, 0x56) != LCD_STATUS_OK)
        return LCD_STATUS_ERROR;

    if (LCD_Command(lcd, 0x6C) != LCD_STATUS_OK)
        return LCD_STATUS_ERROR;

    _delay_ms(200);

    if (LCD_Command(lcd, 0x38) != LCD_STATUS_OK)
        return LCD_STATUS_ERROR;

    if (LCD_Command(lcd, 0x0C) != LCD_STATUS_OK)
        return LCD_STATUS_ERROR;

    if (LCD_Command(lcd, 0x01) != LCD_STATUS_OK)
        return LCD_STATUS_ERROR;

    _delay_ms(2);

    if (LCD_Command(lcd, 0x06) != LCD_STATUS_OK)
        return LCD_STATUS_ERROR;

    return LCD_STATUS_OK;
}

lcd_status_t LCD_Clear(
    const lcd_t *lcd)
{
    lcd_status_t status;

    status = LCD_Command(lcd, 0x01);

    if (status != LCD_STATUS_OK)
    {
        return status;
    }

    _delay_ms(2);

    return LCD_STATUS_OK;
}

lcd_status_t LCD_Home(
    const lcd_t *lcd)
{
    lcd_status_t status;

    status = LCD_Command(lcd, 0x02);

    if (status != LCD_STATUS_OK)
    {
        return status;
    }

    _delay_ms(2);

    return LCD_STATUS_OK;
}

lcd_status_t LCD_SetCursor(
    const lcd_t *lcd,
    uint8_t row,
    uint8_t column)
{
    uint8_t address;

    if (lcd == NULL)
    {
        return LCD_STATUS_INVALID_PARAMETER;
    }

    if ((row >= lcd->rows) ||
        (column >= lcd->columns))
    {
        return LCD_STATUS_INVALID_PARAMETER;
    }

    address = (row == 0U)
        ? column
        : (0x40U + column);

    return LCD_Command(
        lcd,
        0x80U | address);
}

lcd_status_t LCD_PrintChar(
    const lcd_t *lcd,
    char character)
{
    return LCD_Data(
        lcd,
        (uint8_t)character);
}

lcd_status_t LCD_PrintString(
    const lcd_t *lcd,
    const char *string)
{
    if ((lcd == NULL) ||
        (string == NULL))
    {
        return LCD_STATUS_INVALID_PARAMETER;
    }

    while (*string != '\0')
    {
        if (LCD_Data(
                lcd,
                (uint8_t)*string) != LCD_STATUS_OK)
        {
            return LCD_STATUS_ERROR;
        }

        string++;
    }

    return LCD_STATUS_OK;
}