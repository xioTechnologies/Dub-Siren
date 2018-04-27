/**
 * @file I2CSlaveAddress.h
 * @author Seb Madgwick
 * @brief Functions for appending the R/W bit on a 7-bit I2C slave address.
 */

#ifndef I2C_SLAVE_ADDRESS_H
#define I2C_SLAVE_ADDRESS_H

//------------------------------------------------------------------------------
// Function prototypes

char I2CSlaveAddressRead(const char slaveAddress);
char I2CSlaveAddressWrite(const char slaveAddress);

#endif

//------------------------------------------------------------------------------
// End of file
