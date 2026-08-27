#ifndef LCD_H
#define LCD_H

#include <stdint.h>

#include "lcd_types.h"

/* -------------------------------------------------------------------------- */
/* Initialization                                                              */
/* -------------------------------------------------------------------------- */

lcd_status_t LCD_Init(
    const lcd_t *lcd);

/* -------------------------------------------------------------------------- */
/* Display Control                                                             */
/* -------------------------------------------------------------------------- */

lcd_status_t LCD_Clear(
    const lcd_t *lcd);

lcd_status_t LCD_Home(
    const lcd_t *lcd);

/* -------------------------------------------------------------------------- */
/* Cursor                                                                      */
/* -------------------------------------------------------------------------- */

lcd_status_t LCD_SetCursor(
    const lcd_t *lcd,
    uint8_t row,
    uint8_t column);

/* -------------------------------------------------------------------------- */
/* Write                                                                       */
/* -------------------------------------------------------------------------- */

lcd_status_t LCD_PrintChar(
    const lcd_t *lcd,
    char character);

lcd_status_t LCD_PrintString(
    const lcd_t *lcd,
    const char *string);

#endif