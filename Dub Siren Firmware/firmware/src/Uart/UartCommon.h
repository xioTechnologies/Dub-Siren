/**
 * @file UartCommon.h
 * @author Seb Madgwick
 * @brief UART library for PIC32MZ.
 */

#ifndef UART_COMMON_H
#define UART_COMMON_H

//------------------------------------------------------------------------------
// Includes

#include "stdbool.h"
#include <stdint.h>
#include "system_config.h" // SYS_CLK_BUS_PERIPHERAL_2

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief UART parity and data enumeration.  Values equal to PDSEL bits of
 * UxMODE register.
 */
typedef enum {
    EightBitNoParity,
    EightBitEvenParity,
    EightBitOddParity,
    NineBitNoParity,
} UartParityAndData;

/**
 * @brief UART stop bits enumeration.  Values equal to STSEL bits of UxMODE
 * register.
 */
typedef enum {
    OneStopBit,
    TwoStopBits,
} UartStopBits;

/**
 * @brief UART settings structure.
 */
typedef struct {
    uint32_t baudRate;
    bool ctsRtsEnabled;
    UartParityAndData parityAndData;
    UartStopBits stopBits;
    bool invertDataLines;
} UartSettings;

/**
 * @brief Default UART settings.  Can be used to initialised a UartSettings
 * structure.
 */
#define DEFAULT_UART_SETTINGS ((UartSettings) { \
    .baudRate = 115200, \
    .ctsRtsEnabled = false, \
    .parityAndData = EightBitNoParity, \
    .stopBits = OneStopBit, \
    .invertDataLines = false, })

/**
 * @brief Calculates ideal UXBRG value as a float for a specified baud rate and
 * defined peripheral clock (PBCLK).  Assumes BRGH = 1.
 * @see Page 12 of "Section 21. UART".
 */
#define CALCULATE_UXBRG_FLOAT(baudRate) ((float) SYS_CLK_BUS_PERIPHERAL_2 / (4.0f * (float) baudRate) - 1.0f)

/**
 * @brief Calculates UXBRG value as the rounded result of CALCULATE_UXBRG_FLOAT
 * for a specified baud rate and defined peripheral clock (PBCLK).
 */
#define CALCULATE_UXBRG(baudRate) ((uint32_t) (CALCULATE_UXBRG_FLOAT(baudRate) + 0.5f))

/**
 * @brief Calculates baud rate percentage error for a specified baud rate and
 * defined peripheral clock (PBCLK).
 */
#define CALCULATE_UART_BAUD_ERROR(baudRate) (100.0f * ((float) CALCULATE_UXBRG(baudRate) / CALCULATE_UXBRG_FLOAT(baudRate)) - 100.0f)

#endif

//------------------------------------------------------------------------------
// End of file
