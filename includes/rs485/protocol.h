/*
 * protocol.h
 *
 *  Created on: 24.11.2010
 *      Author: sven
 *
 *  This file includes the physical layer and therefore handles basic
 *  communication, package address recognition and package error handling.
 *  If a valid packet has been received it is passed to the processor.
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <avr/io.h>

struct rs485_flags {
	uint8_t ack_req:1;
	uint8_t ack:1;
	uint8_t nack:1;
	uint8_t flag4:1;
	uint8_t flag5:1;
	uint8_t flag6:1;
	uint8_t flag7:1;
	uint8_t flag8:1;
};

typedef union rs485_flags_union {
	uint8_t byte;
	struct rs485_flags flags;
} RS485_FLAGS;

typedef struct rs485_header {
	uint8_t destination;
	uint8_t origin;
	RS485_FLAGS flags;
	uint8_t length;
} RS485_HEADER;

typedef struct rs485_body {
	uint8_t* data;
	uint16_t checksum;
} RS485_BODY;

struct rs485_package {
	RS485_HEADER header;
	RS485_BODY body;
};

typedef union rs485_package_union {
	struct rs485_package package;
	uint8_t* bytes;
} RS485_PACKAGE;

typedef struct rs485_travelling_package {
	RS485_PACKAGE* package;
	uint8_t position;
	uint8_t size;
} RS485_TRAVELLING_PACKAGE;

/*
 * UART callbacks
 */
void rs485_uart_receive_error(uint8_t error);
void rs485_uart_address_received(uint8_t addr);
void rs485_uart_received(uint8_t byte);
void rs485_uart_sent();

typedef uint8_t RS485_ADDRESS;
#define RS485_DEFAULT_ADDRESS		0xFE
#define RS485_MASTER_ADDRESS		0x00
#define RS485_BROADCAST_ADDRESS		0xFF
uint8_t rs485_address_equals(void* a1, void* a2);

#define RS485_CRC16_POLY 			0x04C1
#define RS485_CRC16_START			0xFFFF

/* Timeouts/Delays */
#define	RS485_RX_TIMEOUT			5	// 0.5 ms
#define RS485_ACK_TIMEOUT			10  // 1ms

#endif /* PROTOCOL_H_ */
