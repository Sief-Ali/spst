#include "eeprom.h"

#include <avr/io.h>

#include "bit_utils.h"

static uint8_t EEPROM_IsAddressValid(uint16_t address)
{
	return (uint8_t)(address < EEPROM_SIZE);
}

uint8_t EEPROM_IsBusy(void)
{
	return (uint8_t)READ_BIT(EECR, EEWE);
}

uint8_t EEPROM_ReadByte(uint16_t address)
{
	if (EEPROM_IsAddressValid(address) == 0U)
	{
		return 0U;
	}

	while (EEPROM_IsBusy() != 0U)
	{
	}

	EEAR = address;

	SET_BIT(EECR, EERE);

	return EEDR;
}

void EEPROM_WriteByte(uint16_t address, uint8_t data)
{
	if (EEPROM_IsAddressValid(address) == 0U)
	{
		return;
	}

	while (EEPROM_IsBusy() != 0U)
	{
	}

	EEAR = address;
	EEDR = data;

	/* EEMWE must be set immediately before EEWE on AVR EEPROM writes. */
	SET_BIT(EECR, EEMWE);
	SET_BIT(EECR, EEWE);
}

void EEPROM_UpdateByte(uint16_t address, uint8_t data)
{
	if (EEPROM_ReadByte(address) != data)
	{
		EEPROM_WriteByte(address, data);
	}
}

void EEPROM_Clear(void)
{
	uint16_t address;

	for (address = 0U; address < EEPROM_SIZE; address++)
	{
		EEPROM_UpdateByte(address, 0xFFU);
	}
}
