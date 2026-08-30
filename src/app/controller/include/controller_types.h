#ifndef CONTROLLER_TYPES_H
#define CONTROLLER_TYPES_H

/* Steps in the MetroExpress ticket-purchase state machine. */
typedef enum
{
    STATE_INIT = 0,

    STATE_TRACKING,

    STATE_NIGHT_PARKING,

    STATE_CLOUD_HOLD,

    STATE_FAULT,

    STATE_COUNT

} controller_state_t;

/* Array mapping each state enum value to its human-readable string name. */
extern const char * const controller_state_names[STATE_COUNT];

#endif
