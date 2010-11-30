/*
 * rs485.h
 *
 *  Created on: 24.11.2010
 *      Author: sven
 */

#ifndef RS485_H_
#define RS485_H_

#include "../../conf/controller.h"
#include "protocol.h"
#include <avr/io.h>

/*
 * Configuration
 */
#define	RS485_MAX_TX_RETRIES					3		/* Maximum number of retries when sending a packet */
#define RS485_RX_PACKAGE_BUFFER_SIZE			3		/* Maximum number of received packages that can wait in the buffer */

// USART driver enable pin
#define RS485_USART_DE_DDR			DDRB
#define	RS485_USART_DE_PORT			PORTB
#define RS485_USART_DE_PIN			PB0

// USART interrupt vectors
#define RS485_USART_RXIR			USART_RXC_vect
#define RS485_USART_TXIR			USART_TXC_vect
#define RS485_USART_UDRIR			USART_UDRE_vect

// USART registers used
#define RS485_USART_UCSRA			defUCSRA	// Configuration register A
#define RS485_USART_UCSRB			defUCSRB	// Configuration register B
#define RS485_USART_UCSRC			defUCSRC	// Configuration register C
#define RS485_USART_UDR				defUDR		// Data register
#define RS485_USART_UBBRH			defUBRRH	// Baudrate register H
#define RS485_USART_UBBRL			defUBRRL	// Baudrate register L

// Register bit defines
// UCSRA
#define RS485_USART_UDRE			defUDRE 	// Read empty
#define RS485_USART_PE				defPE		// Parity error
#define RS485_USART_FE				defFE		// Frame error
#define RS485_USART_DOR				defDOR		// Data overrun
#define RS485_USART_U2X				defU2X		// Highspeed
#define RS485_USART_MPCM			defMPCM		// Multiprocessor

// UCSRB
#define RS485_USART_RXCIE					defRXCIE	// RX interrupt enable
#define RS485_USART_TXCIE					defTXCIE	// TX interrupt enable
#define RS485_USART_UDRIE					defUDRIE	// UDR empty interrupt enable
#define RS485_USART_TXEN					defTXEN		// TX enable
#define RS485_USART_RXEN					defRXEN		// RX enable
#define RS485_USART_TXB8					defTXB8		// TX ninth bit
#define RS485_USART_RXB8					defRXB8		// RX ninth bit

// UCSRC
#define RS485_USART_URSEL					defURSEL	// Write to UCSRC
#define RS485_USART_UMSEL					defUMSEL	// 0 => Asynchronous
#define RS485_USART_UPM0					defUPM0		// 0 => Even parity
#define RS485_USART_UPM1					defUPM1 	// 1
#define RS485_USART_USBS					defUSBS		// 0 => 1 stop bit
#define RS485_USART_UCSZ0					defUCSZ0	// 1 => 9-bit
#define RS485_USART_UCSZ1					defUCSZ1	// 1
#define RS485_USART_UCSZ2					defUCSZ2	// 1


#endif /* RS485_H_ */
