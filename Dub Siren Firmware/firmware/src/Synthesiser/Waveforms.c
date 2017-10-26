/**
 * @file Waveforms.c
 * @author Seb Madgwick
 * @brief LFO and VCO waveforms.
 */

//------------------------------------------------------------------------------
// Includes

#include <math.h> // ceilf, floorf
#include "MathHelpers.h"
#include <stdbool.h>
#include <stdint.h>
#include "Waveforms.h"
#include "WaveformTables.h"

//------------------------------------------------------------------------------
// Function prototypes

static inline __attribute__((always_inline)) float InterpolateWaveformTable(const float* const waveformTable, const float normalisedPeriod);

//------------------------------------------------------------------------------
// Functions

/**
 * @breif Wraps-around normalised period to limit range to 0.0 to 1.0.
 * @param normalisedPeriod 0.0 to 1.0 corresponding to 0 to 2 pi.
 * @return Normalised period limited range to 0.0 to 1.0.
 */
float WaveformsLimitNormalisedPeriod(float normalisedPeriod) {
    while (normalisedPeriod < 0.0f) {
        normalisedPeriod += 1.0f;
    }
    while (normalisedPeriod > 1.0f) {
        normalisedPeriod -= 1.0f;
    }
    return normalisedPeriod;
}

/**
 * @breif Returns the interpolated waveform amplitude for a normalised period.
 * @param table Waveform table.
 * @param normalisedPeriod 0.0 to 1.0 corresponding to 0 to 2 pi.
 * @return Interpolated waveform amplitude.
 */
static inline __attribute__((always_inline)) float InterpolateWaveformTable(const float* const waveformTable, const float normalisedPeriod) {
    const float index = (normalisedPeriod * (float) (WAVEFORM_TABLE_LENGTH - 1));
    const float indexFloor = floor(index);
    const float indexCeil = ceil(index);
    if (indexFloor == indexCeil) {
        return waveformTable[(unsigned int) index];
    }
    const float amplitudeFloor = waveformTable[(unsigned int) indexFloor];
    const float amplitudeCeil = waveformTable[(unsigned int) indexCeil];
    return MAP(index, indexFloor, indexCeil, amplitudeFloor, amplitudeCeil);
}

/**
 * @breif Returns sine wave amplitude for a normalised period.
 * @param normalisedPeriod 0.0 to 1.0 corresponding to 0 to 2 pi.
 * @return Sine wave amplitude.
 */
float WaveformsSine(const float normalisedPeriod) {
    return InterpolateWaveformTable(sineTable, normalisedPeriod);
}

/**
 * @breif Returns bandwidth-limited triangle wave amplitude for a normalised
 * period.
 * @param normalisedPeriod 0.0 to 1.0 corresponding to 0 to 2 pi.
 * @param frequency Frequency of triangle wave.
 * @return Triangle wave amplitude.
 */
float WaveformsBandwidthLimitedTriangle(const float normalisedPeriod, const float frequency) {

    // Use unlimited-bandwidth waveform for low frequencies
    if (frequency < MINIMUM_FREQUENCY) {
        return normalisedPeriod < 0.5f ? (-4.0f * (normalisedPeriod - 0.25f)) : (4.0f * (normalisedPeriod - 0.75f));
    }

    // Use sine wave for high frequencies
    if (frequency >= MAXIMUM_FREQUENCY) {
        return InterpolateWaveformTable(sineTable, normalisedPeriod);
    }

    // Else use bandwidth-limited waveform
    const unsigned int harmonic = (unsigned int) ((float) MAXIMUM_FREQUENCY / frequency); // intentionally rounded down
    const unsigned int wavefromIndex = CLAMP(((harmonic - 1) / 2) - 1, 0, NUMBER_OF_TRIANGLE_WAVEFORMS - 1);
    return InterpolateWaveformTable(triangleTable[wavefromIndex], normalisedPeriod);
}

/**
 * @breif Returns bandwidth-limited sawtooth wave amplitude for a normalised
 * period.
 * @param normalisedPeriod 0.0 to 1.0 corresponding to 0 to 2 pi.
 * @param frequency Frequency of sawtooth wave.
 * @return Sawtooth wave amplitude.
 */
float WaveformsBandwidthLimitedSawtooth(const float normalisedPeriod, const float frequency) {

    // Use unlimited-bandwidth waveform for low frequencies
    if (frequency < MINIMUM_FREQUENCY) {
        return (-2.0f * normalisedPeriod) + 1.0f;
    }

    // Use sine wave for high frequencies
    if (frequency >= MAXIMUM_FREQUENCY) {
        return InterpolateWaveformTable(sineTable, normalisedPeriod);
    }

    // Else use bandwidth-limited waveform
    const unsigned int harmonic = (unsigned int) ((float) MAXIMUM_FREQUENCY / frequency); // intentionally rounded down
    const unsigned int wavefromIndex = CLAMP(harmonic - 2, 0, NUMBER_OF_SAWTOOTH_WAVEFORMS - 1);
    return InterpolateWaveformTable(sawtoothTable[wavefromIndex], normalisedPeriod);
}

/**
 * @breif Returns bandwidth-limited square wave amplitude for a normalised
 * period.
 * @param normalisedPeriod 0.0 to 1.0 corresponding to 0 to 2 pi.
 * @param frequency Frequency of square wave.
 * @return Square wave amplitude.
 */
float WaveformsBandwidthLimitedSquare(const float normalisedPeriod, const float frequency) {

    // Use unlimited-bandwidth waveform for low frequencies
    if (frequency < MINIMUM_FREQUENCY) {
        return normalisedPeriod < 0.5f ? 1.0f : -1.0f;
    }

    // Use sine wave for high frequencies
    if (frequency >= MAXIMUM_FREQUENCY) {
        return InterpolateWaveformTable(sineTable, normalisedPeriod);
    }

    // Else use bandwidth-limited waveform
    const unsigned int harmonic = (unsigned int) ((float) MAXIMUM_FREQUENCY / frequency); // intentionally rounded down
    const unsigned int wavefromIndex = CLAMP(((harmonic - 1) / 2) - 1, 0, NUMBER_OF_SQUARE_WAVEFORMS - 1);
    return InterpolateWaveformTable(squareTable[wavefromIndex], normalisedPeriod);
}

/**
 * @breif Returns bandwidth-limited pulse wave amplitude for a normalised
 * period.
 * @param normalisedPeriod 0.0 to 1.0 corresponding to 0 to 2 pi.
 * @param frequency Frequency of pulse wave.
 * @return Pulse wave amplitude.
 */
float WaveformsBandwidthLimitedPulse(const float normalisedPeriod, const float frequency) {

    // Use unlimited-bandwidth waveform for low frequencies
    if (frequency < MINIMUM_FREQUENCY) {
        if (normalisedPeriod < 0.05f) {
            return 1.0f;
        }
        if (normalisedPeriod > 0.95f) {
            return 1.0f;
        } else {
            return -1.0f;
        }
    }

    // Use sine wave for high frequencies
    if (frequency >= MAXIMUM_FREQUENCY) {
        return InterpolateWaveformTable(sineTable, normalisedPeriod);
    }

    // Else use bandwidth-limited waveform
    const unsigned int harmonic = (unsigned int) ((float) MAXIMUM_FREQUENCY / frequency); // intentionally rounded down
    const unsigned int wavefromIndex = CLAMP(harmonic - 2, 0, NUMBER_OF_PULSE_WAVEFORMS - 1);
    return InterpolateWaveformTable(pulseTable[wavefromIndex], normalisedPeriod);
}

/**
 * @breif Returns one-bit noise amplitude of a specified frequency.  Random bit
 * generated using a linear-feedback shift register.
 * @see https://en.wikipedia.org/wiki/Linear-feedback_shift_register
 * @param frequency Frequency of one-bit noise.
 * @param sampleFrequency Rate at which this function is being called.
 * @return One-bit noise amplitude.
 */
float WaveformsOneBitNoise(const float frequency, const float sampleFrequency) {
    static float returnValue = 1.0f;
    const unsigned int samplesPerUpdate = (unsigned int) (sampleFrequency / frequency);
    static int sampleCounter = 0;
    if (sampleCounter++ >= samplesPerUpdate) {
        sampleCounter = 0;

        // Update LFSR
        static uint16_t lfsr = 0xACE1;
        bool lsb = (lfsr & 1) != 0;
        lfsr >>= 1;
        if (lsb == true) {
            lfsr ^= 0xB400;
        }

        // Convert bit into amplitude
        if (lsb == true) {
            returnValue = 1.0f;
        } else {
            returnValue = -1.0f;
        }
    }
    return returnValue;
}

/**
 * @breif Returns asymmetric sine wave amplitude for a normalised period.
 * @param normalisedPeriod 0.0 to 1.0 corresponding to 0 to 2 pi.
 * @param shape 0.0 to 1.0 adjusts adjusts symmetry.  A value of 0.5 results in
 * a symmetric sine wave.
 * @return Asymmetric sine wave amplitude.
 */
float WaveformsAsymmetricSine(const float normalisedPeriod, const float shape) {
    float skewedNormalisedPeriod;
    if (normalisedPeriod < shape) {
        skewedNormalisedPeriod = MAP(normalisedPeriod, 0.0f, shape, 0.0f, 0.5f);
    } else {
        skewedNormalisedPeriod = MAP(normalisedPeriod, shape, 1.0f, 0.5f, 01.0f);
    }
    return InterpolateWaveformTable(sineTable, WaveformsLimitNormalisedPeriod(skewedNormalisedPeriod - 0.25f));
}

/**
 * @breif Returns triangle wave amplitude for a normalised period.
 * @param normalisedPeriod 0.0 to 1.0 corresponding to 0 to 2 pi.
 * @param shape 0.0 to 1.0 skews the triangle wave between a negative sawtooth
 * and positive sawtooth.  A value of 0.5 results in a triangle wave.
 * @return Triangle wave amplitude.
 */
float WaveformsTriangle(const float normalisedPeriod, const float shape) {
    if (normalisedPeriod < shape) {
        return MAP(normalisedPeriod, 0.0f, shape, -1.0f, 1.0f);
    } else {
        return MAP(normalisedPeriod, shape, 1.0f, 1.0f, -1.0f);
    }
}

/**
 * @breif Returns sawtooth wave amplitude for a normalised period.
 * @param normalisedPeriod 0.0 to 1.0 corresponding to 0 to 2 pi.
 * @param shape 0.0 to 1.0 adjusts adjusts the gradient from linear to
 * exponential.
 * @return Sawtooth wave amplitude.
 */
float WaveformsSawtooth(const float normalisedPeriod, const float shape) {
    const float normalisedWaveform = CLAMP((1.0f + shape * shape * 10.0f) * normalisedPeriod, 0.0f, 1.0f);
    return 2.0f * (normalisedWaveform - 0.5f);
}

/**
 * @breif Returns square wave amplitude for a normalised period.
 * @param normalisedPeriod 0.0 to 1.0 corresponding to 0 to 2 pi.
 * @param shape 0.0 to 1.0 adjusts duty cycle from 0% to 100%.  A value of 0.5
 * results in a square wave.
 * @return Square wave amplitude.
 */
float WaveformsSquare(const float normalisedPeriod, const float shape) {
    return normalisedPeriod < shape ? -1.0f : 1.0f;
}

/**
 * @breif Returns stepped triangle wave amplitude for a normalised period.
 * @param normalisedPeriod 0.0 to 1.0 corresponding to 0 to 2 pi.
 * @param shape 0.0 to 1.0 adjusts the number of steps between 3 and 32.
 * @return Stepped triangle amplitude.
 */
float WaveformsSteppedTriangle(const float normalisedPeriod, const float shape) {
    const unsigned int numberOfSteps = 3 + ROUND(shape * 29.0f);
    const float numberOfStepsMinusOne = (float) (numberOfSteps - 1);
    float normalisedWaveform;
    if (normalisedPeriod > 0.5f) {
        normalisedWaveform = floorf((2.0f * normalisedPeriod - 1.0f) * numberOfStepsMinusOne) * (1.0f / numberOfStepsMinusOne);
    } else {
        normalisedWaveform = ceilf((-2.0f * normalisedPeriod + 1.0f) * numberOfStepsMinusOne) * (1.0f / numberOfStepsMinusOne);
    }
    return -2.0f * (normalisedWaveform - 0.5f);
}

/**
 * @breif Returns stepped sawtooth wave amplitude for a normalised period.
 * @param normalisedPeriod 0.0 to 1.0 corresponding to 0 to 2 pi.
 * @param shape 0.0 to 1.0 adjusts the number of steps between 3 and 32.
 * @return Stepped sawtooth amplitude.
 */
float WaveformsSteppedSawtooth(const float normalisedPeriod, const float shape) {
    const unsigned int numberOfSteps = 3 + ROUND(shape * 29.0f);
    const float normalisedWaveform = floorf(normalisedPeriod * numberOfSteps) * (1.0f / (numberOfSteps - 1.0f));
    return 2.0f * (normalisedWaveform - 0.5f);
}

//------------------------------------------------------------------------------
// End of file
