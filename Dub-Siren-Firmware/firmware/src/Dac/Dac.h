/**
 * @file Dac.h
 * @author Seb Madgwick
 * @brief CS4354 audio DAC interface.
 */

#ifndef DAC_H
#define DAC_H

//------------------------------------------------------------------------------
// Definitions

/**
 * @breif DAC sample frequency in Hz.
 */
#define SAMPLE_FREQUENCY (96000.0f)

//------------------------------------------------------------------------------
// Function prototypes

void DacInitialise(void (*audioUpdate)());
void DacWriteBuffer(const float sample);

#endif

//------------------------------------------------------------------------------
// End of file
