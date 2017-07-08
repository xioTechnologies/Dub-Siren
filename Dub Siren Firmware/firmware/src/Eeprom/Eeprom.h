/**
 * @file Eeprom.h
 * @author Seb Madgwick
 * @brief EEPROM library for Microchip 24AA32A.
 */

//------------------------------------------------------------------------------
// Includes

#ifndef EEPROM_H
#define EEPROM_H

//------------------------------------------------------------------------------
// Includes

#include "I2cBitBang/I2cBitBang.h"
#include <stddef.h>

//------------------------------------------------------------------------------
// Definitions

#define EEPROM_SIZE (0x3FF)

//------------------------------------------------------------------------------
// Function prototypes

void EepromRead(const I2cBitBang * const i2cBitBang, const unsigned int address, char *const destination, const size_t numberOfBytes);
void EepromWrite(const I2cBitBang * const i2cBitBang, unsigned int address, const char* source, const size_t numberOfBytes);
void EepromEraseAll(const I2cBitBang * const i2cBitBang);

#endif

//------------------------------------------------------------------------------
// End of file
