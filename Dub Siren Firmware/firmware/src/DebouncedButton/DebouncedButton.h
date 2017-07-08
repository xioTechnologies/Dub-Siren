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
 * @brief Debounced button structure.  Must be initialised using
 * DebouncedButtonInitialise.
 */
typedef struct {
    volatile unsigned int* port;
    int portBit;
    Ticks64 ticks;
    bool wasPressed;
    bool isHeld;
} DebouncedButton;

//------------------------------------------------------------------------------
// Function prototypes

void DebouncedButtonInitialise(DebouncedButton * const debouncedButton, volatile unsigned int* const port, const int portBit);
bool DebouncedButtonWasPressed(DebouncedButton * const debouncedButton);
bool DebouncedButtonIsHeld(DebouncedButton * const debouncedButton);

#endif

//------------------------------------------------------------------------------
// End of file