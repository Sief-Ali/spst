#include "controller.h"
#include <stdio.h>

typedef struct
{
    controller_state_t current_state;
    controller_state_t previous_state;

    uint16_t dead_band;
    uint8_t min_angle;
    uint8_t max_angle;
    uint8_t park_angle;

    uint16_t cloud_entry_level;
    uint16_t cloud_exit_level;
    uint16_t cloud_confirmation_time_s;

    uint16_t night_start_minutes;
    uint16_t night_end_minutes;

    uint32_t cloud_low_count;
    uint32_t cloud_low_threshold;
} controller_state_machine_t;

static controller_state_machine_t fsm;

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
    uint8_t night_end_minute)
{
    fsm.current_state = STATE_INIT;
    fsm.previous_state = STATE_INIT;

    fsm.dead_band = dead_band;
    fsm.min_angle = min_angle;
    fsm.max_angle = max_angle;
    fsm.park_angle = park_angle;

    fsm.cloud_entry_level = cloud_entry_level;
    fsm.cloud_exit_level = cloud_exit_level;
    fsm.cloud_confirmation_time_s = cloud_confirmation_time_s;

    fsm.night_start_minutes = (uint16_t)night_start_hour * 60U + night_start_minute;
    fsm.night_end_minutes = (uint16_t)night_end_hour * 60U + night_end_minute;

    fsm.cloud_low_count = 0U;
    fsm.cloud_low_threshold = cloud_confirmation_time_s * 10U;
}

controller_state_t Controller_GetState(void)
{
    return fsm.current_state;
}

uint8_t Controller_StateChanged(void)
{
    return (fsm.current_state != fsm.previous_state) ? 1U : 0U;
}

static uint16_t get_current_time_minutes(uint8_t hour, uint8_t minute)
{
    return (uint16_t)hour * 60U + minute;
}

static uint8_t is_night_time(uint16_t current_minutes)
{
    if (fsm.night_start_minutes >= fsm.night_end_minutes)
    {
        return (current_minutes >= fsm.night_start_minutes) || (current_minutes < fsm.night_end_minutes);
    }
    else
    {
        return (current_minutes >= fsm.night_start_minutes) && (current_minutes < fsm.night_end_minutes);
    }
}

static uint8_t is_valid_reading(uint16_t reading)
{
    return (reading >= 21U) && (reading <= 999U);
}

static uint8_t are_both_readings_extreme(uint16_t east, uint16_t west)
{
    uint8_t east_valid = is_valid_reading(east);
    uint8_t west_valid = is_valid_reading(west);

    return (east_valid == 0U) && (west_valid == 0U);
}

static uint16_t get_light_level(uint16_t east, uint16_t west)
{
    return (east + west) / 2U;
}

controller_state_t Controller_Update(
    uint16_t east_reading,
    uint16_t west_reading,
    uint8_t current_hour,
    uint8_t current_minute)
{
    uint16_t light_level;
    uint16_t current_minutes;
    uint8_t is_night;
    uint8_t is_fault_condition;

    fsm.previous_state = fsm.current_state;

    current_minutes = get_current_time_minutes(current_hour, current_minute);
    is_night = is_night_time(current_minutes);
    is_fault_condition = are_both_readings_extreme(east_reading, west_reading);

    if (fsm.current_state == STATE_INIT)
    {
        fsm.current_state = STATE_TRACKING;
    }

    if (is_night)
    {
        fsm.current_state = STATE_NIGHT_PARKING;
        fsm.cloud_low_count = 0U;
    }
    else if (is_fault_condition)
    {
        fsm.current_state = STATE_FAULT;
        fsm.cloud_low_count = 0U;
    }
    else if (fsm.current_state == STATE_FAULT)
    {
        fsm.current_state = STATE_TRACKING;
        fsm.cloud_low_count = 0U;
    }
    else if (fsm.current_state == STATE_CLOUD_HOLD)
    {
        light_level = get_light_level(east_reading, west_reading);

        if (light_level >= fsm.cloud_exit_level)
        {
            fsm.current_state = STATE_TRACKING;
            fsm.cloud_low_count = 0U;
        }
    }
    else if (fsm.current_state == STATE_TRACKING)
    {
        light_level = get_light_level(east_reading, west_reading);

        if (light_level <= fsm.cloud_entry_level)
        {
            fsm.cloud_low_count++;

            if (fsm.cloud_low_count >= fsm.cloud_low_threshold)
            {
                fsm.current_state = STATE_CLOUD_HOLD;
                fsm.cloud_low_count = 0U;
            }
        }
        else
        {
            fsm.cloud_low_count = 0U;
        }
    }

    return fsm.current_state;
}
