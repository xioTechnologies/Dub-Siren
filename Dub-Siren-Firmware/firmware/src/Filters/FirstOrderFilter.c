/**
 * @file FirstOrderFilter.c
 * @author Seb Madgwick
 * @brief First-order low-pass or high-pass filter.
 *
 * https://en.wikipedia.org/wiki/Low-pass_filter
 * https://en.wikipedia.org/wiki/High-pass_filter
 */

//------------------------------------------------------------------------------
// Includes

#include "FirstOrderFilter.h"
#include "math.h" // M_PI

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Sets the filter corner frequency.
 * @param FirstOrderFilter First-order filter structure.
 * @param cornerFrequency Corner frequency in Hz.
 * @param sampleFrequency Sample frequency in Hz.
 * @param isHighPass True if high-pass filter, false if low-pass filter.
 */
void FirstOrderFilterSetCornerFrequency(FirstOrderFilter * const firstOrderFilter, const float cornerFrequency, const float sampleFrequency, const bool isHighPass) {
    firstOrderFilter->isHighPass = isHighPass;
    if (firstOrderFilter->isHighPass == true) {
        firstOrderFilter->coefficient = 1.0f / ((2.0f * M_PI * cornerFrequency * (1.0f / sampleFrequency)) + 1.0f);
    } else {
        firstOrderFilter->coefficient = ((1.0f / sampleFrequency) / ((1.0f / (2.0f * M_PI * cornerFrequency)) + (1.0f / sampleFrequency)));
    }
}

/**
 * @brief Updates the low-pass filter with the new input sample and returns the
 * output.
 * @param FirstOrderFilter First-order filter structure.
 * @param input Input sample.
 * @return First-order filter output.
 */
float FirstOrderFilterUpdate(FirstOrderFilter * const firstOrderFilter, const float input) {
    float output;
    if (firstOrderFilter->isHighPass == true) {
        output = firstOrderFilter->coefficient * (firstOrderFilter->previousOutput + input - firstOrderFilter->previousInput);
    } else {
        output = firstOrderFilter->previousOutput + ((input - firstOrderFilter->previousOutput) * firstOrderFilter->coefficient);
    }
    firstOrderFilter->previousInput = input;
    firstOrderFilter->previousOutput = output;
    return output;
}

//------------------------------------------------------------------------------
// End of file
