/**
 * @file Main.c
 * @author Seb Madgwick
 * @brief Firmware main file.
 *
 * Device:
 * PIC32MZ2048EFH064
 *
 * Compiler:
 * XC32 v1.43 (using Legacy libc)
 */

//------------------------------------------------------------------------------
// Includes

#include "FirmwareVersion.h"
#include "IODefinitions.h"
#include <stdbool.h>
#include <stddef.h> // NULL
#include "Synthesiser/Synthesiser.h"
#include "system/common/sys_module.h" // SYS_Initialize
#include "Timer/Timer.h"
#include "Uart/Uart1.h"
#include "UserInterface/UserInterface.h"
#include <xc.h>

//------------------------------------------------------------------------------
// Function prototypes

static void Initialise();

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Firmware entry point.
 * @return This function should never return.
 */
void main() {

    Initialise();

    TimerInitialise();

    Uart1Initialise(&defaultUartSettings);
    TimerDelay(1); // wait else first data may be corrupted
    Uart1WriteStringIfReady(
            "\r\n"
            "FIRMWARE VERSION:\r\n"
            FIRMWARE_VERSION
            "\r\n");

    SynthesiserInitialise();

    UserInterfaceInitialise();

    // Main program loop
    while (true) {
        UserInterfaceDoTasks();
    }
}

/**
 * @brief Initialise CPU and I/O.
 */
static void Initialise() {

    // Configure system clock and enable interrupts using MPLAB Harmony
    SYS_Initialize(NULL);

    // Disable all analogue inputs
    ANSELB = 0x00000000;
    ANSELE = 0x00000000;
    ANSELG = 0x00000000;

    // Enable all pull-downs
    CNPDB = 0xFFFFFFFF;
    CNPDC = 0xFFFFFFFF;
    CNPDD = 0xFFFFFFFF;
    CNPDE = 0xFFFFFFFF;
    CNPDF = 0xFFFFFFFF;
    CNPDG = 0xFFFFFFFF;

    // Configure analogue inputs I/O
    POTENTIOMETER_1_ANSEL = 1;
    POTENTIOMETER_1_CNPD = 0;
    POTENTIOMETER_2_ANSEL = 1;
    POTENTIOMETER_2_CNPD = 0;
    POTENTIOMETER_3_ANSEL = 1;
    POTENTIOMETER_3_CNPD = 0;
    POTENTIOMETER_4_ANSEL = 1;
    POTENTIOMETER_4_CNPD = 0;
    POTENTIOMETER_5_ANSEL = 1;
    POTENTIOMETER_5_CNPD = 0;
    POTENTIOMETER_6_ANSEL = 1;
    POTENTIOMETER_6_CNPD = 0;
    POTENTIOMETER_7_ANSEL = 1;
    POTENTIOMETER_7_CNPD = 0;
    POTENTIOMETER_8_ANSEL = 1;
    POTENTIOMETER_8_CNPD = 0;
    POTENTIOMETER_9_ANSEL = 1;
    POTENTIOMETER_9_CNPD = 0;

    // Configure LED I/O
    LFO_GATE_CONTROL_LED_TRIS = 0;
    GATE_LED_TRIS = 0;

    // Configure DAC I/O
    DAC_REFCLKO_MAP();
    DAC_SDO_MAP();
    DAC_SS_MAP();

    // Configure EEPROM I/O
    EEPROM_SCL_TRIS = 0;
    EEPROM_SCL_LAT = 1;
    EEPROM_SCL_ODC = 1;
    EEPROM_SCL_CNPD = 0;
    EEPROM_SDA_TRIS = 0;
    EEPROM_SDA_LAT = 1;
    EEPROM_SDA_ODC = 1;
    EEPROM_SDA_CNPD = 0;

    // Configure UART I/O
    UART_RX_MAP();
    UART_TX_MAP();
    UART_RTS_MAP();
    UART_CTS_MAP();
}

//------------------------------------------------------------------------------
// End of file
