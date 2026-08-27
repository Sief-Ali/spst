#ifndef TWI_H
#define TWI_H

#include <stdint.h>

#include "twi_types.h"

/* -------------------------------------------------------------------------- */
/* Initialization                                                              */
/* -------------------------------------------------------------------------- */

void TWI_Init(
    const twi_config_t *config);

/* -------------------------------------------------------------------------- */
/* Bus Control                                                                 */
/* -------------------------------------------------------------------------- */

twi_status_t TWI_Start(void);

twi_status_t TWI_RepeatedStart(void);

void TWI_Stop(void);

/* -------------------------------------------------------------------------- */
/* Address                                                                     */
/* -------------------------------------------------------------------------- */

twi_status_t TWI_SendAddress(
    uint8_t address,
    twi_direction_t direction);

/* -------------------------------------------------------------------------- */
/* Data                                                                        */
/* -------------------------------------------------------------------------- */

twi_status_t TWI_WriteByte(
    uint8_t data);

twi_status_t TWI_ReadByteAck(
    uint8_t *data);

twi_status_t TWI_ReadByteNack(
    uint8_t *data);

#endif