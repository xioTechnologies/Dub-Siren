/**
 * @file Uart1.h
 * @author Seb Madgwick
 * @brief UART library for PIC32MZ.
 */

#ifndef UART1_H
#define UART1_H

//------------------------------------------------------------------------------
// Includes

#include "stdbool.h"
#include <stddef.h>
#include "UartCommon.h"

//------------------------------------------------------------------------------
// Definitions

#define UART1_BUFFER_SIZE (1024) // must be a 2^n number, e.g. 256, 512, 1024, ...

//------------------------------------------------------------------------------
// Function prototypes

void Uart1Initialise(const UartSettings * const uartSettings);
void Uart1Disable();
size_t Uart1IsGetReady();
char Uart1GetChar();
size_t Uart1IsPutReady();
void Uart1PutChar(const char byte);
void Uart1PutCharArray(const char* const source, const size_t numberOfBytes);
void Uart1PutString(const char* string);
void Uart1ClearRxBuffer();
void Uart1ClearTxBuffer();
bool Uart1GetRxBufferOverrunFlag();
void Uart1ClearRxBufferOverrunFlag();
bool Uart1TxIsIdle();

#endif

//------------------------------------------------------------------------------
// End of file
