/**
 * @file i2cBitBang.h
 * @author Seb Madgwick
 * @brief I2C master bit-bang.
 */

#ifndef I2C_BIT_BANG_H
#define I2C_BIT_BANG_H

//------------------------------------------------------------------------------
// Includes

#include "I2CSlaveAddress.h"
#include <stdbool.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief I2C bit-bang structure.  Structure members are used internally and
 * should not be used by the user application.
 */
typedef struct {
    void (*waitHalfClockCycle)();
    void (*writeScl)(const bool);
    bool (*readSda)();
    void (*writeSda)(const bool);
} I2cBitBang;

//------------------------------------------------------------------------------
// Function prototypes

void I2CBitBangInitialise(I2cBitBang * const i2cBitBang, void (*waitHalfClockCycle)(), void (*writeScl)(const bool), bool (*readSda)(), void (*writeSda)(const bool));
void I2CBitBangBusClear(const I2cBitBang * const i2cBitBang);
void I2CBitBangStart(const I2cBitBang * const i2cBitBang);
void I2CBitBangStop(const I2cBitBang * const i2cBitBang);
bool I2CBitBangSend(const I2cBitBang * const i2cBitBang, const char byte);
char I2CBitBangReceive(const I2cBitBang * const i2cBitBang);
void I2CBitBangAck(const I2cBitBang * const i2cBitBang);
void I2CBitBangNack(const I2cBitBang * const i2cBitBang);

#endif

//------------------------------------------------------------------------------
// End of file
