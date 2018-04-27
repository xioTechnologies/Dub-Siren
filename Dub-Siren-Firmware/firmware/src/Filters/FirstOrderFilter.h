/**
 * @file FirstOrderFilter.h
 * @author Seb Madgwick
 * @brief First-order low-pass or high-pass filter.
 */

#ifndef FIRST_ORDER_FILTER_H
#define FIRST_ORDER_FILTER_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief First-order filter structure.  Structure members are used internally
 * and should not be used by the user application.
 */
typedef struct {
    bool isHighPass; // else is low-pass
    float previousInput;
    float previousOutput;
    float coefficient;
} FirstOrderFilter;

//------------------------------------------------------------------------------
// Function prototypes

void FirstOrderFilterSetCornerFrequency(FirstOrderFilter * const firstOrderFilter, const float cornerFrequency, const float sampleFrequency, const bool isHighPass);
float FirstOrderFilterUpdate(FirstOrderFilter * const FirstOrderFilter, const float input);

#endif

//------------------------------------------------------------------------------
// End of file
