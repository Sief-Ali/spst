#ifndef LCD_TYPES_H
#define LCD_TYPES_H

#include <stdint.h>

/* -------------------------------------------------------------------------- */
/* Configuration                                                               */
/* -------------------------------------------------------------------------- */

typedef struct
{
    uint8_t address;

    uint8_t rows;

    uint8_t columns;
} lcd_t;

/* -------------------------------------------------------------------------- */
/* Status                                                                      */
/* -------------------------------------------------------------------------- */

typedef enum
{
    LCD_STATUS_OK = 0,

    LCD_STATUS_ERROR,

    LCD_STATUS_INVALID_PARAMETER
} lcd_status_t;

#endif