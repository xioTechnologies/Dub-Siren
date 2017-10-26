/**
 * @file UartCommon.h
 * @author Seb Madgwick
 * @brief UART library for PIC32MZ.
 */

//------------------------------------------------------------------------------
// Includes

#include "system_config.h"
#include "UartCommon.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Value used for baud rate calculation.  See page 12 of "PIC32 Family
 * Reference Manual, Section 21. UART".
 */
#define FPB (SYS_CLK_BUS_PERIPHERAL_2)

//------------------------------------------------------------------------------
// Variables

const UartSettings defaultUartSettings = {
    .baudRate = 115200,
    .ctsRtsEnabled = false,
    .parityAndData = EightBitNoParity,
    .stopBits = OneStopBit,
    .invertDataLines = false
};

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Calculates a UXBRG value for a specified baud rate.
 */
uint32_t UartCalculateUxbrg(const uint32_t baudRate) {
    const float idealUxbrg = (FPB / (4.0f * (float) baudRate)) - 1.0f;
    return (uint32_t) (idealUxbrg + 0.5f);
}

/**
 * @brief Calculates baud rate percentage error for a specified baud rate.
 */
float UartCalculateBaudRateError(const uint32_t baudRate) {
    const float uxbrg = UartCalculateUxbrg(baudRate);
    const float actualBaudRate = ((float) FPB / (4.0f * (uxbrg + 1.0f)));
    return 100.0f * ((actualBaudRate - baudRate) / baudRate);
}

//------------------------------------------------------------------------------
// End of file
