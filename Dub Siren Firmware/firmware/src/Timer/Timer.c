/**
 * @file Timer.c
 * @author Seb Madgwick
 * @brief Timer library for PIC32MZ.
 */

//------------------------------------------------------------------------------
// Includes

#include <stdint.h> // UINT32_MAX
#include "system/int/sys_int.h"
#include "Timer.h"
#include <xc.h>

//------------------------------------------------------------------------------
// Variable declarations

static volatile Ticks64 timerOverflowCounter;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises module.  This function should be called once on system
 * start up.
 */
void TimerInitialise() {
    T4CONbits.T32 = 1;
    T4CONbits.ON = 1; // start timer
    SYS_INT_VectorPrioritySet(_TIMER_5_VECTOR, INT_PRIORITY_LEVEL7); // set interrupt priority
    SYS_INT_SourceStatusClear(INT_SOURCE_TIMER_5); // clear interrupt flag
    SYS_INT_SourceEnable(INT_SOURCE_TIMER_5); // enable interrupt
}

/**
 * @brief Gets 32-bit timer value.  This function call is quicker than
 * TimerGetTicks64 and so may be preferable if the 32-bit ticks overflow period
 * is not too short.
 * @return 32-bit timer value.
 */
Ticks32 TimerGetTicks32() {
    return TMR4; // read 32-bit timer value
}

/**
 * @brief Gets 64-bit timer value.
 * @return 64-bit timer value.
 */
Ticks64 TimerGetTicks64() {
    Ticks64 sampledTimerOverflowCounter;
    Ticks64 ticks64;
    do {
        sampledTimerOverflowCounter = timerOverflowCounter;
        ticks64.dword1 = 0;
        ticks64.dword0 = TMR4; // read 32-bit timer value
    } while (sampledTimerOverflowCounter.dword0 != timerOverflowCounter.dword0); // avoid seconds overflow hazard
    ticks64.value += sampledTimerOverflowCounter.value;
    return ticks64;
}

/**
 * @brief Blocking delay in milliseconds.
 * @param milliseconds Delay in milliseconds.
 */
void TimerDelay(uint32_t milliseconds) {
    const Ticks64 previousTicks = TimerGetTicks64();
    Ticks64 currentTicks;
    do {
        currentTicks = TimerGetTicks64();
    } while ((currentTicks.value - previousTicks.value) < (milliseconds * (TIMER_TICKS_PER_SECOND / 1000)));
}

/**
 * @brief Blocking delay in microseconds.
 * @param milliseconds Delay in microseconds.
 */
void TimerDelayMicroseconds(uint32_t microseconds) {
    const Ticks64 previousTicks = TimerGetTicks64();
    Ticks64 currentTicks;
    do {
        currentTicks = TimerGetTicks64();
    } while ((currentTicks.value - previousTicks.value) < (microseconds * (TIMER_TICKS_PER_SECOND / 1000000)));
}

/**
 * @brief Timer overflow interrupt to increment overflow counter.
 */
void __attribute__((interrupt(), vector(_TIMER_5_VECTOR))) Timer5Interrupt() {
    timerOverflowCounter.value += (uint64_t) UINT32_MAX;
    SYS_INT_SourceStatusClear(INT_SOURCE_TIMER_5); // clear interrupt flag
}

//------------------------------------------------------------------------------
// End of file
