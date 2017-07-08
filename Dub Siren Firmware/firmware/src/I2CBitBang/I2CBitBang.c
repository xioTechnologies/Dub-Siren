/**
 * @file i2cBitBang.c
 * @author Seb Madgwick
 * @brief Implements I2C bit-bang.  Requires use of open-drain outputs.
 */

//------------------------------------------------------------------------------
// Includes

#include "i2cBitBang.h"

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises I2C bit-bang structure.
 * @param i2cBitBang I2C bit-bang structure to be initialise.
 * @param waitHalfClockCycle Function that implements a delay for half the I2C
 * clock period.
 * @param writeScl Function that implements a write to the SCL pin.
 * @param readSda Function that returns the value of the SDA pin.
 * @param writeSda Function that implements a write to the SDA pin.
 */
void I2CBitBangInitialise(I2cBitBang * const i2cBitBang, void (*waitHalfClockCycle)(), void (*writeScl)(const bool), bool(*readSda)(), void (*writeSda)(const bool)) {
    i2cBitBang->waitHalfClockCycle = waitHalfClockCycle;
    i2cBitBang->writeScl = writeScl;
    i2cBitBang->readSda = readSda;
    i2cBitBang->writeSda = writeSda;
}

/**
 * @brief Performs the bus clear procedure.  This procedure should be performed
 * if the SDA line is stuck low.
 * @see Page 20 of UM10204 I2C-bus specification and user manual Rev. 6 - 4
 * April 2014.
 * @param i2cBitBang I2C bit-bang structure.
 */
void I2CBitBangBusClear(const I2cBitBang * const i2cBitBang) {
    int i;
    for (i = 0; i < 9; i++) {
        i2cBitBang->waitHalfClockCycle();
        if (i2cBitBang->readSda() == true) { // sample data during clock high period
            break; // stop once SDA is released otherwise it may get stuck again
        }
        i2cBitBang->writeScl(false);
        i2cBitBang->waitHalfClockCycle();
        i2cBitBang->writeScl(true);
    }
}

/**
 * @brief Performs start condition.
 * @param i2cBitBang I2C bit-bang structure.
 */
void I2CBitBangStart(const I2cBitBang * const i2cBitBang) {
    i2cBitBang->writeScl(true);
    i2cBitBang->writeSda(true);
    i2cBitBang->waitHalfClockCycle();
    i2cBitBang->writeSda(false);
    i2cBitBang->waitHalfClockCycle();
    i2cBitBang->writeScl(false);
}

/**
 * @brief Performs stop condition.
 * @param i2cBitBang I2C bit-bang structure.
 */
void I2CBitBangStop(const I2cBitBang * const i2cBitBang) {
    i2cBitBang->writeSda(false);
    i2cBitBang->waitHalfClockCycle();
    i2cBitBang->writeScl(true);
    i2cBitBang->waitHalfClockCycle();
    i2cBitBang->writeSda(true);
}

/**
 * @brief Sends byte and checks for ACK.
 * @param i2cBitBang I2C bit-bang structure.
 * @param byte Byte to be sent.
 * @return True if an ACK was received.
 */
bool I2CBitBangSend(const I2cBitBang * const i2cBitBang, const char byte) {

    // Data
    int bitNumber;
    for (bitNumber = 7; bitNumber >= 0; bitNumber--) {
        i2cBitBang->writeSda((byte & (1 << bitNumber)) != 0);
        i2cBitBang->waitHalfClockCycle();
        i2cBitBang->writeScl(true);
        i2cBitBang->waitHalfClockCycle();
        i2cBitBang->writeScl(false);
    }

    // ACK
    i2cBitBang->writeSda(true);
    i2cBitBang->waitHalfClockCycle();
    i2cBitBang->writeScl(true);
    i2cBitBang->waitHalfClockCycle();
    const bool ack = i2cBitBang->readSda() == false;
    i2cBitBang->writeScl(false);
    i2cBitBang->writeSda(false);
    return ack;
}

/**
 * @brief Receives and returns byte.
 * @param i2cBitBang I2C bit-bang structure.
 * @return Received byte
 */
char I2CBitBangReceive(const I2cBitBang * const i2cBitBang) {
    i2cBitBang->writeSda(true);
    char byte = 0;
    int bitNumber;
    for (bitNumber = 7; bitNumber >= 0; bitNumber--) {
        i2cBitBang->waitHalfClockCycle();
        i2cBitBang->writeScl(true);
        i2cBitBang->waitHalfClockCycle();
        byte |= i2cBitBang->readSda() == true ? (1 << bitNumber) : 0;
        i2cBitBang->writeScl(false);
    }
    return byte;
}

/**
 * @brief Performs acknowledge.
 * @param i2cBitBang I2C bit-bang structure.
 */
void I2CBitBangAck(const I2cBitBang * const i2cBitBang) {
    i2cBitBang->writeSda(false);
    i2cBitBang->waitHalfClockCycle();
    i2cBitBang->writeScl(true);
    i2cBitBang->waitHalfClockCycle();
    i2cBitBang->writeScl(false);
}

/**
 * @brief Performs not acknowledge.
 * @param i2cBitBang I2C bit-bang structure.
 */
void I2CBitBangNack(const I2cBitBang * const i2cBitBang) {
    i2cBitBang->writeSda(true);
    i2cBitBang->waitHalfClockCycle();
    i2cBitBang->writeScl(true);
    i2cBitBang->waitHalfClockCycle();
    i2cBitBang->writeScl(false);
}

//------------------------------------------------------------------------------
// End of file
