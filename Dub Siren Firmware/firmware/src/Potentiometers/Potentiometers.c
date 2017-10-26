/**
 * @file Potentiometers.c
 * @author Seb Madgwick
 * @brief Potentiometers module to provide high-resolution measurement of
 * potentiometers values through oversampling.
 *
 * Assumes PBLCK3 is 84 MHz so that TQ = 11.9 ns and TAD = 23.8 ns.
 */

//------------------------------------------------------------------------------
// Includes

#include <xc.h>
#include "Potentiometers.h"
#include <stdint.h>
#include "system/int/sys_int.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @breif Oversampling rate.  Must be an integer value.
 */
#define OVERSAMPLING (100)

/**
 * @breif Maximum ADC value.
 */
#define MAXIMUM_ADC_VALUE ((1 << 12) - 1)

/**
 * @brief ADC data accumulators for oversampled averaging.
 */
typedef struct {
    uint32_t sampleCount;
    uint32_t input1;
    uint32_t input2;
    uint32_t input3;
    uint32_t input4;
    uint32_t input5;
    uint32_t input6;
    uint32_t input7;
    uint32_t input8;
    uint32_t input9;
} AdcDataAccumulator;

#define SAMC_VALUE (100)

//------------------------------------------------------------------------------
// Variables

static AdcDataAccumulator adcDataAccumulator;
static float currentPotentiometers[NUMBER_OF_POTENTIOMETERS];

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises module.  This function should be called once on system
 * start up.
 */
void PotentiometersInitialise() {

    // Load calibration
    ADC0CFG = DEVADC0;
    ADC1CFG = DEVADC1;
    ADC2CFG = DEVADC2;
    ADC3CFG = DEVADC3;
    ADC4CFG = DEVADC4;
    ADC7CFG = DEVADC7;

    // Configure ADC control registers
    ADCCON1bits.STRGSRC = 1; // scan trigger source is GLSWTRG
    ADCCON1bits.AICPMPEN = 0; // charge pump disabled

    // ADC0 timing
    ADC0TIMEbits.ADCDIV = 1; // TAD = 2 * TQ
    ADC0TIMEbits.SAMC = SAMC_VALUE;

    // ADC1 timing
    ADC1TIMEbits.ADCDIV = 1; // TAD = 2 * TQ
    ADC1TIMEbits.SAMC = SAMC_VALUE;

    // ADC2 timing
    ADC2TIMEbits.ADCDIV = 1; // TAD = 2 * TQ
    ADC2TIMEbits.SAMC = SAMC_VALUE;

    // ADC4 timing
    ADC4TIMEbits.ADCDIV = 1; // TAD = 2 * TQ
    ADC4TIMEbits.SAMC = SAMC_VALUE;

    // ADC7 timing
    ADCCON2bits.ADCDIV = 1; // TAD = 2 * TQ
    ADCCON2bits.SAMC = SAMC_VALUE;

    // Warm up timing
    ADCANCONbits.WKUPCLKCNT = 5;

    // Enable alternative analogue inputs
    ADCTRGMODEbits.SH0ALT = 0b01; // AN45
    ADCTRGMODEbits.SH1ALT = 0b01; // AN46
    ADCTRGMODEbits.SH2ALT = 0b01; // AN47
    ADCTRGMODEbits.SH4ALT = 0b01; // AN49

    // Enable inputs for common scan
    ADCCSS1bits.CSS2 = 1; // AN2 is AN47 because SH2ALT = 0b01
    ADCCSS1bits.CSS4 = 1; // AN4 is AN49 because SH4ALT = 0b01
    ADCCSS1bits.CSS0 = 1; // AN0 is AN45 because SH0ALT = 0b01
    ADCCSS1bits.CSS1 = 1; // AN1 is AN46 because SH1ALT = 0b01
    ADCCSS1bits.CSS12 = 1;
    ADCCSS1bits.CSS11 = 1;
    ADCCSS1bits.CSS15 = 1;
    ADCCSS1bits.CSS16 = 1;
    ADCCSS1bits.CSS17 = 1;

    // Set scan as trigger source
    ADCTRG1bits.TRGSRC0 = 0b00011; // AN2 is AN47 because SH2ALT = 0b01
    ADCTRG1bits.TRGSRC1 = 0b00011; // AN4 is AN49 because SH4ALT = 0b01
    ADCTRG1bits.TRGSRC2 = 0b00011; // AN0 is AN45 because SH0ALT = 0b01
    ADCTRG2bits.TRGSRC4 = 0b00011; // AN1 is AN46 because SH1ALT = 0b01
    ADCTRG3bits.TRGSRC11 = 0b00011;

    // Enable ADC
    ADCCON1bits.ON = 1;

    // Wait for voltage reference
    while (ADCCON2bits.BGVRRDY == 0);

    // Wake up ADCs
    ADCANCONbits.ANEN0 = 1;
    ADCANCONbits.ANEN1 = 1;
    ADCANCONbits.ANEN2 = 1;
    ADCANCONbits.ANEN4 = 1;
    ADCANCONbits.ANEN7 = 1;

    // Wait for ADCs to wake up
    while (ADCANCONbits.WKRDY0 == 0);
    while (ADCANCONbits.WKRDY1 == 0);
    while (ADCANCONbits.WKRDY2 == 0);
    while (ADCANCONbits.WKRDY4 == 0);
    while (ADCANCONbits.WKRDY7 == 0);

    // Enable digital ADCs
    ADCCON3bits.DIGEN0 = 1;
    ADCCON3bits.DIGEN1 = 1;
    ADCCON3bits.DIGEN2 = 1;
    ADCCON3bits.DIGEN4 = 1;
    ADCCON3bits.DIGEN7 = 1;

    // Configure end of scan interrupt
    ADCCON2bits.EOSIEN = 1; // Interrupt will be generated when EOSRDY bit is set
    SYS_INT_VectorPrioritySet(_ADC_EOS_VECTOR, INT_PRIORITY_LEVEL4); // set interrupt priority
    SYS_INT_SourceStatusClear(INT_SOURCE_ADC_END_OF_SCAN); // clear interrupt flag
    SYS_INT_SourceEnable(INT_SOURCE_ADC_END_OF_SCAN); // enable interrupt

    // Trigger first conversion
    ADCCON3bits.GSWTRG = 1;
}

/**
 * @brief Gets most recent potentiometers values normalised to a range of 0.0 to
 * 1.0.
 */
void PotentiometersGetValues(float potentiometers[NUMBER_OF_POTENTIOMETERS]) {
    SYS_INT_SourceDisable(INT_SOURCE_ADC_END_OF_SCAN); // enable interrupt
    unsigned int index;
    for (index = 0; index < NUMBER_OF_POTENTIOMETERS; index++) {
        potentiometers[index] = currentPotentiometers[index];
    }
    SYS_INT_SourceEnable(INT_SOURCE_ADC_END_OF_SCAN); // enable interrupt
}

/**
 * @brief ADC interrupt to store ADC results.
 */
void __ISR(_ADC_EOS_VECTOR) AdcEndOfScanInterrupt() {

    // Store ADC data to accumulators
    adcDataAccumulator.sampleCount++;
    adcDataAccumulator.input1 += ADCDATA2; // AN2 is AN47 because SH2ALT = 0b01
    adcDataAccumulator.input2 += ADCDATA4; // AN4 is AN49 because SH4ALT = 0b01
    adcDataAccumulator.input3 += ADCDATA0; // AN0 is AN45 because SH0ALT = 0b01
    adcDataAccumulator.input4 += ADCDATA1; // AN1 is AN46 because SH1ALT = 0b01
    adcDataAccumulator.input5 += ADCDATA12;
    adcDataAccumulator.input6 += ADCDATA11;
    adcDataAccumulator.input7 += ADCDATA15;
    adcDataAccumulator.input8 += ADCDATA16;
    adcDataAccumulator.input9 += ADCDATA17;

    // Calculate oversampled average
    if (adcDataAccumulator.sampleCount >= OVERSAMPLING) {

        // Calculate average
        currentPotentiometers[0] = adcDataAccumulator.input1 * (1.0f / ((float) MAXIMUM_ADC_VALUE * (float) OVERSAMPLING));
        currentPotentiometers[1] = adcDataAccumulator.input2 * (1.0f / ((float) MAXIMUM_ADC_VALUE * (float) OVERSAMPLING));
        currentPotentiometers[2] = adcDataAccumulator.input3 * (1.0f / ((float) MAXIMUM_ADC_VALUE * (float) OVERSAMPLING));
        currentPotentiometers[3] = adcDataAccumulator.input4 * (1.0f / ((float) MAXIMUM_ADC_VALUE * (float) OVERSAMPLING));
        currentPotentiometers[4] = adcDataAccumulator.input5 * (1.0f / ((float) MAXIMUM_ADC_VALUE * (float) OVERSAMPLING));
        currentPotentiometers[5] = adcDataAccumulator.input6 * (1.0f / ((float) MAXIMUM_ADC_VALUE * (float) OVERSAMPLING));
        currentPotentiometers[6] = adcDataAccumulator.input7 * (1.0f / ((float) MAXIMUM_ADC_VALUE * (float) OVERSAMPLING));
        currentPotentiometers[7] = adcDataAccumulator.input8 * (1.0f / ((float) MAXIMUM_ADC_VALUE * (float) OVERSAMPLING));
        currentPotentiometers[8] = adcDataAccumulator.input9 * (1.0f / ((float) MAXIMUM_ADC_VALUE * (float) OVERSAMPLING));

        // Reset accumulators
        adcDataAccumulator.sampleCount = 0;
        adcDataAccumulator.input1 = 0;
        adcDataAccumulator.input2 = 0;
        adcDataAccumulator.input3 = 0;
        adcDataAccumulator.input4 = 0;
        adcDataAccumulator.input5 = 0;
        adcDataAccumulator.input6 = 0;
        adcDataAccumulator.input7 = 0;
        adcDataAccumulator.input8 = 0;
        adcDataAccumulator.input9 = 0;
    }

    // Clear status bit else interrupt will persist
    ADCCON2bits.EOSRDY = 0;

    // Clear interrupt flag
    SYS_INT_SourceStatusClear(INT_SOURCE_ADC_END_OF_SCAN); // clear interrupt flag

    // Trigger next conversion
    ADCCON3bits.GSWTRG = 1;
}

//------------------------------------------------------------------------------
// End of file
