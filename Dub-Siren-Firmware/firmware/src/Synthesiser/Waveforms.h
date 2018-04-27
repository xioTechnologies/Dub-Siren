/**
 * @file Waveforms.h
 * @author Seb Madgwick
 * @brief LFO and VCO waveforms.
 */

#ifndef WAVEFORMS_H
#define WAVEFORMS_H

//------------------------------------------------------------------------------
// Function prototypes

float WaveformsLimitNormalisedPeriod(float normalisedPeriod);
float WaveformsSine(const float normalisedPeriod);
float WaveformsBandwidthLimitedTriangle(const float normalisedPeriod, const float frequency);
float WaveformsBandwidthLimitedSawtooth(const float normalisedPeriod, const float frequency);
float WaveformsBandwidthLimitedSquare(const float normalisedPeriod, const float frequency);
float WaveformsBandwidthLimitedPulse(const float normalisedPeriod, const float frequency);
float WaveformsOneBitNoise(const float frequency, const float sampleFrequency);
float WaveformsAsymmetricSine(const float normalisedPeriod, const float shape);
float WaveformsTriangle(const float normalisedPeriod, const float shape);
float WaveformsSawtooth(const float normalisedPeriod, const float shape);
float WaveformsSquare(const float normalisedPeriod, const float shape);
float WaveformsSteppedTriangle(const float normalisedPeriod, const float shape);
float WaveformsSteppedSawtooth(const float normalisedPeriod, const float shape);

#endif

//------------------------------------------------------------------------------
// End of file
