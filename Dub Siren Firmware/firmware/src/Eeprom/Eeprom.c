/**
 * @file Eeprom.c
 * @author Seb Madgwick
 * @brief EEPROM library for Microchip 24AA32A.
 */

//------------------------------------------------------------------------------
// Includes

#include "Eeprom.h"
#include "stdbool.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief I2C address of EEPROM.  Assumes pins A0-2 are tied to ground.
 */
#define I2C_ADDRESS (0x50)

/**
 * @brief Page size of EEPROM.
 */
#define EEPROM_PAGE_SIZE (32)

//------------------------------------------------------------------------------
// Function prototypes

static void StartSequence(const I2cBitBang * const i2cBitBang, const unsigned int address);

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Read byte array from specified start address.  The start address and
 * number of bytes may be arbitrary.
 * @param address Start address of EEPROM memory.
 * @param destination Destination address of byte array.
 * @param numberOfBytes Number of bytes to read.
 */
void EepromRead(const I2cBitBang * const i2cBitBang, const unsigned int address, char *const destination, const size_t numberOfBytes) {
    StartSequence(i2cBitBang, address);
    I2CBitBangStop(i2cBitBang);
    I2CBitBangStart(i2cBitBang);
    I2CBitBangSend(i2cBitBang, I2C_READ_ADDRESS(I2C_ADDRESS));
    if (numberOfBytes > 0) {
        unsigned int index = 0;
        while (true) {
            destination[index] = I2CBitBangReceive(i2cBitBang);
            if (++index < numberOfBytes) {
                I2CBitBangAck(i2cBitBang);
            } else {
                I2CBitBangNack(i2cBitBang);
                break;
            }
        }
    }
    I2CBitBangStop(i2cBitBang);
}

/**
 * @brief Writes byte array to specified start address.   The start address and
 * number of bytes may be arbitrary.
 * @param address Start address of EEPROM memory.
 * @param source Source address of byte array.
 * @param numberOfBytes Number of bytes to written.
 */
void EepromWrite(const I2cBitBang * const i2cBitBang, unsigned int address, const char* source, const size_t numberOfBytes) {
    StartSequence(i2cBitBang, address);
    const unsigned int endAddress = address + numberOfBytes;
    int currentPageNumber = address / EEPROM_PAGE_SIZE;
    while (address < endAddress) {
        I2CBitBangSend(i2cBitBang, *source++);
        const int nextPageNumber = ++address / EEPROM_PAGE_SIZE;
        if (nextPageNumber != currentPageNumber) { // if crossing page boundary
            currentPageNumber = nextPageNumber;
            I2CBitBangStop(i2cBitBang);
            StartSequence(i2cBitBang, address);
        }
    }
    I2CBitBangStop(i2cBitBang);
}

/**
 * @brief Start sequence common to read and write functions.  Implements
 * 'acknowledge polling' to minimise delay while device is engaged in write
 * cycle.
 * @param address Start address of EEPROM memory.
 */
static void StartSequence(const I2cBitBang * const i2cBitBang, const unsigned int address) {
    bool ack;
    do {
        I2CBitBangStart(i2cBitBang);
        ack = I2CBitBangSend(i2cBitBang, I2C_WRITE_ADDRESS(I2C_ADDRESS));
    } while (ack == false); // wait for write cycle to complete
    I2CBitBangSend(i2cBitBang, address >> 8);
    I2CBitBangSend(i2cBitBang, address & 0xFF);
}

/**
 * @brief Erases the entire EEPROM.  All data bytes are set to 0xFF.
 */
void EepromEraseAll(const I2cBitBang * const i2cBitBang) {
    const char blankPage[EEPROM_PAGE_SIZE] = {[0 ... (EEPROM_PAGE_SIZE - 1)] = 0xFF};
    unsigned int index;
    for (index = 0; index < (EEPROM_SIZE / EEPROM_PAGE_SIZE); index++) {
        EepromWrite(i2cBitBang, (index * EEPROM_PAGE_SIZE), (char*) blankPage, sizeof (blankPage));
    }
}

//------------------------------------------------------------------------------
// End of file
