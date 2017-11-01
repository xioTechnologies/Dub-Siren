/**
 * @file Potentiometers.h
 * @author Seb Madgwick
 * @brief Provides high-resolution measurement of potentiometers values through
 * oversampling.
 */

#ifndef POTENTIOMETERS_H
#define POTENTIOMETERS_H

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Number of potentiometers.
 */
#define NUMBER_OF_POTENTIOMETERS (9)

/**
 * @brief Potentiometer index names.
 */
typedef enum {
    PotentiometerIndexLfoWaveform,
    PotentiometerIndexLfoFrequency,
    PotentiometerIndexLfoShape,
    PotentiometerIndexLfoAmplitude,
    PotentiometerIndexVcoWaveform,
    PotentiometerIndexVcoFrequency,
    PotentiometerIndexDelayTime,
    PotentiometerIndexDelayFeedback,
    PotentiometerIndexDelayFilter,
} PotentiometerIndex;

//------------------------------------------------------------------------------
// Function prototypes

void PotentiometersInitialise();
void PotentiometersGetValues(float potentiometers[NUMBER_OF_POTENTIOMETERS]);

#endif

//------------------------------------------------------------------------------
// End of file
