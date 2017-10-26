/**
 * @file CascadeFilter.c
 * @author Seb Madgwick
 * @brief Cascaded first-order low-pass or high-pass filter.
 */

//------------------------------------------------------------------------------
// Includes

#include "CascadeFilter.h"
#include "MathHelpers.h"

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Sets the filter corner frequency of each filter in the cascade.
 * @param CascadeFilter Cascade filter structure.
 * @param cornerFrequency Corner frequency in Hz.
 * @param sampleFrequency Sample frequency in Hz.
 * @param isHighPass True if high-pass filter, False if low-pass filter.
 * @param numberOfFilters Number of filters in the cascade.
 */
void CascadeFilterSetCornerFrequency(CascadeFilter * const cascadeFilter, const float cornerFrequency, const float sampleFrequency, const bool isHighPass, const unsigned int numberOfFilters) {
    cascadeFilter->numberOfFilters = CLAMP(numberOfFilters, 1, MAXIMUM_NUMBER_OF_CASCADED_FILTERS);
    FirstOrderFilterSetCornerFrequency(&cascadeFilter->firstOrderFilter[0], cornerFrequency, sampleFrequency, isHighPass);
    unsigned int index;
    for (index = 1; index < cascadeFilter->numberOfFilters; index++) {
        cascadeFilter->firstOrderFilter[index].isHighPass = isHighPass;
        cascadeFilter->firstOrderFilter[index].coefficient = cascadeFilter->firstOrderFilter[0].coefficient; // avoid repeated calculations
    }
}

/**
 * @brief Updates each filter in the cascade with the new input sample and
 * returns the output.
 * @param CascadeFilter Cascade filter structure.
 * @param input Input sample.
 * @return Cascaded filter output.
 */
float CascadeFilterUpdate(CascadeFilter * const cascadeFilter, const float input) {
    float output = input;
    unsigned int index;
    for (index = 0; index < cascadeFilter->numberOfFilters; index++) {
        output = FirstOrderFilterUpdate(&cascadeFilter->firstOrderFilter[index], output);
    }
    return output;
}

//------------------------------------------------------------------------------
// End of file
