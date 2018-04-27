/**
 * @file UartCommon.h
 * @author Seb Madgwick
 * @brief UART library for PIC32.
 */

#ifndef UART_COMMON_H
#define UART_COMMON_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stdint.h>

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

//------------------------------------------------------------------------------
// Variable declarations

extern const UartSettings defaultUartSettings;

//------------------------------------------------------------------------------
// Function prototypes

unsigned int UartCalculateUxbrg(const uint32_t baudRate);
float UartCalculateBaudRateError(const uint32_t baudRate);

#endif

//------------------------------------------------------------------------------
// End of file
