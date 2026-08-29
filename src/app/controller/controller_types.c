#include "controller_types.h"

/* Display names for each state, indexed directly by controller_state_t values */
const char * const controller_state_names[STATE_COUNT] = {
    [STATE_INIT]               = "STATE_INIT",
    [STATE_TRACKING]           = "STATE_TRACKING",
    [STATE_NIGHT_PARKING]      = "STATE_NIGHT_PARKING",
    [STATE_CLAUDE_HOLD]        = "STATE_CLAUDE_HOLD",
    [STATE_FAULT]              = "STATE_FAULT",
};