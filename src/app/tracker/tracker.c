#include "tracker.h"

#include <stddef.h>

tracker_direction_t Tracker_GetDirection(
    const tracker_readings_t *readings,
    uint16_t dead_band)
{
    int16_t difference;
    uint16_t magnitude;

    if (readings == NULL)
    {
        return TRACKER_DIRECTION_HOLD;
    }

    difference = (int16_t)readings->east - (int16_t)readings->west;
    magnitude = (uint16_t)((difference < 0) ? -difference : difference);

    if (magnitude <= dead_band)
    {
        return TRACKER_DIRECTION_HOLD;
    }

    return (difference > 0)
        ? TRACKER_DIRECTION_EAST
        : TRACKER_DIRECTION_WEST;
}

uint8_t Tracker_GetNextAngle(
    uint8_t current_angle,
    tracker_direction_t direction,
    uint8_t step_angle,
    uint8_t min_angle,
    uint8_t max_angle)
{
    uint16_t next_angle = current_angle;

    if (direction == TRACKER_DIRECTION_EAST)
    {
        next_angle += step_angle;
    }
    else if (direction == TRACKER_DIRECTION_WEST)
    {
        if (step_angle > next_angle)
        {
            next_angle = 0U;
        }
        else
        {
            next_angle -= step_angle;
        }
    }

    if (next_angle < min_angle)
    {
        next_angle = min_angle;
    }

    if (next_angle > max_angle)
    {
        next_angle = max_angle;
    }

    return (uint8_t)next_angle;
}

uint16_t Tracker_GetAverage(
    const tracker_readings_t *readings)
{
    if (readings == NULL)
    {
        return 0U;
    }

    return (uint16_t)(((uint32_t)readings->east + readings->west) / 2U);
}
