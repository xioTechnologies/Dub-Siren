/**
 * @file DebouncedButton.c
 * @author Seb Madgwick
 * @brief Provides functions for implementing debounced buttons.
 */

//------------------------------------------------------------------------------
// Includes

#include "DebouncedButton.h"
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @breif Debounce holdoff period in timer ticks.
 */
#define HOLDOFF_PERIOD ((uint64_t) (TIMER_TICKS_PER_SECOND / 100))

//------------------------------------------------------------------------------
// Function prototypes

static inline __attribute__((always_inline)) void Update(DebouncedButton * const debouncedButton);

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises an OSC bundle structure with a specified OSC time tag.
 * @param debouncedButton Debounced button structure to be initialised.
 * @param port Port register, e.g. PORTB.
 * @param portBit Port bit number, e.g. 7 for RB7.
 */
void DebouncedButtonInitialise(DebouncedButton * const debouncedButton, volatile unsigned int* const port, const unsigned int portBit) {
    debouncedButton->port = port;
    debouncedButton->portBit = portBit;
    debouncedButton->ticks = 0;
    debouncedButton->wasPressed = false;
    debouncedButton->isHeld = false;
}

/**
 * @brief Returns true if the button was pressed since the previous call of this
 * function.
 * @param debouncedButton Debounced button structure being queried.
 * @return True if the button was pressed since the previous call of this
 * function.
 */
bool DebouncedButtonWasPressed(DebouncedButton * const debouncedButton) {
    Update(debouncedButton);
    const bool returnValue = debouncedButton->wasPressed;
    debouncedButton->wasPressed = false;
    return returnValue;
}

/**
 * @brief Returns true if the button is currently being held.
 * @param debouncedButton Debounced button structure being queried.
 * @return True if the button is currently being held.
 */
bool DebouncedButtonIsHeld(DebouncedButton * const debouncedButton) {
    Update(debouncedButton);
    return debouncedButton->isHeld;
}

/**
 * @brief Reads the button pin state and updates the debounced button structure.
 * @param debouncedButton Debounced button structure to be updated.
 */
static inline __attribute__((always_inline)) void Update(DebouncedButton * const debouncedButton) {
    const uint64_t currentTicks = TimerGetTicks64();
    if ((*debouncedButton->port & (1 << debouncedButton->portBit)) != 0) {
        debouncedButton->ticks = currentTicks;
        if (debouncedButton->isHeld == false) {
            debouncedButton->wasPressed = true;
        }
        debouncedButton->isHeld = true;
    } else {
        if (currentTicks >= (debouncedButton->ticks + HOLDOFF_PERIOD)) {
            debouncedButton->isHeld = false;
        }
    }
}

//------------------------------------------------------------------------------
// End of file
