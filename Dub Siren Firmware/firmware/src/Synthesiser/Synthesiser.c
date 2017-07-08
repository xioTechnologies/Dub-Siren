/**
 * @file Synthesiser.c
 * @author Seb Madgwick
 * @brief Synthesiser module.
 */

//------------------------------------------------------------------------------
// Includes

#include "Filters/CascadeFilter.h"
#include "Filters/FirstOrderFilter.h"
#include "MathHelpers.h"
#include "Synthesiser.h"
#include "Waveforms.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief The period between the gate opening and the LFO period elapsing when
 * LFO gate control is enabled.
 */
#define PREEMPTIVE_GATE_PERIOD (0.01)

/**
 * @brief Delay buffer size.
 */
#define DELAY_BUFFER_SIZE (128000)

//------------------------------------------------------------------------------
// Function prototypes

static void AudioUpdate();
static float MixSamples(const float sampleA, const float sampleB, const float gain);
static void WriteToDelayBuffer(const float sample);
static float ReadFromDelayBuffer(float delay);
static void MixToDelayBuffer(const float sample);
static void IncrementDelayBufferIndex();

//------------------------------------------------------------------------------
// Variable declarations

static SynthesiserParameters synthesiserParameters;
static SynthesiserParameters pendingSynthesiserParameters = DEFAULT_SYTHESISER_PARAMETERS;
static bool newSynthesiserParametersPending = true; // initialised state is true to ensure default parameters are applied
static bool trigger;
static bool gate = true;
static FirstOrderFilter gateGainLowPassFilter;
static float delayBuffer[DELAY_BUFFER_SIZE];
static int delayBufferIndex = 0;
static FirstOrderFilter delayTimeLowPassFilter;
static CascadeFilter delayFilter;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises module.  This function should be called once on system
 * start up.
 */
void SynthesiserInitialise() {

    // Initialise fixed filters
    FirstOrderFilterSetCornerFrequency(&gateGainLowPassFilter, 100.0f, SAMPLE_FREQUENCY, false);
    FirstOrderFilterSetCornerFrequency(&delayTimeLowPassFilter, 1.0f, SAMPLE_FREQUENCY, false);

    // Initialise DAC
    DacInitialise(&AudioUpdate);
}

void SynthesiserSetParameters(const SynthesiserParameters * const newSynthesiserParameters) {
    newSynthesiserParametersPending = false;
    pendingSynthesiserParameters = *newSynthesiserParameters;
    newSynthesiserParametersPending = true;
}

/**
 * @brief Triggers synthesiser.
 */
void SynthesiserTrigger() {
    trigger = true;
}

/**
 * @brief Sets gate state.
 * @param state Gate state.
 */
void SynthesiserSetGate(const bool state) {
    gate = state;
}

/**
 * @brief Returns current gate state.
 * @return Current gate state.
 */
bool SynthesiserGetGate() {
    return gate;
}

/**
 * @brief Updates audio calculations and writes output to DAC buffer.
 */
static void AudioUpdate() {

    // Must write to DAC buffer immediately
    static float output = 0.0f;
    DacWriteBuffer(output);

    // Update synthesiser parameters
    if (newSynthesiserParametersPending == true) {
        synthesiserParameters = pendingSynthesiserParameters;
        CascadeFilterSetCornerFrequency(&delayFilter,
                synthesiserParameters.delayFilterFrequency,
                SAMPLE_FREQUENCY,
                synthesiserParameters.delayFilterType == DelayFilterTypeHighPass,
                3);
        newSynthesiserParametersPending = false;
    }

    // LFO
    static float lfoPeriodClock = 0.0f;
    if (trigger == true) {
        trigger = false;
        lfoPeriodClock = 0.0f;
        gate = true;
    }
    float lfoWaveform = 0.0f;
    switch (synthesiserParameters.lfoWaveform) {
        case LfoWaveformSine:
            lfoWaveform = WaveformsAsymmetricSine(lfoPeriodClock, synthesiserParameters.lfoShape);
            break;
        case LfoWaveformTriangle:
            lfoWaveform = WaveformsTriangle(lfoPeriodClock, synthesiserParameters.lfoShape);
            break;
        case LfoWaveformSawtooth:
            lfoWaveform = WaveformsSawtooth(lfoPeriodClock, synthesiserParameters.lfoShape);
            break;
        case LfoWaveformSquare:
            lfoWaveform = WaveformsSquare(lfoPeriodClock, synthesiserParameters.lfoShape);
            break;
        case LfoWaveformSteppedTriangle:
            lfoWaveform = WaveformsSteppedTriangle(lfoPeriodClock, synthesiserParameters.lfoShape);
            break;
        case LfoWaveformSteppedSawtooth:
            lfoWaveform = WaveformsSteppedSawtooth(lfoPeriodClock, synthesiserParameters.lfoShape);
            break;
    }
    lfoPeriodClock += (1.0f / SAMPLE_FREQUENCY) * synthesiserParameters.lfoFrequency;
    if ((synthesiserParameters.lfoGateControl == true) && (lfoPeriodClock >= (1.0f - (PREEMPTIVE_GATE_PERIOD * synthesiserParameters.lfoFrequency)))) {
        gate = false;
    }
    lfoPeriodClock = WaveformsLimitNormalisedPeriod(lfoPeriodClock);
    const float vcoModulatedFrequency = synthesiserParameters.vcoFrequency + synthesiserParameters.lfoAmplitude * lfoWaveform;

    // VCO
    static float vcoPeriodClock = 0.0f;
    switch (synthesiserParameters.vcoWaveform) {
        case VcoWaveformSine:
            output = WaveformsSine(vcoPeriodClock);
            break;
        case VcoWaveformTriangle:
            output = WaveformsBandwidthLimitedTriangle(vcoPeriodClock, vcoModulatedFrequency);
            break;
        case VcoWaveformSawtooth:
            output = WaveformsBandwidthLimitedSawtooth(vcoPeriodClock, vcoModulatedFrequency);
            break;
        case VcoWaveformSquare:
            output = WaveformsBandwidthLimitedSquare(vcoPeriodClock, vcoModulatedFrequency);
            break;
        case VcoWaveformPulse:
            output = WaveformsBandwidthLimitedPulse(vcoPeriodClock, vcoModulatedFrequency);
            break;
        case VcoWaveformOneBitNoise:
            output = WaveformsOneBitNoise(vcoModulatedFrequency, SAMPLE_FREQUENCY);
            break;
    }
    vcoPeriodClock += (1.0f / SAMPLE_FREQUENCY) * vcoModulatedFrequency;
    vcoPeriodClock = WaveformsLimitNormalisedPeriod(vcoPeriodClock);

    // Gate
    static float gateGain = 0.0f;
    gateGain = FirstOrderFilterUpdate(&gateGainLowPassFilter, gate == true ? 1.0f : 0.0f);
    output *= gateGain;

    // Attenuate output
    output *= 0.25f;

    // Delay
    WriteToDelayBuffer(output);
    float delaySample = synthesiserParameters.delayFeedback * ReadFromDelayBuffer(synthesiserParameters.delayTime);
    if (synthesiserParameters.delayFilterType != DelayFilterTypeNone) {
        delaySample = CascadeFilterUpdate(&delayFilter, delaySample);
    }
    MixToDelayBuffer(delaySample);
    IncrementDelayBufferIndex();
    output += delaySample;
}

/**
 * @brief Writes sample to delay buffer.
 * @param sample Sample to be written to delay buffer.
 */
static void WriteToDelayBuffer(const float sample) {
    delayBuffer[delayBufferIndex] = sample;
}

/**
 * @brief Returns sample read from delay buffer with specified delay time.
 * @param delay Delay time between 0.0 and 1.0.
 * @return Returns sample read from delay buffer.
 */
static float ReadFromDelayBuffer(float delayTime) {
    delayTime = FirstOrderFilterUpdate(&delayTimeLowPassFilter, delayTime); // filter out sudden changes to avoid distortion
    int readIndex = delayBufferIndex - CLAMP((int) (delayTime * SAMPLE_FREQUENCY), 0, DELAY_BUFFER_SIZE);
    (int) (delayTime * (float) (DELAY_BUFFER_SIZE - 1));
    if (readIndex < 0) {
        readIndex = DELAY_BUFFER_SIZE + readIndex; // handle index underflow
    }
    return delayBuffer[readIndex];
}

/**
 * @brief Mixes sample to delay buffer.
 * @param sample Sample to be mixed to delay buffer.
 */
static void MixToDelayBuffer(const float sample) {
    delayBuffer[delayBufferIndex] += CLAMP(sample, -1.0f, 1.0f);
}

/**
 * @brief Increments delay buffer index.
 */
static void IncrementDelayBufferIndex() {
    if (++delayBufferIndex >= DELAY_BUFFER_SIZE) {
        delayBufferIndex = 0;
    }
}

//------------------------------------------------------------------------------
// End of file
