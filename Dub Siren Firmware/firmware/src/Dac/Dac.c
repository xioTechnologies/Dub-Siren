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
#include <sys/attribs.h> // __ISR
#include "system_config.h" // SYS_CLK_BUS_REFERENCE_1
#include "Timer/Timer.h"
#include <xc.h>

//------------------------------------------------------------------------------
// Definitions

#define SPI1TX_IFSXCLR IFS3CLR
#define SPI1TX_IECXSET IEC3SET
#define SPI1TX_INT_BIT (1 << 15)

#define T1_IFSXCLR IFS0CLR
#define T1_IFSXSET IFS0SET
#define T1_IECXSET IEC0SET
#define T1_INT_BIT (1 << 4)

//------------------------------------------------------------------------------
// Variable declarations

static void (*audioUpdateFunction)();
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
    audioUpdateFunction = audioUpdate;

    // Configure I2S
    SPI1BRG = (int) ((((float) SYS_CLK_BUS_REFERENCE_1 / (2.0f * 6144000.0f)) - 1.0f) + 0.5f); // 6.144 MHz
    SPI1CONbits.MCLKSEL = 1; // REFCLKO1 is used by the Baud Rate Generator
    SPI1CONbits.MODE16 = 1; // 24-bit Data, 32-bit FIFO, 32-bit Channel/64-bit Frame
    SPI1CONbits.MODE32 = 1; // 24-bit Data, 32-bit FIFO, 32-bit Channel/64-bit Frame
    SPI1CONbits.CKP = 1; // Idle state for clock is a high level; active state is a low level
    SPI1CONbits.MSTEN = 1; // Master mode
    SPI1CON2bits.AUDEN = 1; // Audio protocol is enabled
    SPI1CON2bits.AUDMONO = 1; // Audio data is mono (Each data word is transmitted on both left and right channels)
    SPI1CONbits.ON = 1; // SPI/I2S module is enabled

    // Configure timer for for audio update interrupt (software triggered)
    IPC1bits.T1IP = 5; // set interrupt priority
    T1_IFSXCLR = T1_INT_BIT; // clear interrupt flag
    T1_IECXSET = T1_INT_BIT; // enable interrupt

    // Configure SPI interrupt
    IPC27bits.SPI1TXIP = 6; // set interrupt priority
    SPI1TX_IFSXCLR = SPI1TX_INT_BIT; // clear interrupt flag
    SPI1TX_IECXSET = SPI1TX_INT_BIT; // enable interrupt
}

/**
 * @breif SPI interrupt service routine to write buffered value to DAC and call
 * external audio update function.
 */
void __ISR(_SPI1_TX_VECTOR) Spi1TXInterrupt() {
    SPI1BUF = buffer;
    T1_IFSXSET = T1_INT_BIT; // trigger lower priority audio update interrupt
    SPI1TX_IFSXCLR = SPI1TX_INT_BIT; // clear interrupt flag
}

/**
 * @breif Timer interrupt service routine to update audio output.  This
 * interrupt is software triggered.
 */
void __ISR(_TIMER_1_VECTOR) Timer1Interrupt() {
    audioUpdateFunction();
    T1_IFSXCLR = T1_INT_BIT; // clear interrupt flag
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
