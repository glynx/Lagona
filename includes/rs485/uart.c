/*
 * uart.c
 *
 *  Created on: 25.11.2010
 *      Author: sven
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "rs485.h"
#include "protocol.h"
#include "../../conf/errors.h"

/* Interrupt: Reception complete */
ISR(RS485_USART_RXIR) {
	if(RS485_USART_UCSRA & (1<<RS485_USART_PE)) {
		rs485_uart_receive_error(ERR_USART_PARITY_ERROR);
		return;
	}
	if(RS485_USART_UCSRA & (1<<RS485_USART_FE)) {
		rs485_uart_receive_error(ERR_USART_FRAME_ERROR);
		return;
	}
	if(RS485_USART_UCSRA & (1<<RS485_USART_DOR)) {
		rs485_uart_receive_error(ERR_USART_DATA_OVERRUN_ERROR);
		return;
	}

	uint8_t byte = RS485_USART_UDR;
	if(RS485_USART_UCSRB & (1<<RS485_USART_RXB8)) {
		/* Address byte */
		rs485_uart_address_received(byte);
	} else {
		/* Data byte */
		rs485_uart_received(byte);
	}
}

/* Interrupt: Transmission complete */
ISR(RS485_USART_TXIR) {
	rs485_uart_sent();
}

void rs485_uart_init() {
	/* Setup baud rate, bus speed 0,5MBit/s */
	RS485_USART_UBBRH = 0;
	RS485_USART_UBBRL = 3;

	/* Double speed */
	RS485_USART_UCSRA |= (1<<RS485_USART_U2X);
	/* Multi-processor mode */
	RS485_USART_UCSRA |= (1<<RS485_USART_MPCM);
	/* Use even parity mode */
	/* Asynchronous operation */
	/* Select 9-bit data frame */
	/* 1 stop bit */
	RS485_USART_UCSRC = (1<<RS485_USART_URSEL) | (1<<RS485_USART_UPM1) | (1<<RS485_USART_UCSZ0) | (1<<RS485_USART_UCSZ1);
	/* Select 9-bit data frame */
	RS485_USART_UCSRB |= (1<<RS485_USART_UCSZ2);
	/* Enable receive and transmit interrupts */
	RS485_USART_UCSRB |= (1<<RS485_USART_RXCIE) | (1<<RS485_USART_TXCIE);
	/* Enable receiver and transmitter */
	RS485_USART_UCSRB |= (1<<RS485_USART_RXEN) | (1<<RS485_USART_TXEN);
}

void rs485_uart_puts(uint8_t byte, uint8_t addr) {
	while(!(RS485_USART_UCSRA & (1<<RS485_USART_UDRE))) {};
	if(addr) {
		RS485_USART_UCSRB |= (1<<RS485_USART_TXB8);
	} else {
		RS485_USART_UCSRB &= ~(1<<RS485_USART_TXB8);
	}
	RS485_USART_UDR = byte;
}

void rs485_uart_addressed(uint8_t b) {
	if(b) {
		/* Only receive address bytes */
		RS485_USART_UCSRA |= (1<<RS485_USART_MPCM);
	} else {
		/* Receive all bytes (data bytes) */
		RS485_USART_UCSRA &= ~(1<<RS485_USART_MPCM);
	}
}
