/**
 * @file Timer.h
 * @author Seb Madgwick
 * @brief Timer library for PIC32MZ.
 */

#ifndef TIMER_H
#define TIMER_H

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const uint32_t timerTicksPerSecond;

//------------------------------------------------------------------------------
// Function prototypes

void TimerInitialise();
void TimerDisable();
uint32_t TimerGetTicks32();
uint64_t TimerGetTicks64();
void TimerDelay(const uint32_t milliseconds);
void TimerDelayMicroseconds(const uint32_t microseconds);

#endif

//------------------------------------------------------------------------------
// End of file
