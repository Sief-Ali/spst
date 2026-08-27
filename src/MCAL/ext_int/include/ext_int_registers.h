#ifndef EXT_INT_REGISTERS_H
#define EXT_INT_REGISTERS_H

#include "ext_int_private.h"
#include "ext_int_types.h"

/* -------------------------------------------------------------------------- */
/* Line Descriptor                                                             */
/* -------------------------------------------------------------------------- */

const ext_int_descriptor_t *EXT_INT_RegistersGet(
    ext_int_id_t line);

#endif /* EXT_INT_REGISTERS_H */