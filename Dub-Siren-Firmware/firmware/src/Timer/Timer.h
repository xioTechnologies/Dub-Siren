/**
 * @file Timer.h
 * @author Seb Madgwick
 * @brief Timer library for PIC32.
 */

#ifndef TIMER_H
#define TIMER_H

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>
#include "system_config.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @bief Number of timer ticks per second.
 */
#define TIMER_TICKS_PER_SECOND ((uint32_t) SYS_CLK_BUS_PERIPHERAL_3)

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
