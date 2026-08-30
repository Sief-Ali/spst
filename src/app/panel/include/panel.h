#ifndef PANEL_H
#define PANEL_H

#include <stdint.h>

/* Panel is the sole owner of travel limits and commanded angle. */

void Panel_Init(uint8_t min_angle, uint8_t max_angle, uint8_t initial_angle);

void Panel_SetLimits(uint8_t min_angle, uint8_t max_angle);

/* Move the panel toward a target direction by step_angle, respecting limits. */
void Panel_Move(uint8_t step_angle, int8_t direction);

uint8_t Panel_GetAngle(void);

void Panel_SetAngle(uint8_t angle);

uint8_t Panel_GetMinAngle(void);

uint8_t Panel_GetMaxAngle(void);

/* Park the panel at the configured east parking angle (typically 20 degrees). */
void Panel_Park(uint8_t park_angle);

/* Hold the panel at the current angle (e.g., during cloud or fault). */
void Panel_Hold(void);

/* Wake (unpark) the panel to prepare for tracking. */
void Panel_Wake(void);

/* Check if an angle is within the panel travel limits. */
uint8_t Panel_AngleWithinLimits(uint8_t angle);

#endif /* PANEL_H */
