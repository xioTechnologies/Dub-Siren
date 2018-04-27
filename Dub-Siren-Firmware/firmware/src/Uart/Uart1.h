/**
 * @file Uart1.h
 * @author Seb Madgwick
 * @brief UART library for PIC32.
 */

#ifndef UART1_H
#define UART1_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>
#include "UartCommon.h"

//------------------------------------------------------------------------------
// Function prototypes

void Uart1Initialise(const UartSettings * const uartSettings);
void Uart1Disable();
size_t Uart1IsReadReady();
char Uart1Read();
size_t Uart1IsWriteReady();
void Uart1WriteChar(const char byte);
void Uart1WriteCharIfReady(const char byte);
void Uart1WriteCharArray(const char* const source, const size_t numberOfBytes);
void Uart1WriteCharArrayIfReady(const char* const source, const size_t numberOfBytes);
void Uart1WriteString(const char* string);
void Uart1WriteStringIfReady(const char* string);
void Uart1ClearReadBuffer();
void Uart1ClearWriteBuffer();
bool Uart1GetReadBufferOverrunFlag();
void Uart1ClearReadBufferOverrunFlag();
bool Uart1IsTransmitionComplete();

#endif

//------------------------------------------------------------------------------
// End of file
