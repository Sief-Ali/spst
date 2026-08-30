#include "panel.h"
#include "servo.h"
#include "board.h"

void Panel_Init(void)
{
    /* Servo already initialized in main, but can add any panel-specific setup here. */
}

void Panel_Move(uint8_t step_angle, uint8_t min_angle, uint8_t max_angle, int8_t direction)
{
    uint8_t current_angle;
    int16_t new_angle_temp;
    uint8_t new_angle;

    current_angle = Servo_GetAngle(&servo);

    if (direction > 0)
    {
        /* Move toward higher angle (west). */
        new_angle_temp = (int16_t)current_angle + (int16_t)step_angle;
        if (new_angle_temp > max_angle)
        {
            new_angle = max_angle;
        }
        else
        {
            new_angle = (uint8_t)new_angle_temp;
        }
    }
    else if (direction < 0)
    {
        /* Move toward lower angle (east). */
        new_angle_temp = (int16_t)current_angle - (int16_t)step_angle;
        if (new_angle_temp < min_angle)
        {
            new_angle = min_angle;
        }
        else
        {
            new_angle = (uint8_t)new_angle_temp;
        }
    }
    else
    {
        /* No movement if direction is 0. */
        return;
    }

    Servo_SetAngle(&servo, new_angle);
}

uint8_t Panel_GetAngle(void)
{
    return Servo_GetAngle(&servo);
}

void Panel_SetAngle(uint8_t angle)
{
    Servo_SetAngle(&servo, angle);
}

void Panel_Park(uint8_t park_angle)
{
    Servo_SetAngle(&servo, park_angle);
}

void Panel_Hold(void)
{
    /* Hold just keeps the panel at its current angle; no servo command needed. */
}

void Panel_Wake(void)
{
    /* Wake just resumes tracking from current angle; no specific action needed. */
}

uint8_t Panel_AngleWithinLimits(uint8_t angle, uint8_t min_limit, uint8_t max_limit)
{
    return (angle >= min_limit) && (angle <= max_limit);
}
