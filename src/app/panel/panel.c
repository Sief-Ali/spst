#include "panel.h"

#include "servo.h"
#include "board.h"

static uint8_t panel_min_angle;
static uint8_t panel_max_angle;
static uint8_t panel_current_angle;

static uint8_t Panel_ClampAngle(uint8_t angle)
{
    if (angle < panel_min_angle)
    {
        return panel_min_angle;
    }

    if (angle > panel_max_angle)
    {
        return panel_max_angle;
    }

    return angle;
}

void Panel_Init(uint8_t min_angle, uint8_t max_angle, uint8_t initial_angle)
{
    Panel_SetLimits(min_angle, max_angle);
    Panel_SetAngle(initial_angle);
}

void Panel_SetLimits(uint8_t min_angle, uint8_t max_angle)
{
    if (max_angle < min_angle)
    {
        uint8_t swap = min_angle;
        min_angle = max_angle;
        max_angle = swap;
    }

    panel_min_angle = min_angle;
    panel_max_angle = max_angle;
    panel_current_angle = Panel_ClampAngle(panel_current_angle);
}

void Panel_Move(uint8_t step_angle, int8_t direction)
{
    int16_t new_angle;

    if (direction == 0)
    {
        return;
    }

    if (direction > 0)
    {
        new_angle = (int16_t)panel_current_angle + (int16_t)step_angle;
    }
    else
    {
        new_angle = (int16_t)panel_current_angle - (int16_t)step_angle;
    }

    if (new_angle < 0)
    {
        new_angle = 0;
    }
    else if (new_angle > 255)
    {
        new_angle = 255;
    }

    Panel_SetAngle((uint8_t)new_angle);
}

uint8_t Panel_GetAngle(void)
{
    return panel_current_angle;
}

void Panel_SetAngle(uint8_t angle)
{
    panel_current_angle = Panel_ClampAngle(angle);
    Servo_SetAngle(&servo, panel_current_angle);
}

uint8_t Panel_GetMinAngle(void)
{
    return panel_min_angle;
}

uint8_t Panel_GetMaxAngle(void)
{
    return panel_max_angle;
}

void Panel_Park(uint8_t park_angle)
{
    Panel_SetAngle(park_angle);
}

void Panel_Hold(void)
{
    /*
     * No new servo command is required.
     * The panel remains at its current commanded angle.
     */
}

void Panel_Wake(void)
{
    /*
     * Tracking naturally resumes from the current angle.
     */
}

uint8_t Panel_AngleWithinLimits(uint8_t angle)
{
    return (angle >= panel_min_angle) &&
           (angle <= panel_max_angle);
}
