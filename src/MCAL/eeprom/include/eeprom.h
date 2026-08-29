#ifndef EEPROM_DRIVER_H
#define EEPROM_DRIVER_H

#include <stdint.h>

/* ATmega32 contains 1 KiB of internal EEPROM. */
#define EEPROM_SIZE 1024U

/* Returns non-zero while an EEPROM write is in progress. */
uint8_t EEPROM_IsBusy(void);

/* Reads one byte from the supplied EEPROM address. */
uint8_t EEPROM_ReadByte(uint16_t address);

/* Writes one byte and waits until the write completes. */
void EEPROM_WriteByte(uint16_t address, uint8_t data);

/* Writes one byte only when its value differs from the stored value. */
void EEPROM_UpdateByte(uint16_t address, uint8_t data);

/* Erases the EEPROM by writing 0xFF to every address. */
void EEPROM_Clear(void);

#endif /* EEPROM_DRIVER_H */
