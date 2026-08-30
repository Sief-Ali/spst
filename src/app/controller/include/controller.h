#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>
#include "controller_types.h"

/* Application state machine controller for Task 2 behavior. */

/* Initialize the controller with the current configuration and state. */
void Controller_Init(
    uint16_t dead_band,
    uint8_t min_angle,
    uint8_t max_angle,
    uint8_t park_angle,
    uint16_t cloud_entry_level,
    uint16_t cloud_exit_level,
    uint16_t cloud_confirmation_time_s,
    uint8_t night_start_hour,
    uint8_t night_start_minute,
    uint8_t night_end_hour,
    uint8_t night_end_minute);

/* Get the current FSM state. */
controller_state_t Controller_GetState(void);

/* Update the controller state based on sensor readings and current time. 
   Returns the new state. */
controller_state_t Controller_Update(
    uint16_t east_reading,
    uint16_t west_reading,
    uint8_t current_hour,
    uint8_t current_minute);

/* Check if state changed since last update. */
uint8_t Controller_StateChanged(void);

#endif /* CONTROLLER_H */
