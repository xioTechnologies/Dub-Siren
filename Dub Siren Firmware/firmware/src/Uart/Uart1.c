/**
 * @file Uart1.c
 * @author Seb Madgwick
 * @brief UART library for PIC32MZ.
 */

//------------------------------------------------------------------------------
// Includes

#include <string.h> // strlen
#include "system/int/sys_int.h"
#include "Uart1.h"
#include <xc.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Read and write buffers size in number of bytes.  Must be a 2^n number,
 * e.g. 256, 512, 1024, 2048, 4096, etc.
 */
#define READ_WRITE_BUFFER_SIZE (4096)

/**
 * @brief Read and write buffers index mask.  This value is bitwise anded with
 * buffer indexes for optimised overflow calculations.
 */
#define READ_WRITE_BUFFER_INDEX_BIT_MASK (READ_WRITE_BUFFER_SIZE - 1)

/**
 * @brief TX/RX interrupt priority.
 */
#define INTERRUPT_PRIORITY (INT_PRIORITY_LEVEL4)

//------------------------------------------------------------------------------
// Function prototypes

static void TriggerTransmission();

//------------------------------------------------------------------------------
// Variables

static volatile bool readBufferOverrun = false;
static volatile char readBuffer[READ_WRITE_BUFFER_SIZE];
static volatile unsigned int readBufferInIndex = 0; // only written to by interrupt
static volatile unsigned int readBufferOutIndex = 0;
static volatile char writeBuffer[READ_WRITE_BUFFER_SIZE];
static volatile unsigned int writeBufferInIndex = 0;
static volatile unsigned int writeBufferOutIndex = 0; // only written to by interrupt

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the UART module with specified settings.
 * @param uartSettings UART settings.
 */
void Uart1Initialise(const UartSettings * const uartSettings) {

    // Ensure default register states
    Uart1Disable();

    // Configure UART
    if (uartSettings->ctsRtsEnabled == true) {
        U1MODEbits.UEN = 0b10; // UxTX, UxRX, UxCTS and UxRTS pins are enabled and used
    }
    if (uartSettings->invertDataLines == true) {
        U1MODEbits.RXINV = 1; // UxRX Idle state is '0'
        U1STAbits.UTXINV = 1; // UxTX Idle state is '0'
    }
    U1MODEbits.PDSEL = uartSettings->parityAndData;
    U1MODEbits.STSEL = uartSettings->stopBits;
    U1MODEbits.BRGH = 1; // High-Speed mode - 4x baud clock enabled
    U1STAbits.UTXISEL = 0b10; // Interrupt is generated when the transmit buffer becomes empty
    U1STAbits.URXEN = 1; // UARTx receiver is enabled. UxRX pin is controlled by UARTx (if ON = 1)
    U1STAbits.UTXEN = 1; // UARTx transmitter is enabled. UxTX pin is controlled by UARTx (if ON = 1)
    U1BRG = UartCalculateUxbrg(uartSettings->baudRate);
    U1MODEbits.ON = 1; // UARTx is enabled. UARTx pins are controlled by UARTx as defined by UEN<1:0> and UTXEN control bits

    // Configure interrupt
    SYS_INT_VectorPrioritySet(INT_VECTOR_UART1_RX, INTERRUPT_PRIORITY); // set RX interrupt priority
    SYS_INT_VectorPrioritySet(INT_VECTOR_UART1_TX, INTERRUPT_PRIORITY); // set TX interrupt priority
    SYS_INT_SourceEnable(INT_SOURCE_USART_1_RECEIVE); // enable RX interrupt
}

/**
 * @brief Disables the UART module.
 */
void Uart1Disable() {

    // Disable module and restore default register states
    U1MODECLR = 0xFFFFFFFF;
    U1STACLR = 0xFFFFFFFF;

    // Disable interrupts
    SYS_INT_SourceDisable(INT_SOURCE_USART_1_RECEIVE); // disable RX interrupt
    SYS_INT_SourceDisable(INT_SOURCE_USART_1_TRANSMIT); // disable TX interrupt
    SYS_INT_SourceStatusClear(INT_SOURCE_USART_1_RECEIVE); // clear RX interrupt flag
    SYS_INT_SourceStatusClear(INT_SOURCE_USART_1_TRANSMIT); // clear TX interrupt flag

    // Clear buffers
    Uart1ClearReadBuffer();
    Uart1ClearWriteBuffer();
}

/**
 * @brief Returns the number of bytes available to read from the read buffer.
 * @return Number of bytes available to read from the read buffer.
 */
size_t Uart1IsReadReady() {

    // Trigger interrupt if hardware receive buffer not empty
    if (U1STAbits.URXDA == 1) {
        SYS_INT_SourceStatusSet(INT_SOURCE_USART_1_RECEIVE);
    }

    // Clear hardware receive buffer overrun flag
    if (U1STAbits.OERR == 1) {
        U1STAbits.OERR = 0;
        readBufferOverrun = true;
    }

    // Return number of bytes
    return (readBufferInIndex - readBufferOutIndex) & READ_WRITE_BUFFER_INDEX_BIT_MASK;
}

/**
 * @brief Reads byte from read buffer.
 * @return Byte from read buffer.
 */
char Uart1Read() {
    const char byte = readBuffer[readBufferOutIndex++ & READ_WRITE_BUFFER_INDEX_BIT_MASK];
    return byte;
}

/**
 * @brief Returns the space available in the write buffer in number of bytes.
 * @return Space available in the write buffer in number of bytes.
 */
size_t Uart1IsWriteReady() {
    return (READ_WRITE_BUFFER_SIZE - 1) - ((writeBufferInIndex - writeBufferOutIndex) & READ_WRITE_BUFFER_INDEX_BIT_MASK);
}

/**
 * @brief Writes byte to write buffer.
 * @param numberOfBytes Number of bytes.
 */
void Uart1WriteChar(const char byte) {
    writeBuffer[writeBufferInIndex++ & READ_WRITE_BUFFER_INDEX_BIT_MASK] = byte;
    TriggerTransmission();
}

/**
 * @brief Writes byte to write buffer if enough space available.
 * @param numberOfBytes Number of bytes.
 */
void Uart1WriteCharIfReady(const char byte) {
    if (Uart1IsWriteReady() < 1) {
        return;
    }
    Uart1WriteChar(byte);
}

/**
 * @brief Writes byte array to write buffer.
 * @param source Data to write.
 * @param numberOfBytes Number of bytes.
 */
void Uart1WriteCharArray(const char* const source, const size_t numberOfBytes) {
    unsigned int index;
    for (index = 0; index < numberOfBytes; index++) {
        writeBuffer[writeBufferInIndex++ & READ_WRITE_BUFFER_INDEX_BIT_MASK] = source[index];
    }
    TriggerTransmission();
}

/**
 * @brief Writes byte array to write buffer if enough space available.
 * @param source Data to write.
 * @param numberOfBytes Number of bytes.
 */
void Uart1WriteCharArrayIfReady(const char* const source, const size_t numberOfBytes) {
    if (Uart1IsWriteReady() < numberOfBytes) {
        return;
    }
    Uart1WriteCharArray(source, numberOfBytes);
}

/**
 * @brief Writes string to write buffer.
 * @param string String to write.
 */
void Uart1WriteString(const char* string) {
    while (*string != '\0') {
        writeBuffer[writeBufferInIndex++ & READ_WRITE_BUFFER_INDEX_BIT_MASK] = *string++;
    }
    TriggerTransmission();
}

/**
 * @brief Writes string to write buffer if enough space available.
 * @param string String to write.
 */
void Uart1WriteStringIfReady(const char* string) {
    if (Uart1IsWriteReady() < strlen(string)) {
        return;
    }
    Uart1WriteString(string);
}

/**
 * @brief Triggers interrupt handled transmission of write buffer contents.
 */
static void TriggerTransmission() {
    if (SYS_INT_SourceIsEnabled(INT_SOURCE_USART_1_TRANSMIT) == false) {
        SYS_INT_SourceStatusSet(INT_SOURCE_USART_1_TRANSMIT); // set TX interrupt flag
        SYS_INT_SourceEnable(INT_SOURCE_USART_1_TRANSMIT); // enable TX interrupt
    }
}

/**
 * @brief Clears read buffer and read buffer overrun flag.
 */
void Uart1ClearReadBuffer() {
    readBufferOutIndex = readBufferInIndex & READ_WRITE_BUFFER_INDEX_BIT_MASK;
    readBufferOverrun = false;
}

/**
 * @brief Clears write buffer.
 */
void Uart1ClearWriteBuffer() {
    writeBufferInIndex = writeBufferOutIndex & READ_WRITE_BUFFER_INDEX_BIT_MASK;
}

/**
 * @brief Returns true if either a hardware or software buffer overrun has
 * occurred.
 * @return Read buffer overrun flag.
 */
bool Uart1GetReadBufferOverrunFlag() {
    return readBufferOverrun;
}

/**
 * @brief Clears read buffer overrun flag.
 */
void Uart1ClearReadBufferOverrunFlag() {
    readBufferOverrun = false;
}

/**
 * @brief Returns true if interrupt handled transmission has completed.
 * @return True if interrupt handled transmission has completed.
 */
bool Uart1IsTransmitionComplete() {
    return SYS_INT_SourceIsEnabled(INT_SOURCE_USART_1_TRANSMIT) == false;
}

/**
 * @brief UART RX interrupt.  Data received immediately before clearing the RX
 * interrupt flag will not be processed, URXDA must be polled to set the RX
 * interrupt flag.  This is done in Uart1IsReadReady.
 */
void __ISR(_UART1_RX_VECTOR) Uart1RxInterrupt() {
    while (U1STAbits.URXDA == 1) { // repeat while data available in receive buffer
        const char byte = U1RXREG;
        if (readBufferInIndex == ((readBufferOutIndex & READ_WRITE_BUFFER_INDEX_BIT_MASK) - 1)) {
            readBufferOverrun = true;
        } else {
            readBuffer[readBufferInIndex++ & READ_WRITE_BUFFER_INDEX_BIT_MASK] = byte;
        }
    }
    SYS_INT_SourceStatusClear(INT_SOURCE_USART_1_RECEIVE); // clear RX interrupt flag
}

/**
 * @brief UART TX interrupt.
 */
void __ISR(_UART1_TX_VECTOR) Uart1TxInterrupt() {
    SYS_INT_SourceDisable(INT_SOURCE_USART_1_TRANSMIT); // disable TX interrupt to avoid nested interrupt
    SYS_INT_SourceStatusClear(INT_SOURCE_USART_1_TRANSMIT); // clear TX interrupt flag
    while (U1STAbits.UTXBF == 0) { // repeat while transmit buffer not full
        if (((writeBufferOutIndex - writeBufferInIndex) & READ_WRITE_BUFFER_INDEX_BIT_MASK) == 0) { // if write buffer empty
            return;
        }
        U1TXREG = writeBuffer[writeBufferOutIndex++ & READ_WRITE_BUFFER_INDEX_BIT_MASK];
    }
    SYS_INT_SourceEnable(INT_SOURCE_USART_1_TRANSMIT); // re-enable TX interrupt
}

//------------------------------------------------------------------------------
// End of file
