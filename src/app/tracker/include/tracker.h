#ifndef APP_TRACKER_H
#define APP_TRACKER_H

#include <stdint.h>

#include "tracker_types.h"

tracker_direction_t Tracker_GetDirection(
    const tracker_readings_t *readings,
    uint16_t dead_band);

uint8_t Tracker_GetNextAngle(
    uint8_t current_angle,
    tracker_direction_t direction,
    uint8_t step_angle,
    uint8_t min_angle,
    uint8_t max_angle);

uint16_t Tracker_GetAverage(
    const tracker_readings_t *readings);

#endif /* APP_TRACKER_H */
