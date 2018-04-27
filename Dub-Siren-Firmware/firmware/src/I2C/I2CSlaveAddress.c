/**
 * @file I2CSlaveAddress.c
 * @author Seb Madgwick
 * @brief Functions for appending the R/W bit on a 7-bit I2C slave address.
 */

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Appends the R/W bit on a 7-bit I2C slave address to indicate a write.
 * @param slaveAddress 7-bit slave address.
 * @return 7-bit slave address with appended R/W bit.
 */
char I2CSlaveAddressRead(const char slaveAddress) {
    return (slaveAddress << 1) | 1;
}

/**
 * @brief Appends the R/W bit on a 7-bit I2C slave address to indicate a read.
 * @param slaveAddress 7-bit slave address.
 * @return 7-bit slave address with appended R/W bit.
 */
char I2CSlaveAddressWrite(const char slaveAddress) {
    return (slaveAddress << 1) | 0;
}

//------------------------------------------------------------------------------
// End of file
