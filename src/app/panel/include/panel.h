#ifndef PANEL_H
#define PANEL_H

#include <stdint.h>

/* Panel control interface for movement, angle management, and parking/waking. */

void Panel_Init(void);

/* Move the panel toward a target direction by step_angle, respecting limits. */
void Panel_Move(uint8_t step_angle, uint8_t min_angle, uint8_t max_angle, int8_t direction);

/* Get the current servo angle. */
uint8_t Panel_GetAngle(void);

/* Set the servo angle directly. */
void Panel_SetAngle(uint8_t angle);

/* Park the panel at the configured east parking angle (typically 20 degrees). */
void Panel_Park(uint8_t park_angle);

/* Hold the panel at the current angle (e.g., during cloud or fault). */
void Panel_Hold(void);

/* Wake (unpark) the panel to prepare for tracking. */
void Panel_Wake(void);

/* Check if the current angle is within the valid travel limits. */
uint8_t Panel_AngleWithinLimits(uint8_t angle, uint8_t min_limit, uint8_t max_limit);

#endif /* PANEL_H */
