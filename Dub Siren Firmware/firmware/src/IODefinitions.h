/**
 * @file IODefinitions.h
 * @author Seb Madgwick
 * @brief I/O definitions.
 */

#ifndef IO_DEFINITIONS_H
#define IO_DEFINITIONS_H

//------------------------------------------------------------------------------
// Includes

#include <xc.h>

//------------------------------------------------------------------------------
// Definitions

// Analogue inputs
#define POTENTIOMETER_1_ANSEL               ANSELBbits.ANSB7 /* AN47 */
#define POTENTIOMETER_1_CNPD                CNPDBbits.CNPDB7
#define POTENTIOMETER_2_ANSEL               ANSELBbits.ANSB9 /* AN49 */
#define POTENTIOMETER_2_CNPD                CNPDBbits.CNPDB9
#define POTENTIOMETER_3_ANSEL               ANSELBbits.ANSB5 /* AN45 */
#define POTENTIOMETER_3_CNPD                CNPDBbits.CNPDB5
#define POTENTIOMETER_4_ANSEL               ANSELBbits.ANSB6 /* AN46 */
#define POTENTIOMETER_4_CNPD                CNPDBbits.CNPDB6
#define POTENTIOMETER_5_ANSEL               ANSELGbits.ANSG8 /* AN12 */
#define POTENTIOMETER_5_CNPD                CNPDGbits.CNPDG8
#define POTENTIOMETER_6_ANSEL               ANSELGbits.ANSG9 /* AN11 */
#define POTENTIOMETER_6_CNPD                CNPDGbits.CNPDG9
#define POTENTIOMETER_7_ANSEL               ANSELEbits.ANSE7 /* AN15 */
#define POTENTIOMETER_7_CNPD                CNPDEbits.CNPDE7
#define POTENTIOMETER_8_ANSEL               ANSELEbits.ANSE6 /* AN16 */
#define POTENTIOMETER_8_CNPD                CNPDEbits.CNPDE6
#define POTENTIOMETER_9_ANSEL               ANSELEbits.ANSE5 /* AN17 */
#define POTENTIOMETER_9_CNPD                CNPDEbits.CNPDE5

// Buttons
#define TRIGGER_SAVE_BUTTON_PORT            PORTB
#define TRIGGER_SAVE_BUTTON_PORT_BIT        (13)
#define LFO_GATE_CONTROL_BUTTON_PORT        PORTB
#define LFO_GATE_CONTROL_BUTTON_PORT_BIT    (12)
#define GATE_BUTTON_PORT                    PORTG
#define GATE_BUTTON_PORT_BIT                (7)
#define PRESET_KEY_1_PORT                   PORTB
#define PRESET_KEY_1_PORT_BIT               (14)
#define PRESET_KEY_2_PORT                   PORTF
#define PRESET_KEY_2_PORT_BIT               (3)
#define PRESET_KEY_3_PORT                   PORTF
#define PRESET_KEY_3_PORT_BIT               (4)
#define PRESET_KEY_4_PORT                   PORTC
#define PRESET_KEY_4_PORT_BIT               (14)
#define PRESET_KEY_5_PORT                   PORTE
#define PRESET_KEY_5_PORT_BIT               (4)
#define PRESET_KEY_6_PORT                   PORTB
#define PRESET_KEY_6_PORT_BIT               (15)
#define PRESET_KEY_7_PORT                   PORTF
#define PRESET_KEY_7_PORT_BIT               (5)
#define PRESET_KEY_8_PORT                   PORTD
#define PRESET_KEY_8_PORT_BIT               (11)
#define PRESET_KEY_9_PORT                   PORTD
#define PRESET_KEY_9_PORT_BIT               (0)
#define PRESET_KEY_10_PORT                  PORTE
#define PRESET_KEY_10_PORT_BIT              (3)

// LEDs
#define LFO_GATE_CONTROL_LED_TRIS           TRISBbits.TRISB11
#define LFO_GATE_CONTROL_LED_LAT            LATBbits.LATB11
#define GATE_LED_TRIS                       TRISGbits.TRISG6
#define GATE_LED_LAT                        LATGbits.LATG6

// DAC
#define DAC_REFCLKO_MAP()                   RPD3Rbits.RPD3R = 0b1111
#define DAC_SDO_MAP()                       RPC13Rbits.RPC13R = 0b0101
#define DAC_SS_MAP()                        RPD4Rbits.RPD4R = 0b0101

// EEPROM
#define EEPROM_SCL_TRIS                     TRISDbits.TRISD10
#define EEPROM_SCL_LAT                      LATDbits.LATD10
#define EEPROM_SCL_ODC                      ODCDbits.ODCD10
#define EEPROM_SCL_CNPD                     CNPDDbits.CNPDD10
#define EEPROM_SDA_TRIS                     TRISDbits.TRISD9
#define EEPROM_SDA_PORT                     PORTDbits.RD9
#define EEPROM_SDA_LAT                      LATDbits.LATD9
#define EEPROM_SDA_ODC                      ODCDbits.ODCD9
#define EEPROM_SDA_CNPD                     CNPDDbits.CNPDD9

// UART
#define UART_RX_MAP()                       U1RXRbits.U1RXR = 0b1010
#define UART_TX_MAP()                       RPB3Rbits.RPB3R = 0b0001
#define UART_RTS_MAP()                      RPB2Rbits.RPB2R = 0b0001
#define UART_CTS_MAP()                      U1CTSRbits.U1CTSR = 0b0010

// Test pin
#define TEST_PIN_TRIS                       TRISBbits.TRISB4
#define TEST_PIN_LAT                        LATBbits.LATB4

#endif

//------------------------------------------------------------------------------
// End of file
