/*
 * uart.c
 *
 *  Created on: 25.11.2010
 *      Author: sven
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rs485.h"
#include "protocol.h"
#include "../../conf/errors.h"
#include "../data/fifo.h"

#define BAUD 500000UL
//#define BAUD 115200UL      // Baudrate
//#define BAUD 9600UL
#include <util/setbaud.h>

FIFO* tx_fifo;

void rs485_uart_puts(uint8_t byte, uint8_t addr);

void rs485_uart_drive(uint8_t d) {
	if(d) {
		RS485_USART_DE_PORT |= (1<<RS485_USART_DE_PIN);
	} else {
		RS485_USART_DE_PORT &= ~(1<<RS485_USART_DE_PIN);
	}
	_delay_us(1);
}

/* Interrupt: Reception complete */
ISR(RS485_USART_RXIR) {
	/*if(RS485_USART_UCSRA & (1<<RS485_USART_PE)) {
		rs485_uart_receive_error(ERR_USART_PARITY_ERROR);
		return;
	}*/
	if(RS485_USART_UCSRA & (1<<RS485_USART_FE)) {
		rs485_uart_receive_error(ERR_USART_FRAME_ERROR);
		return;
	}
	if(RS485_USART_UCSRA & (1<<RS485_USART_DOR)) {
		rs485_uart_receive_error(ERR_USART_DATA_OVERRUN_ERROR);
		return;
	}

	uint8_t byte = RS485_USART_UDR;
	// TODO: Reimplement mpc
	//if(RS485_USART_UCSRB & (1<<RS485_USART_RXB8)) {
		/* Address byte */
	//	rs485_uart_address_received(byte);
	//} else {
		/* Data byte */
	//	rs485_uart_received(byte);
	//}
	if(rs485_is_receiving() || rs485_is_sending()) {
		rs485_uart_received(byte);
	} else {
		rs485_uart_address_received(byte);
	}
}

ISR(RS485_USART_TXIR) {
	if(!rs485_is_sending()) {
		rs485_uart_drive(0);
	}
	RS485_USART_UCSRB |= (1<<RS485_USART_RXEN);
}

/* Interrupt: UDR empty, transmit next */
ISR(RS485_USART_UDRIR) {
	/*PORTC &= ~(1<<PC5);
	_delay_ms(1);
	PORTC |= (1<<PC5);
	_delay_ms(1);*/
	//_delay_ms(1);
	rs485_uart_sent();
}

/*
 * Initialize the uart
 */
void rs485_uart_init() {
	RS485_USART_DE_DDR |= (1<<RS485_USART_DE_PIN);
	rs485_uart_drive(0);

	/* Asynchronous operation */
	/* Select 9-bit data frame */
	/* 1 stop bit */

	RS485_USART_UCSRC = (1<<RS485_USART_URSEL) | (1<<RS485_USART_UCSZ0) | (1<<RS485_USART_UCSZ1);
	/* Multi-processor mode */
	//RS485_USART_UCSRA |= (1<<RS485_USART_MPCM);
	/* Select 9-bit data frame */
	//RS485_USART_UCSRB |= (1<<RS485_USART_UCSZ2);
	/* Enable receive and transmit interrupts */
	RS485_USART_UCSRB |= (1<<RS485_USART_RXCIE);
	//RS485_USART_UCSRB |= (1<<RS485_USART_TXCIE);
	/* Enable receiver and transmitter */
	//RS485_USART_UCSRB |= (1<<RS485_USART_RXEN) | (1<<RS485_USART_TXEN);
	RS485_USART_UCSRB |= (1<<RS485_USART_TXEN);
	/* Setup baud rate, bus speed 0,5MBit/s */
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
#if USE_2X
	/* Double speed */
	RS485_USART_UCSRA |= (1<<RS485_USART_U2X);
#else
	/* Normal speed */
	RS485_USART_UCSRA &= ~(1<<RS485_USART_U2X);
#endif
}

/*
 * Write a byte to the bus
 */
void rs485_uart_puts(uint8_t byte, uint8_t addr) {
	while(!(RS485_USART_UCSRA & (1<<RS485_USART_UDRE))) {};
	/*if(addr) {
		RS485_USART_UCSRB |= (1<<RS485_USART_TXB8);
	} else {
		RS485_USART_UCSRB &= ~(1<<RS485_USART_TXB8);
	}*/

	//rs485_uart_drive(1);
	RS485_USART_UDR = byte;
}

void rs485_uart_start_transmission() {
	rs485_uart_drive(1);
	PORTC |= (1<<PC5);
	RS485_USART_UCSRB |= (1<<RS485_USART_UDRIE);
}

void rs485_uart_stop_transmission() {
	PORTC &= ~(1<<PC5);
	RS485_USART_UCSRB &= ~(1<<RS485_USART_UDRIE);
}

/*
 * Set whether the UART should only listen to address bytes
 */
void rs485_uart_addressed(uint8_t b) {
	if(b) {
		/* Only receive address bytes */
		//RS485_USART_UCSRA |= (1<<RS485_USART_MPCM);
	} else {
		/* Receive all bytes (data bytes) */
		//RS485_USART_UCSRA &= ~(1<<RS485_USART_MPCM);
	}
}
