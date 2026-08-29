#ifndef APP_TRACKER_TYPES_H
#define APP_TRACKER_TYPES_H

#include <stdint.h>

typedef struct
{
    uint16_t east;
    uint16_t west;
} tracker_readings_t;

typedef enum
{
    TRACKER_DIRECTION_HOLD,
    TRACKER_DIRECTION_EAST,
    TRACKER_DIRECTION_WEST
} tracker_direction_t;

#endif /* APP_TRACKER_TYPES_H */
