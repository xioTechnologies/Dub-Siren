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

/**
 * @brief Default synthesiser parameters.
 */
#define DEFAULT_SYTHESISER_PARAMETERS ((SynthesiserParameters) { \
    .lfoWaveform = LfoWaveformSine, \
    .lfoShape = 0.5f, \
    .lfoFrequency = 2.0f, \
    .lfoAmplitude = 500.0f, \
    .lfoGateControl = false, \
    .vcoWaveform = LfoWaveformSine, \
    .vcoFrequency = 1000.0f, \
    .delayTime = 0.0f, \
    .delayFeedback = 0.0f, \
    .delayFilterType = DelayFilterTypeNone, \
    .delayFilterFrequency = 1.0f, })

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
