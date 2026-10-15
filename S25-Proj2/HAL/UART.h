/*
 * UART.h
 *
 *  Created on: Dec 31, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#ifndef HAL_UART_H_
#define HAL_UART_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

// Ever since the API update in mid 2019, the UART configuration struct's
// name changed from [eUSCI_UART_Config] to [eUSCI_UART_ConfigV1]. If your code
// does not compile, uncomment the alternate #define and comment the original
// one. This should not really matter anymore
// -----------------------------------------------------------------------------
// #define UART_Config eUSCI_UART_Config
#define UART_Config eUSCI_UART_ConfigV1

// TODO: define these macros based on the UART module that is connected to USB
// on Launchpad via XDS110

#define USB_UART_PORT   GPIO_PORT_P1
#define USB_UART_PINS (GPIO_PIN2 | GPIO_PIN3)  // The pins are given to you for guidance. Also,
                           // because many students miss the parentheses
#define USB_UART_INSTANCE EUSCI_A0_BASE

// An enum outlining what baud rates the UART_construct() function can use in
// its initialization.
enum _UART_Baudrate {
  BAUD_9600,
  BAUD_19200,
  BAUD_38400,
  BAUD_57600,
  NUM_BAUD_CHOICES
};
typedef enum _UART_Baudrate UART_Baudrate;


// TODO: Write an overview explanation of what this UART struct does, and how it
//       interacts with the functions below. Consult <HAL/Button.h> and
//       <HAL/LED.h> for examples on how to do this.
/**============================================================================
 * UART Struct - Stores configuration and hardware information for a UART instance.
 * ============================================================================
 * This struct contains all the information needed to set up and use UART
 * communication on the MSP432. It stores:
 *
 * - `config`: Stores settings like baud rate, data bits, and parity.
 * - `moduleInstance`: Identifies which UART module is used (e.g., `EUSCI_A0_BASE`).
 * - `port`: Specifies which GPIO port the UART TX/RX pins belong to.
 * - `pins`: Specifies which TX and RX pins are used for communication.
 *
 * ============================================================================
 * USAGE WARNINGS
 * ============================================================================
 * - A UART struct must be initialized using `UART_construct()` before use.
 * - This struct should be passed to all UART functions (e.g., `UART_sendChar()`).
 * - Modifying this struct manually is not recommended. Use provided functions.
 */


struct _UART {
  UART_Config config;

  uint32_t moduleInstance;
  uint32_t port;
  uint32_t pins;
};
typedef struct _UART UART;

// Constructs a uart using the moduleInstance at the given port and pin
UART UART_construct(uint32_t moduleInstance, uint32_t port, uint32_t pins);

// Sets the baudrate and enables a UART based on the given baudrate enum
void UART_SetBaud_Enable(UART*, UART_Baudrate baudrate);

// TODO: Write a comment which explains what each of these functions do. In
// the
//       header, prefer explaining WHAT the function does, as opposed to HOW it
//       is implemented.
bool UART_hasChar(UART* uart_p); //Checks if there is a new character available in the UART receive buffer
char UART_getChar(UART* uart_p); //Retrieves the next char from the UART receiver buffer, if no char available, behavior depends on implementation
bool UART_canSend(UART* uart_p); //Checks if the UART transmit bugger is free, can accept a new char to send
void UART_sendChar(UART* uart_p, char c); //places a char into the UART transmit buffer for sending, if buffer full, behavior depends on implementation

// Updates the UART baudrate to use the new baud choice.
void UART_updateBaud(UART* uart_p, UART_Baudrate baudChoice);
//UART uart;

// Initialize UART (using proper port and pin information)
//uart = UART_construct(EUSCI_A0_BASE, GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3);
//void UART_init(UART *uart);
#endif /* HAL_UART_H_ */

