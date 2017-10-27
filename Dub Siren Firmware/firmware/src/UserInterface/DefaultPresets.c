/**
 * @file DefaultPresets.c
 * @author Seb Madgwick
 * @brief Default presets.
 */

//------------------------------------------------------------------------------
// Includes

#include "DefaultPresets.h"

//------------------------------------------------------------------------------
// Variable declarations

const SynthesiserParameters fogHorn = {
    .lfoWaveform = LfoWaveformSawtooth,
    .lfoShape = 0.870488,
    .lfoFrequency = 0.918656,
    .lfoAmplitude = -105.533592,
    .lfoGateControl = true,
    .vcoWaveform = VcoWaveformSquare,
    .vcoFrequency = 182.045883,
    .delayTime = 0.380044,
    .delayFeedback = 0.284430,
    .delayFilterType = DelayFilterTypeLowPass,
    .delayFilterFrequency = 6879.085938,
};

const SynthesiserParameters levelUp = {
    .lfoWaveform = LfoWaveformSteppedSawtooth,
    .lfoShape = 0.748286,
    .lfoFrequency = 1.156882,
    .lfoAmplitude = 466.714294,
    .lfoGateControl = true,
    .vcoWaveform = VcoWaveformPulse,
    .vcoFrequency = 471.714294,
    .delayTime = 0.380060,
    .delayFeedback = 0.284444,
    .delayFilterType = DelayFilterTypeLowPass,
    .delayFilterFrequency = 6879.655273,
};

const SynthesiserParameters marioCoin = {
    .lfoWaveform = LfoWaveformSquare,
    .lfoShape = 0.347211,
    .lfoFrequency = 5.576817,
    .lfoAmplitude = 374.693115,
    .lfoGateControl = true,
    .vcoWaveform = VcoWaveformSquare,
    .vcoFrequency = 1380.171265,
    .delayTime = 0.380063,
    .delayFeedback = 0.284503,
    .delayFilterType = DelayFilterTypeLowPass,
    .delayFilterFrequency = 6878.091309,
};

const SynthesiserParameters bombFalling = {
    .lfoWaveform = LfoWaveformSteppedSawtooth,
    .lfoShape = 1.000000,
    .lfoFrequency = 0.418580,
    .lfoAmplitude = -716.046814,
    .lfoGateControl = true,
    .vcoWaveform = VcoWaveformPulse,
    .vcoFrequency = 1123.452881,
    .delayTime = 0.380076,
    .delayFeedback = 0.284415,
    .delayFilterType = DelayFilterTypeLowPass,
    .delayFilterFrequency = 6877.664551,
};

const SynthesiserParameters policeSiren = {
    .lfoWaveform = LfoWaveformTriangle,
    .lfoShape = 0.258210,
    .lfoFrequency = 0.170868,
    .lfoAmplitude = 688.041199,
    .lfoGateControl = false,
    .vcoWaveform = VcoWaveformTriangle,
    .vcoFrequency = 1013.351929,
    .delayTime = 0.380083,
    .delayFeedback = 0.284545,
    .delayFilterType = DelayFilterTypeLowPass,
    .delayFilterFrequency = 6879.085938,
};

const SynthesiserParameters classicDubSirenLow = {
    .lfoWaveform = LfoWaveformTriangle,
    .lfoShape = 0.497863,
    .lfoFrequency = 1.808816,
    .lfoAmplitude = 148.919556,
    .lfoGateControl = false,
    .vcoWaveform = VcoWaveformSquare,
    .vcoFrequency = 205.318161,
    .delayTime = 0.380047,
    .delayFeedback = 0.284479,
    .delayFilterType = DelayFilterTypeLowPass,
    .delayFilterFrequency = 6876.527832,
};

const SynthesiserParameters highHat = {
    .lfoWaveform = LfoWaveformSine,
    .lfoShape = 0.500000,
    .lfoFrequency = 14.999997,
    .lfoAmplitude = 0.000000,
    .lfoGateControl = true,
    .vcoWaveform = VcoWaveformOneBitNoise,
    .vcoFrequency = 4999.999023,
    .delayTime = 0.380047,
    .delayFeedback = 0.284454,
    .delayFilterType = DelayFilterTypeLowPass,
    .delayFilterFrequency = 6874.536133,
};

const SynthesiserParameters classicDubSirenHigh = {
    .lfoWaveform = LfoWaveformTriangle,
    .lfoShape = 0.497863,
    .lfoFrequency = 2.738417,
    .lfoAmplitude = -650.351685,
    .lfoGateControl = false,
    .vcoWaveform = VcoWaveformPulse,
    .vcoFrequency = 655.351685,
    .delayTime = 0.379998,
    .delayFeedback = 0.284547,
    .delayFilterType = DelayFilterTypeLowPass,
    .delayFilterFrequency = 6876.810547,
};

const SynthesiserParameters bombExploding = {
    .lfoWaveform = LfoWaveformSteppedSawtooth,
    .lfoShape = 1.000000,
    .lfoFrequency = 0.597933,
    .lfoAmplitude = -813.184753,
    .lfoGateControl = true,
    .vcoWaveform = VcoWaveformOneBitNoise,
    .vcoFrequency = 1325.321045,
    .delayTime = 0.380083,
    .delayFeedback = 0.284484,
    .delayFilterType = DelayFilterTypeLowPass,
    .delayFilterFrequency = 6876.667969,
};

const SynthesiserParameters airRaidSiren = {
    .lfoWaveform = LfoWaveformSine,
    .lfoShape = 0.672904,
    .lfoFrequency = 0.027004,
    .lfoAmplitude = 420.044983,
    .lfoGateControl = false,
    .vcoWaveform = VcoWaveformSawtooth,
    .vcoFrequency = 468.001923,
    .delayTime = 0.380086,
    .delayFeedback = 0.284405,
    .delayFilterType = DelayFilterTypeLowPass,
    .delayFilterFrequency = 6873.684570,
};

//------------------------------------------------------------------------------
// End of file
