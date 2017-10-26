/**
 * @file Timer.c
 * @author Seb Madgwick
 * @brief Timer library for PIC32MZ.
 */

//------------------------------------------------------------------------------
// Includes

#include "system_config.h" // SYS_CLK_BUS_PERIPHERAL_3
#include "system/int/sys_int.h"
#include "Timer.h"
#include <xc.h>

//------------------------------------------------------------------------------
// Definitions

typedef union {
    uint64_t value;

    struct {
        uint32_t dword0; // least-significant dword
        uint32_t dword1; // most-significant dword
    };
} Uint64Union;

//------------------------------------------------------------------------------
// Variable declarations

const uint32_t timerTicksPerSecond = SYS_CLK_BUS_PERIPHERAL_3;
static volatile uint32_t timerOverflowCounter;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the timer.
 */
void TimerInitialise() {
    T4CONbits.T32 = 1;
    T4CONbits.ON = 1; // start timer
    SYS_INT_VectorPrioritySet(_TIMER_5_VECTOR, INT_PRIORITY_LEVEL7); // set interrupt priority
    SYS_INT_SourceStatusClear(INT_SOURCE_TIMER_5); // clear interrupt flag
    SYS_INT_SourceEnable(INT_SOURCE_TIMER_5); // enable interrupt
}

/**
 * @brief Disables the timer.
 */
void TimerDisable() {
    T4CONbits.ON = 0; // stop timer
    SYS_INT_SourceDisable(INT_SOURCE_TIMER_5); // disable interrupt
}

/**
 * @brief Gets 32-bit timer value.
 * @return 32-bit timer value.
 */
uint32_t TimerGetTicks32() {
    return TMR4; // read 32-bit timer value
}

/**
 * @brief Gets 64-bit timer value.
 * @return 64-bit timer value.
 */
uint64_t TimerGetTicks64() {
    Uint64Union ticks64;
    do {
        ticks64.dword1 = timerOverflowCounter; // must read this value first
        ticks64.dword0 = TMR4; // read 32-bit timer value
    } while (ticks64.dword1 != timerOverflowCounter); // avoid seconds overflow hazard
    return ticks64.value;
}

/**
 * @brief Blocking delay in milliseconds.
 * @param milliseconds Delay in milliseconds.
 */
void TimerDelay(const uint32_t milliseconds) {
    uint64_t currentTicks = TimerGetTicks64();
    const uint64_t endTicks = currentTicks + ((uint64_t) milliseconds * (uint64_t) (timerTicksPerSecond / 1000));
    do {
        currentTicks = TimerGetTicks64();
    } while (currentTicks < endTicks);
}

/**
 * @brief Blocking delay in microseconds.  This function should not be used for
 * delays approaching 2^32 microseconds.
 * @param microseconds Delay in microseconds.
 */
void TimerDelayMicroseconds(const uint32_t microseconds) {
    const uint32_t startTicks = TimerGetTicks32();
    uint32_t currentTicks;
    do {
        currentTicks = TimerGetTicks32();
    } while ((currentTicks - startTicks) < (microseconds * (timerTicksPerSecond / 1000000)));
}

/**
 * @brief Timer overflow interrupt to increment overflow counter.
 */
void __ISR(_TIMER_5_VECTOR) Timer5Interrupt() {
    timerOverflowCounter++;
    SYS_INT_SourceStatusClear(INT_SOURCE_TIMER_5); // clear interrupt flag
}

//------------------------------------------------------------------------------
// End of file
