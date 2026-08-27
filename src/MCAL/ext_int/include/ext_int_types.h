#ifndef EXT_INT_TYPES_H
#define EXT_INT_TYPES_H

/* -------------------------------------------------------------------------- */
/* External Interrupt Lines                                                    */
/* -------------------------------------------------------------------------- */

typedef enum
{
    EXT_INT_0,
    EXT_INT_1,
    EXT_INT_2,

    EXT_INT_COUNT
} ext_int_id_t;

/* -------------------------------------------------------------------------- */
/* Trigger Mode                                                                */
/* -------------------------------------------------------------------------- */

typedef enum
{
    EXT_INT_TRIGGER_LOW_LEVEL,
    EXT_INT_TRIGGER_ANY_CHANGE,
    EXT_INT_TRIGGER_FALLING_EDGE,
    EXT_INT_TRIGGER_RISING_EDGE,

    EXT_INT_TRIGGER_COUNT
} ext_int_trigger_t;

/* -------------------------------------------------------------------------- */
/* Callback Type                                                               */
/* -------------------------------------------------------------------------- */

typedef void (*ext_int_callback_t)(void);

/* -------------------------------------------------------------------------- */
/* External Interrupt Configuration                                            */
/* -------------------------------------------------------------------------- */

typedef struct
{
    ext_int_id_t line;
    ext_int_trigger_t trigger;
} ext_int_config_t;

#endif /* EXT_INT_TYPES_H */