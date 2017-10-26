/**
 * @file Synthesiser.h
 * @author Seb Madgwick
 * @brief Synthesiser module.
 */

#ifndef SYNTHESISER_H
#define SYNTHESISER_H

//------------------------------------------------------------------------------
// Includes

#include "Dac/Dac.h"
#include <stdbool.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief LFO waveforms type.
 */
typedef enum {
    LfoWaveformSine,
    LfoWaveformTriangle,
    LfoWaveformSawtooth,
    LfoWaveformSquare,
    LfoWaveformSteppedTriangle,
    LfoWaveformSteppedSawtooth,
    LfoWaveformNumberOfWaveforms,
} LfoWaveform;

/**
 * @brief VCO waveforms type.
 */
typedef enum {
    VcoWaveformSine,
    VcoWaveformTriangle,
    VcoWaveformSawtooth,
    VcoWaveformSquare,
    VcoWaveformPulse,
    VcoWaveformOneBitNoise,
    VcoWaveformNumberOfWaveforms,
} VcoWaveform;

/**
 * @brief Delay filter type.
 */
typedef enum {
    DelayFilterTypeNone,
    DelayFilterTypeLowPass,
    DelayFilterTypeHighPass,
} DelayFilterType;

/**
 * @brief Synthesiser parameter structure.
 */
typedef struct {
    LfoWaveform lfoWaveform;
    float lfoShape; // 0.0 to 1.0
    float lfoFrequency; // Hz
    float lfoAmplitude; // Hz, negative values negate the LFO waveform
    bool lfoGateControl;
    VcoWaveform vcoWaveform;
    float vcoFrequency; // Hz
    float delayTime; // seconds
    float delayFeedback; // 0.0 to 1.0 corresponding to 0% to 100%
    DelayFilterType delayFilterType;
    float delayFilterFrequency; // Hz
} SynthesiserParameters;

//------------------------------------------------------------------------------
// Variable declarations

extern const SynthesiserParameters defaultSynthesiserParameters;

//------------------------------------------------------------------------------
// Function prototypes

void SynthesiserInitialise();
void SynthesiserSetParameters(const SynthesiserParameters * const newSynthesiserParameters);
void SynthesiserTrigger();
void SynthesiserSetGate(const bool state);
bool SynthesiserGetGate();

#endif

//------------------------------------------------------------------------------
// End of file
