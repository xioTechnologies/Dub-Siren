/**
 * @file Uart1.c
 * @author Seb Madgwick
 * @brief UART library for PIC32MZ.
 *
 * Transmission and reception are interrupt-driven to empty or fill software
 * buffers in the background.
 *
 * UART1_BUFFER_SIZE and U1_INTERRUPT_PRIORITY may be changed as required by the
 * application.
 */

//------------------------------------------------------------------------------
// Includes

#include "system/int/sys_int.h"
#include "Uart1.h"
#include <xc.h>

//------------------------------------------------------------------------------
// Definitions

#define INTERRUPT_PRIORITY (INT_PRIORITY_LEVEL3)

//------------------------------------------------------------------------------
// Variables

static volatile char rxBuffer[UART1_BUFFER_SIZE];
static volatile unsigned int rxBufferIn = 0; // only written to by interrupt
static volatile unsigned int rxBufferOut = 0;
static volatile bool rxBufferOverrun = false;
static volatile char txBuffer[UART1_BUFFER_SIZE];
static volatile unsigned int txBufferIn = 0;
static volatile unsigned int txBufferOut = 0; // only written to by interrupt

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the UART module.
 *
 * Initialises the UART module with specified UART settings. This function can
 * be used to reinitialise the module with new settings if it has already been
 * initialised.
 *
 * @param uartSettings UartSettings structure.
 */
void Uart1Initialise(const UartSettings * const uartSettings) {

    // Ensure default register states
    Uart1Disable();

    // Configure module
    if (uartSettings->ctsRtsEnabled == true) {
        U1MODEbits.UEN = 0b10; // UxTX, UxRX, UxCTS and UxRTS pins are enabled and used
    }
    if (uartSettings->invertDataLines == true) {
        U1MODEbits.RXINV = 1; // UxRX Idle state is '0'
        U1STAbits.UTXINV = 1; // UxTX Idle state is '0'
    }
    U1MODEbits.PDSEL = uartSettings->parityAndData;
    U1MODEbits.STSEL = uartSettings->stopBits;
    U1MODEbits.BRGH = 1; // High-Speed mode – 4x baud clock enabled
    U1STAbits.UTXISEL = 0b10; // Interrupt is generated when the transmit buffer becomes empty
    U1STAbits.URXEN = 1; // UARTx receiver is enabled. UxRX pin is controlled by UARTx (if ON = 1)
    U1STAbits.UTXEN = 1; // UARTx transmitter is enabled. UxTX pin is controlled by UARTx (if ON = 1)
    U1BRG = CALCULATE_UXBRG(uartSettings->baudRate);
    U1MODEbits.ON = 1; // UARTx is enabled. UARTx pins are controlled by UARTx as defined by UEN<1:0> and UTXEN control bits
    SYS_INT_VectorPrioritySet(INT_VECTOR_UART1_RX, INTERRUPT_PRIORITY); // set TX/RX interrupt priority
    SYS_INT_VectorPrioritySet(INT_VECTOR_UART1_TX, INTERRUPT_PRIORITY); // set TX/RX interrupt priority
    SYS_INT_SourceEnable(INT_SOURCE_USART_1_RECEIVE); // enable RX interrupt
}

/**
 * @brief Disable the UART module.
 *
 * UART registers are set to their default values.  UART pins are controlled by
 * corresponding bits in  the PORTx, TRISx and LATx registers.  Power
 * consumption is minimal.
 */
void Uart1Disable() {

    // Disable module and ensure default register states
    U1MODECLR = 0xFFFFFFFF;
    U1STACLR = 0xFFFFFFFF;
    SYS_INT_SourceDisable(INT_SOURCE_USART_1_RECEIVE); // disable RX interrupt
    SYS_INT_SourceDisable(INT_SOURCE_USART_1_TRANSMIT); // disable TX interrupt
    SYS_INT_SourceStatusClear(INT_SOURCE_USART_1_RECEIVE); // clear RX interrupt flag
    SYS_INT_SourceStatusClear(INT_SOURCE_USART_1_TRANSMIT); // clear TX interrupt flag

    // Clear software buffers
    Uart1ClearRxBuffer();
    Uart1ClearTxBuffer();
}

/**
 * @brief Returns the number of bytes available in the software receive buffer.
 *
 * This function also polls the RX hardware buffer and triggers and 'manually'
 * triggers an interrupt to fetch unprocessed bytes.  If the RX hardware buffer
 * overrun flag is set, it will be cleared to re-enabled the UART module.
 * Uart1GetRxBufferOverrunFlag may be used to determine if a hardware or
 * software buffer receive buffer overrun has occurred.
 *
 * @return Number of bytes available in software receive buffer.
 */
size_t Uart1IsGetReady() {
    if (U1STAbits.URXDA == 1) { // trigger interrupt if data available
        SYS_INT_SourceStatusSet(INT_SOURCE_USART_1_RECEIVE);
    }
    if (U1STAbits.OERR == 1) {
        U1STAbits.OERR = 0; // clear flag and re-enable UART if hardware buffer overrun
        rxBufferOverrun = true;
    }
    return (rxBufferIn - rxBufferOut) & (UART1_BUFFER_SIZE - 1);
}

/**
 * @brief Returns next byte available in the software receive buffer.
 *
 * This function should only be called if bytes are available.  Call
 * Uart1IsGetReady to determine the number of bytes in the software receive
 * buffer.
 *
 * @return Next byte available in the software receive buffer.
 */
char Uart1GetChar() {
    const char byte = rxBuffer[rxBufferOut++];
    rxBufferOut &= (UART1_BUFFER_SIZE - 1); // overflow index at buffer size
    return byte;
}

/**
 * @brief Returns the space available in the software transmit buffer.
 * @return Space available (number of bytes) in the software transmit buffer.
 */
size_t Uart1IsPutReady() {
    return (UART1_BUFFER_SIZE - 1) - ((txBufferIn - txBufferOut) & (UART1_BUFFER_SIZE - 1));
}

/**
 * @brief Loads byte into the software transmit buffer and starts interrupt-
 * driven transmission.
 *
 * This function should only be called if space is available.  Call
 * Uart1IsPutReady to determine the number of bytes that may be loaded into the
 * software transmit buffer.
 *
 * @param byte Byte to be buffered for transmission.
 */
void Uart1PutChar(const char byte) {
    txBuffer[txBufferIn++] = byte;
    txBufferIn &= (UART1_BUFFER_SIZE - 1); // overflow index at buffer size
    if (SYS_INT_SourceIsEnabled(INT_SOURCE_USART_1_TRANSMIT) == false) {
        SYS_INT_SourceStatusSet(INT_SOURCE_USART_1_TRANSMIT); // set TX interrupt flag
        SYS_INT_SourceEnable(INT_SOURCE_USART_1_TRANSMIT); // enable TX interrupt
    }
}

/**
 * @brief Loads byte array into software transmit buffer starts interrupt-driven
 * transmission.
 *
 * This function should only be called if space is available.  Call
 * Uart1IsPutReady to determine the number of bytes that may be loaded into the
 * software transmit buffer.
 *
 * @param source Address of byte array.
 * @param numberOfBytes Number of bytes in byte array.
 */
void Uart1PutCharArray(const char* const source, const size_t numberOfBytes) {
    int i;
    for (i = 0; i < numberOfBytes; i++) {
        Uart1PutChar(source[i]);
    }
}

/**
 * @brief Loads string into software transmit buffer starts interrupt-driven
 * transmission.
 *
 * This function should only be called if space is available.  Call
 * Uart1IsPutReady to determine the number of bytes that may be loaded into the
 * software transmit buffer.  The terminating null character will not be sent.
 *
 * @param string String to transmit.
 */
void Uart1PutString(const char* string) {
    while (*string != '\0') {
        Uart1PutChar(*string++);
    }
}

/**
 * @brief Clears software receive buffer and receive buffer overrun flag.
 */
void Uart1ClearRxBuffer() {
    rxBufferOut = rxBufferIn;
    rxBufferOverrun = false;
}

/**
 * @brief Clears transmit receive buffer.
 */
void Uart1ClearTxBuffer() {
    txBufferIn = txBufferOut;
}

/**
 * @brief Returns receive buffer overrun flag.
 *
 * The receive buffer overrun flag indicates if either a hardware or software
 * receive buffer overrun has occurred.  If either receive buffer has overrun
 * then one or more bytes will have been discarded.  This flag must be cleared
 * using Uart1ClearRxBufferOverrunFlag.
 *
 * @return Receive buffer overrun flag.
 */
bool Uart1GetRxBufferOverrunFlag() {
    return rxBufferOverrun;
}

/**
 * @brief Clears receive buffer overrun flag.
 *
 * The receive buffer overrun flag indicates if either a hardware or software
 * receive buffer overrun has occurred.  If either receive buffer has overrun
 * then one or more bytes will have been discarded.
 *
 * @return Receive buffer overrun flag.
 */
void Uart1ClearRxBufferOverrunFlag() {
    rxBufferOverrun = false;
}

/**
 * @brief Returns true if interrupt-driven transmission is complete.
 * @return true if interrupt-driven transmission is complete.
 */
bool Uart1TxIsIdle() {
    return IEC3bits.U1TXIE == 0;
}

/**
 * UART RX interrupt service routine.
 */
void __ISR(_UART1_RX_VECTOR) Uart1RxInterrupt() {
    while (U1STAbits.URXDA == 1) { // repeat while data available
        const char newByte = U1RXREG; // fetch byte from hardware buffer
        if (rxBufferIn == (rxBufferOut - 1)) {
            rxBufferOverrun = true; // set flag and discard byte if rxBuffer overrun
        } else {
            rxBuffer[rxBufferIn++] = newByte; // add to buffer and increment index
            rxBufferIn &= (UART1_BUFFER_SIZE - 1); // overflow index at buffer size
        }
    }
    SYS_INT_SourceStatusClear(INT_SOURCE_USART_1_RECEIVE); // clear RX interrupt flag
}

/**
 * UART TX interrupt service routine.
 */
void __ISR(_UART1_TX_VECTOR) Uart1TxInterrupt() {
    SYS_INT_SourceDisable(INT_SOURCE_USART_1_TRANSMIT); // disable TX interrupt to avoid nested interrupt
    SYS_INT_SourceStatusClear(INT_SOURCE_USART_1_TRANSMIT); // clear TX interrupt flag
    while (U1STAbits.UTXBF == 0) { // repeat while hardware buffer not full
        if (txBufferOut == txBufferIn) { // if txBuffer empty
            return;
        }
        U1TXREG = txBuffer[txBufferOut++]; // send data to hardware buffer and increment index
        txBufferOut &= (UART1_BUFFER_SIZE - 1); // overflow index at buffer size
    }
    SYS_INT_SourceEnable(INT_SOURCE_USART_1_TRANSMIT); // re-enable TX interrupt
}

//------------------------------------------------------------------------------
// End of file
