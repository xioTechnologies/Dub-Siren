/**
 * @file UserInterface.c
 * @author Seb Madgwick
 * @brief User interface module.
 */

//------------------------------------------------------------------------------
// Includes

#include "DebouncedButton/DebouncedButton.h"
#include "Eeprom/Eeprom.h"
#include "I2cBitBang/I2cBitBang.h"
#include "IODefinitions.h"
#include <math.h> // fabs, copysignf
#include "MathHelpers.h"
#include "Potentiometers/Potentiometers.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h> // snprintf
#include <string.h> // strlen
#include "Synthesiser/Synthesiser.h"
#include "Uart/Uart1.h"
#include "UserInterface/UserInterface.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Number of preset keys.
 */
#define NUMBER_OF_PRESET_KEYS (10)

/**
 * @brief Minimum VCO frequency in Hz.
 */
#define MINIMUM_VCO_FREQUENCY (5.0f)

/**
 * @brief Maximum VCO frequency in Hz.
 */
#define MAXIMUM_VCO_FREQUENCY (5000.0f)

/**
 * @brief Calculates the cube of a value.
 */
#define CUBE(value) ((value) * (value) * (value))

/**
 * @brief Preset data structure.
 */
typedef struct {
    SynthesiserParameters presets[NUMBER_OF_PRESET_KEYS];
    int32_t checksum;
} EepromData;

//------------------------------------------------------------------------------
// Function prototypes

static void WaitHalfClockCycle();
static void WriteScl(const bool state);
static bool ReadSda();
static void WriteSda(const bool state);
static void LoadPresetsFromEeprom();
static void RestoreDefaultPresets();
static void SavePresetsToFromEeprom();
static void CheckForFactoryReset();
static bool AnyOrAllButtonOrKeyIsHeld(const bool isHeldState);
static void ReadPotentiometers(SynthesiserParameters * const synthesiserParameters);
static int InterpretDiscretePotentiometer(const float potentiometer, const unsigned int numberOfValues, const bool omitDeadbands);
static bool ComparePotentiometers(const float potentiometerA, const float potentiometerB);
static void PrintSynthesiserParameters(const SynthesiserParameters * const synthesiserParameters);

//------------------------------------------------------------------------------
// Variable declarations

static DebouncedButton triggerSaveButton;
static DebouncedButton lfoGateControlButton;
static DebouncedButton gateButton;
static DebouncedButton presetKeys[NUMBER_OF_PRESET_KEYS];
static I2cBitBang i2cBitBang;
static EepromData eepromData;
static bool ignorePotentiometers;
static bool potentiometerIgnored[NUMBER_OF_POTENTIOMETERS];
static bool undoIgnorePotentiometers;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises module.  This function should be called once on system
 * start up.
 */
void UserInterfaceInitialise() {

    // Initialise potentiometers to start continuous background ADC conversions
    PotentiometersInitialise();

    // Initialise debounced buttons
    DebouncedButtonInitialise(&triggerSaveButton, &TRIGGER_SAVE_BUTTON_PORT, TRIGGER_SAVE_BUTTON_PORT_BIT);
    DebouncedButtonInitialise(&lfoGateControlButton, &LFO_GATE_CONTROL_BUTTON_PORT, LFO_GATE_CONTROL_BUTTON_PORT_BIT);
    DebouncedButtonInitialise(&gateButton, &GATE_BUTTON_PORT, GATE_BUTTON_PORT_BIT);
    DebouncedButtonInitialise(&presetKeys[0], &PRESET_KEY_1_PORT, PRESET_KEY_1_PORT_BIT);
    DebouncedButtonInitialise(&presetKeys[1], &PRESET_KEY_2_PORT, PRESET_KEY_2_PORT_BIT);
    DebouncedButtonInitialise(&presetKeys[2], &PRESET_KEY_3_PORT, PRESET_KEY_3_PORT_BIT);
    DebouncedButtonInitialise(&presetKeys[3], &PRESET_KEY_4_PORT, PRESET_KEY_4_PORT_BIT);
    DebouncedButtonInitialise(&presetKeys[4], &PRESET_KEY_5_PORT, PRESET_KEY_5_PORT_BIT);
    DebouncedButtonInitialise(&presetKeys[5], &PRESET_KEY_6_PORT, PRESET_KEY_6_PORT_BIT);
    DebouncedButtonInitialise(&presetKeys[6], &PRESET_KEY_7_PORT, PRESET_KEY_7_PORT_BIT);
    DebouncedButtonInitialise(&presetKeys[7], &PRESET_KEY_8_PORT, PRESET_KEY_8_PORT_BIT);
    DebouncedButtonInitialise(&presetKeys[8], &PRESET_KEY_9_PORT, PRESET_KEY_9_PORT_BIT);
    DebouncedButtonInitialise(&presetKeys[9], &PRESET_KEY_10_PORT, PRESET_KEY_10_PORT_BIT);

    // Load presets
    I2CBitBangInitialise(&i2cBitBang, &WaitHalfClockCycle, &WriteScl, &ReadSda, &WriteSda);
    I2CBitBangBusClear(&i2cBitBang);
    LoadPresetsFromEeprom();
}

/**
 * @brief Implements delay for half clock cycle for use by I2C bit-bang.
 */
static void WaitHalfClockCycle() {
    TimerDelayMicroseconds(3);
}

/**
 * @brief Implements write to SCL pin for use by I2C bit-bang.
 * @param state SCL pin state.
 */
static void WriteScl(const bool state) {
    EEPROM_SCL_LAT = state == true ? 1 : 0;
}

/**
 * @brief Implements read of SDA pin for use by I2C bit-bang.
 * @return SDA pin state.
 */
static bool ReadSda() {
    return EEPROM_SDA_PORT == 1;
}

/**
 * @brief Implements write to SDA pin for use by I2C bit-bang.
 * @param state SDA pin state.
 */
static void WriteSda(const bool state) {
    EEPROM_SDA_LAT = state == true ? 1 : 0;
}

/**
 * @brief Loads presets from EEPROM.
 */
static void LoadPresetsFromEeprom() {

    // Read EEPROM data
    EepromRead(&i2cBitBang, 0, (char*) &eepromData, sizeof (eepromData));

    // Verify checksum
    unsigned int index;
    for (index = 0; index < sizeof (eepromData.presets); index++) {
        eepromData.checksum += (int32_t) ((uint8_t*) (&eepromData))[index];
    }
    if (eepromData.checksum == 0) {
        Uart1WriteStringIfReady("\r\nEEPROM checksum OK\r\n");
        return;
    }
    Uart1WriteStringIfReady("\r\nEEPROM checksum FAILED\r\n");

    // Load default presets
    RestoreDefaultPresets();
}

/**
 * @brief Loads default presets.
 */
static void RestoreDefaultPresets() {
    unsigned int presetKeyIndex;
    for (presetKeyIndex = 0; presetKeyIndex < NUMBER_OF_PRESET_KEYS; presetKeyIndex++) {
        eepromData.presets[presetKeyIndex] = DEFAULT_SYTHESISER_PARAMETERS;
    }
    SavePresetsToFromEeprom();
}

/**
 * @brief Saves presets to EEPROM.
 */
static void SavePresetsToFromEeprom() {

    // Calculate checksum
    eepromData.checksum = 0;
    unsigned int index;
    for (index = 0; index < sizeof (eepromData.presets); index++) {
        eepromData.checksum -= (int32_t) ((uint8_t*) (&eepromData))[index];
    }

    // Write EEPROM data
    EepromWrite(&i2cBitBang, 0, (char*) &eepromData, sizeof (eepromData));
}

/**
 * @brief Do tasks.  This function should be called repeatedly within the main
 * program loop.
 */
void UserInterfaceDoTasks() {
    static SynthesiserParameters synthesiserParameters;
    static bool nonPresetLfoGateControl;

    // Factory reset
    CheckForFactoryReset();

    // Trigger button
    bool trigger = false;
    if (DebouncedButtonWasPressed(&triggerSaveButton) == true) {
        unsigned int index;
        for (index = 0; index < NUMBER_OF_PRESET_KEYS; index++) {
            if (DebouncedButtonIsHeld(&presetKeys[index]) == true) {
                undoIgnorePotentiometers = true;
                synthesiserParameters.lfoGateControl = nonPresetLfoGateControl;
                break;
            }
        }
        trigger = true;
    }

    // LFO gate control button
    if (DebouncedButtonWasPressed(&lfoGateControlButton) == true) {
        synthesiserParameters.lfoGateControl = !synthesiserParameters.lfoGateControl; // toggle state
        nonPresetLfoGateControl = synthesiserParameters.lfoGateControl;
    }

    // Gate button
    if (DebouncedButtonWasPressed(&gateButton) == true) {
        SynthesiserSetGate(!SynthesiserGetGate()); // toggle state
    }

    // Preset keys
    unsigned int presetKeyIndex;
    for (presetKeyIndex = 0; presetKeyIndex < NUMBER_OF_PRESET_KEYS; presetKeyIndex++) {
        if (DebouncedButtonWasPressed(&presetKeys[presetKeyIndex]) == true) {
            if (DebouncedButtonIsHeld(&triggerSaveButton) == true) {
                eepromData.presets[presetKeyIndex] = synthesiserParameters;
                SavePresetsToFromEeprom();
            }
            synthesiserParameters = eepromData.presets[presetKeyIndex];
            ignorePotentiometers = true;
            trigger = true;
            break;
        }
    }

    // LFO gate control LED
    if (synthesiserParameters.lfoGateControl == true) {
        LFO_GATE_CONTROL_LED_LAT = 1;
    } else {
        LFO_GATE_CONTROL_LED_LAT = 0;
    }

    // Gate LED
    if (SynthesiserGetGate() == true) {
        GATE_LED_LAT = 1;
    } else {
        GATE_LED_LAT = 0;
    }

    // Read potentiometers
    ReadPotentiometers(&synthesiserParameters);

    // Trigger
    if (trigger == true) {
        SynthesiserTrigger();
        PrintSynthesiserParameters(&synthesiserParameters);
    }

    // Update synthesiser parameters
    SynthesiserSetParameters(&synthesiserParameters);
}

/**
 * @brief Loads default presets if all buttons and keys held for a 3 seconds.
 */
static void CheckForFactoryReset() {
    const uint64_t previousTicks = TimerGetTicks64();
    do {
        if (AnyOrAllButtonOrKeyIsHeld(false) == true) {
            return; // return if any buttons or keys not held
        }
        const uint64_t currentTicks = TimerGetTicks64();
        if ((currentTicks - previousTicks) >= (3 * timerTicksPerSecond)) {

            // Load default presets
            RestoreDefaultPresets();

            // Flash LEDs until all buttons and keys released
            do {
                LFO_GATE_CONTROL_LED_LAT = 0;
                GATE_LED_LAT = 0;
                TimerDelay(50);
                LFO_GATE_CONTROL_LED_LAT = 1;
                GATE_LED_LAT = 1;
                TimerDelay(50);
            } while (AnyOrAllButtonOrKeyIsHeld(true) == true);
        }
    } while (true);
}

/**
 * @brief Returns the collective isHeld state of all buttons and keys.
 * @param isHeldState True to return true if any button or key is held, false to
 * return true if any button or key is not held.
 * @return
 */
static bool AnyOrAllButtonOrKeyIsHeld(const bool isHeldState) {
    unsigned int index;
    for (index = 0; index < NUMBER_OF_PRESET_KEYS; index++) {
        if (DebouncedButtonIsHeld(&presetKeys[index]) == isHeldState) {
            return true;
        }
    }
    if (DebouncedButtonIsHeld(&triggerSaveButton) == isHeldState) {
        return true;
    }
    if (DebouncedButtonIsHeld(&lfoGateControlButton) == isHeldState) {
        return true;
    }
    if (DebouncedButtonIsHeld(&gateButton) == isHeldState) {
        return true;
    }
    return false;
}

/**
 * @brief Reads potentiometers as synthesiser parameters.
 * @param synthesiserParameters Synthesiser parameters to be written to.
 */
static void ReadPotentiometers(SynthesiserParameters * const synthesiserParameters) {

    // Get potentiometer values
    float potentiometers[NUMBER_OF_POTENTIOMETERS];
    PotentiometersGetValues(potentiometers);

    // Ignore potentiometers
    static float potentiometersWhenIgnored[NUMBER_OF_POTENTIOMETERS];
    if (ignorePotentiometers == true) {
        unsigned int index;
        for (index = 0; index < NUMBER_OF_POTENTIOMETERS; index++) {
            potentiometerIgnored[index] = true;
            potentiometersWhenIgnored[index] = potentiometers[index];
        }
        ignorePotentiometers = false;
    }

    // Undo ignore potentiometers
    if (undoIgnorePotentiometers == true) {
        unsigned int index;
        for (index = 0; index < NUMBER_OF_POTENTIOMETERS; index++) {
            potentiometerIgnored[index] = false;
        }
        undoIgnorePotentiometers = false;
    }

    // LFO waveform
    unsigned int index = PotentiometerIndexLfoWaveform;
    if (potentiometerIgnored[index] == true) {
        potentiometerIgnored[index] = ComparePotentiometers(potentiometers[index], potentiometersWhenIgnored[index]);
    } else {
        static int validValue = -1;
        const int currentValue = InterpretDiscretePotentiometer(potentiometers[index], LfoWaveformNumberOfWaveforms, validValue != -1);
        if (currentValue != -1) {
            validValue = currentValue;
        }
        synthesiserParameters->lfoWaveform = (LfoWaveform) validValue;
    }

    // LFO shape
    index = PotentiometerIndexLfoShape;
    if (potentiometerIgnored[index] == true) {
        potentiometerIgnored[index] = ComparePotentiometers(potentiometers[index], potentiometersWhenIgnored[index]);
    } else {
        synthesiserParameters->lfoShape = potentiometers[index];
    }

    // LFO frequency
    index = PotentiometerIndexLfoFrequency;
    if (potentiometerIgnored[index] == true) {
        potentiometerIgnored[index] = ComparePotentiometers(potentiometers[index], potentiometersWhenIgnored[index]);
    } else {
        synthesiserParameters->lfoFrequency = CUBE(potentiometers[index]) * 15.0f;
    }

    // VCO frequency
    index = PotentiometerIndexVcoFrequency;
    if (potentiometerIgnored[index] == true) {
        potentiometerIgnored[index] = ComparePotentiometers(potentiometers[index], potentiometersWhenIgnored[index]);
    } else {
        synthesiserParameters->vcoFrequency = MAP(CUBE(potentiometers[index]), 0.0f, 1.0f, MINIMUM_VCO_FREQUENCY, MAXIMUM_VCO_FREQUENCY);
    }

    // LFO amplitude
    index = PotentiometerIndexLfoAmplitude;
    if (potentiometerIgnored[index] == true) {
        potentiometerIgnored[index] = ComparePotentiometers(potentiometers[index], potentiometersWhenIgnored[index]);
    } else {
        synthesiserParameters->lfoAmplitude = CUBE(2.0f * (potentiometers[index] - 0.5f)) * (MAXIMUM_VCO_FREQUENCY / 2.0f);
    }
    float absLfoAmplitude = fabs(synthesiserParameters->lfoAmplitude);
    if ((synthesiserParameters->vcoFrequency - absLfoAmplitude) < MINIMUM_VCO_FREQUENCY) {
        absLfoAmplitude = synthesiserParameters->vcoFrequency - MINIMUM_VCO_FREQUENCY;
    }
    if ((synthesiserParameters->vcoFrequency + absLfoAmplitude) > MAXIMUM_VCO_FREQUENCY) {
        absLfoAmplitude = MAXIMUM_VCO_FREQUENCY - synthesiserParameters->vcoFrequency;
    }
    synthesiserParameters->lfoAmplitude = copysignf(absLfoAmplitude, synthesiserParameters->lfoAmplitude);

    // VCO waveform
    index = PotentiometerIndexVcoWaveform;
    if (potentiometerIgnored[index] == true) {
        potentiometerIgnored[index] = ComparePotentiometers(potentiometers[index], potentiometersWhenIgnored[index]);
    } else {
        static int validValue = -1;
        const int currentValue = InterpretDiscretePotentiometer(potentiometers[index], VcoWaveformNumberOfWaveforms, validValue != -1);
        if (currentValue != -1) {
            validValue = currentValue;
        }
        synthesiserParameters->vcoWaveform = (VcoWaveform) validValue;
    }

    // Delay time
    synthesiserParameters->delayTime = potentiometers[PotentiometerIndexDelayTime] * 1.333333f;

    // Delay feedback
    synthesiserParameters->delayFeedback = potentiometers[PotentiometerIndexDelayFeedback];

    // Delay filter type
    if (potentiometers[PotentiometerIndexDelayFilter] < (synthesiserParameters->delayFilterType == DelayFilterTypeLowPass ? 0.475f : 0.45f)) {
        synthesiserParameters->delayFilterType = DelayFilterTypeLowPass;
    } else if (potentiometers[PotentiometerIndexDelayFilter] > (synthesiserParameters->delayFilterType == DelayFilterTypeHighPass ? 0.525f : 0.55f)) {
        synthesiserParameters->delayFilterType = DelayFilterTypeHighPass;
    } else {
        synthesiserParameters->delayFilterType = DelayFilterTypeNone;
    }

    // Delay filter frequency
    if (potentiometers[PotentiometerIndexDelayFilter] < 0.5f) {
        const float normalisedPotentiometer = MAP(potentiometers[PotentiometerIndexDelayFilter], 0.0f, 0.5f, 0.0f, 1.0f);
        synthesiserParameters->delayFilterFrequency = MAP(CUBE(normalisedPotentiometer), 0.0f, 1.0f, 100.0f, 20000.0f);
    } else {
        const float normalisedPotentiometer = MAP(potentiometers[PotentiometerIndexDelayFilter], 0.5f, 1.0f, 0.0f, 1.0f);
        synthesiserParameters->delayFilterFrequency = MAP(CUBE(normalisedPotentiometer), 0.0f, 1.0f, 1.0f, 5000.0f);
    }
}

/**
 * @brief Returns true if the potentiometer values are approximately the same.
 * @param potentiometerA Potentiometer value to be compared.
 * @param potentiometerB Potentiometer value to be compared.
 * @return True if the potentiometer values are approximately the same.
 */
static bool ComparePotentiometers(const float potentiometerA, const float potentiometerB) {
    const float delta = 0.05f;
    if (potentiometerA > (potentiometerB + delta)) {
        return false;
    }
    if (potentiometerA < (potentiometerB - delta)) {

        return false;
    }
    return true;
}

/**
 * @brief Returns the discrete value of potentiometer for a specified number of
 * discrete values over the full potentiometer range.  A value of -1 will be
 * returned if the potentiometer is in the deadband between any two adjacent
 * discrete values.
 * @param potentiometer Potentiometer value between 0.0 and 1.0.
 * @param numberOfValues Number of discrete values over potentiometer range.
 * @param omitDeadbands True to use deadbands.
 * @return Discrete value of potentiometer.
 */
static int InterpretDiscretePotentiometer(const float potentiometer, const unsigned int numberOfValues, const bool useDeadbands) {
    if (useDeadbands == false) {
        return (int) ROUND(potentiometer * (float) (numberOfValues - 1));
    }
    const unsigned int numberOfValuesIncludingDeadbands = numberOfValues + numberOfValues - 1;
    const unsigned int valueIncludingDeadbands = (unsigned int) ROUND(potentiometer * (float) (numberOfValuesIncludingDeadbands - 1));
    if (valueIncludingDeadbands & 0b1) { // if value is odd
        return -1;
    } else {

        return (int) (valueIncludingDeadbands >> 1);
    }
}

/**
 * @brief Prints synthesiser parameters to the UART.
 * @param synthesiserParameters Synthesiser parameters to be printed.
 */
static void PrintSynthesiserParameters(const SynthesiserParameters * const synthesiserParameters) {
    char string[512];
    snprintf(string, sizeof (string),
            "\r\n"
            "TRIGGERED:\r\n"
            "lfoWaveform            = %i\r\n"
            "lfoShape               = %f\r\n"
            "lfoFrequency           = %f\r\n"
            "lfoAmplitude           = %f\r\n"
            "lfoGateControl         = %s\r\n"
            "vcoWaveform            = %i\r\n"
            "vcoFrequency           = %f\r\n"
            "delayTime              = %f\r\n"
            "delayFeedback          = %f\r\n"
            "delayFilterType        = %i\r\n"
            "delayFilterFrequency   = %f\r\n"
            ,
            synthesiserParameters->lfoWaveform,
            (double) synthesiserParameters->lfoShape,
            (double) synthesiserParameters->lfoFrequency,
            (double) synthesiserParameters->lfoAmplitude,
            synthesiserParameters->lfoGateControl == true ? "true" : "false",
            synthesiserParameters->vcoWaveform,
            (double) synthesiserParameters->vcoFrequency,
            (double) synthesiserParameters->delayTime,
            (double) synthesiserParameters->delayFeedback,
            synthesiserParameters->delayFilterType,
            (double) synthesiserParameters->delayFilterFrequency
            );
    Uart1WriteStringIfReady(string);
}

//------------------------------------------------------------------------------
// End of file
