/**
 * @file DebouncedButton.h
 * @author Seb Madgwick
 * @brief Provides functions for implementing debounced buttons.
 */

#ifndef DEBOUNCED_BUTTON_H
#define DEBOUNCED_BUTTON_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Debounced button structure.  Structure members used internally and
 * should not be used by the user application.
 */
typedef struct {
    volatile unsigned int* port;
    unsigned int portBit;
    uint64_t ticks;
    bool wasPressed;
    bool isHeld;
} DebouncedButton;

//------------------------------------------------------------------------------
// Function prototypes

void DebouncedButtonInitialise(DebouncedButton * const debouncedButton, volatile unsigned int* const port, const unsigned int portBit);
bool DebouncedButtonWasPressed(DebouncedButton * const debouncedButton);
bool DebouncedButtonIsHeld(DebouncedButton * const debouncedButton);

#endif

//------------------------------------------------------------------------------
// End of file
