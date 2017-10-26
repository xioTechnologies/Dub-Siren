/**
 * @file Dac.c
 * @author Seb Madgwick
 * @brief CS4354 audio DAC interface implemented for 24-bit mono at 96 kHz.
 *
 * REFCLKO1 is configured for 24.576 MHz using MPLAB Harmony.  This corresponds
 * to an LRCK value of 96 kHz and I2S data clock of SCLK of 6.144 MHz (64 bits
 * per LRCK period).  See page 13 of CS4354 datasheet.
 */

//------------------------------------------------------------------------------
// Includes

#include "Dac.h"
#include "MathHelpers.h"
#include "system/int/sys_int.h"
#include "system_config.h" // SYS_CLK_BUS_REFERENCE_1
#include "Timer/Timer.h"
#include <xc.h>

//------------------------------------------------------------------------------
// Variable declarations

static void (*audioUpdateCallback)();
static int buffer;

//------------------------------------------------------------------------------
// Functions

/**
 * @breif Initialises module.  This function should be called once on system
 * start up.
 * @param audioUpdate External audio update function.  The audio update function
 * must call DacWriteBuffer immediately.
 */
void DacInitialise(void (*audioUpdate)()) {

    // Store address of audio update function
    audioUpdateCallback = audioUpdate;

    // Configure I2S
    SPI1BRG = (unsigned int) ((((float) SYS_CLK_BUS_REFERENCE_1 / (2.0f * 6144000.0f)) - 1.0f) + 0.5f); // 6.144 MHz
    SPI1CONbits.MCLKSEL = 1; // REFCLKO1 is used by the Baud Rate Generator
    SPI1CONbits.MODE16 = 1; // 24-bit Data, 32-bit FIFO, 32-bit Channel/64-bit Frame
    SPI1CONbits.MODE32 = 1; // 24-bit Data, 32-bit FIFO, 32-bit Channel/64-bit Frame
    SPI1CONbits.CKP = 1; // Idle state for clock is a high level; active state is a low level
    SPI1CONbits.MSTEN = 1; // Master mode
    SPI1CON2bits.AUDEN = 1; // Audio protocol is enabled
    SPI1CON2bits.AUDMONO = 1; // Audio data is mono (Each data word is transmitted on both left and right channels)
    SPI1CONbits.ON = 1; // SPI/I2S module is enabled

    // Configure timer for for audio update interrupt (software triggered)
    SYS_INT_VectorPrioritySet(_TIMER_1_VECTOR, INT_PRIORITY_LEVEL5); // set interrupt priority
    SYS_INT_SourceStatusClear(INT_SOURCE_TIMER_1); // clear interrupt flag
    SYS_INT_SourceEnable(INT_SOURCE_TIMER_1); // enable interrupt

    // Configure SPI interrupt
    SYS_INT_VectorPrioritySet(_SPI1_TX_VECTOR, INT_PRIORITY_LEVEL6); // set interrupt priority
    SYS_INT_SourceStatusClear(INT_SOURCE_SPI_1_TRANSMIT); // clear interrupt flag
    SYS_INT_SourceEnable(INT_SOURCE_SPI_1_TRANSMIT); // enable interrupt
}

/**
 * @breif SPI interrupt service routine to write buffered value to DAC and call
 * external audio update function.
 */
void __ISR(_SPI1_TX_VECTOR) Spi1TXInterrupt() {
    SPI1BUF = buffer;
    SYS_INT_SourceStatusSet(INT_SOURCE_TIMER_1); // trigger lower priority audio update interrupt
    SYS_INT_SourceStatusClear(INT_SOURCE_SPI_1_TRANSMIT); // clear interrupt flag
}

/**
 * @breif Timer interrupt service routine to update audio output.  This
 * interrupt is software triggered.
 */
void __ISR(_TIMER_1_VECTOR) Timer1Interrupt() {
    audioUpdateCallback();
    SYS_INT_SourceStatusClear(INT_SOURCE_TIMER_1); // clear interrupt flag
}

/**
 * @breif Writes sample to DAC buffer.
 * @param sample Sample value between -1.0 and +1.0.
 */
void DacWriteBuffer(const float sample) {
    buffer = CLAMP(sample, -1.0f, 1.0f) * (float) 0x7FFFFF;
}

//------------------------------------------------------------------------------
// End of file
