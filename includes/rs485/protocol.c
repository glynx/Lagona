/*
 * protocol.c
 *
 *  Created on: 24.11.2010
 *      Author: sven
 */

#include <stdlib.h>
#include "../../main.h"
#include "protocol.h"
#include "rs485.h"
#include "uart.h"
#include "../../conf/errors.h"
#include "../data/fifo.h"
#ifdef USE_RS485_ADDRESS_GROUPS
	#include "../data/list.h"
#endif
#include "../security/crc.h"
#include "../security/rand.h"

/* Current protocol status flags */
typedef struct status {
	uint8_t addressed:1;
	uint8_t sending:1;
	uint8_t receiving:1;
	uint8_t header_rcvd:1;
	uint8_t waiting_for_ack:1;
	uint8_t last_send_result:1;
	uint8_t package_received:1;
	uint8_t package_sent:1;
} STATUS;
STATUS rs485_status;

FIFO* rx_buffer;							/* Buffer for received packages */

uint8_t tx_delay = 0;						/* Ticks after which transmit is tried */
uint8_t tx_retries = 0;						/* Number of retries current tx package already had */
uint8_t rx_timeout = 0;						/* Ticks to wait until receiving package will be dropped */

uint8_t last_error = ERR_NONE;				/* Last error that occured */

RS485_TRAVELLING_PACKAGE* tx_package;		/* The currently sent package */
RS485_TRAVELLING_PACKAGE* rx_package;		/* The currently received package */

RS485_ADDRESS address;						/* Bus main address */

#ifdef USE_RS485_ADDRESS_GROUPS
LIST* groups;								/* Additional addresses (used for grouping) */
#endif

// TODO: Address = broadcast address <=> receive all

/*****
 * Callbacks
 *****/
void (*rs485_package_received_cb)(void);
void (*rs485_package_sent_cb)(void);
void (*rs485_error_cb)(uint8_t);

/*****
 * Prototypes
 *****/
void rs485_stop_sending();
void rs485_postpone_sending(uint8_t ticks);
void rs485_stop_receiving();
void rs485_cancel_receiving();

void rs485_send_package(RS485_PACKAGE* package);
void rs485_received_ack(uint8_t ack, uint16_t origin);
void rs485_package_received(RS485_PACKAGE* package);
void rs485_free_package();
RS485_PACKAGE* rs485_next_package();
void rs485_send_ack(uint16_t dest, uint8_t ack);

void rs485_uart_receive_error(uint8_t error);
void rs485_uart_address_received(uint8_t addr);
void rs485_uart_received(uint8_t byte);
void rs485_send_byte();
void rs485_uart_sent();

void rs485_wait_for_address();
void rs485_addressed();
void rs485_proccess(uint8_t ticks_passed);

#ifdef USE_RS485_ADDRESS_GROUPS
uint8_t rs485_add_group(RS485_ADDRESS addr);
void rs485_remove_group(RS485_ADDRESS addr);
#endif
void rs485_init();
uint8_t rs485_is_sending();
uint8_t rs485_is_receiving();
uint8_t rs485_is_ready_to_send();
uint8_t rs485_is_package_waiting();
uint8_t rs485_last_package_sent_successfully();

void rs485_register_package_received_cb(void (*cb)(void));
void rs485_register_package_sent_cb(void (*cb)(void));
void rs485_register_error_cb(void (*cb)(uint8_t));

// TODO: Before sending, check if the bus is busy

/*****
 * RXTX control
 *****/

void rs485_stop_sending() {
	rs485_status.sending = 0;
	rs485_status.package_sent = 1;
	rs485_free_package();
	rs485_wait_for_address();
}

/*
 * Schedules the current tx package to be sent later
 */
void rs485_postpone_sending(uint8_t ticks) {
	if(rs485_status.sending) {
		if(tx_package) {
			if(tx_retries < RS485_MAX_TX_RETRIES) {
				/* Package will be resent */
				tx_retries++;
				tx_package->position = 0;
				if(tx_delay == 0xFF) {
					tx_delay = rand_in(1, 250); // Retry after 2 to 250 ticks, thats 0 to 25 ms
				} else {
					if(ticks == 0) {
						// Resend immediately
						rs485_send_byte();
					}
					// Resend later
					tx_delay = ticks;
				}
			} else {
				/* Package not sent successfully */
				rs485_status.last_send_result = 0;
				rs485_stop_sending();
			}
		}
	}
	rs485_wait_for_address();
}

void rs485_stop_receiving() {
	rs485_status.receiving = 0;
	rs485_free_package();
	rs485_wait_for_address();
}

void rs485_cancel_receiving() {
	if(rs485_status.receiving && rs485_status.header_rcvd) {
		if(rx_package->package->package.header.flags.flags.ack_req) {
			rs485_stop_receiving();
			rs485_send_ack(rx_package->package->package.header.origin, 0);
		} else {
			rs485_stop_receiving();
		}
	} else {
		rs485_stop_receiving();
	}
}

/*****
 * Package processing
 *****/

/*
 * Transmit the given package. The memory of given package is automatically freed
 * after transmission was complete.
 */
void rs485_send_package(RS485_PACKAGE* package) {
	while(rs485_status.receiving || rs485_status.sending) {	};
	tx_package = (RS485_TRAVELLING_PACKAGE*) malloc(sizeof(RS485_TRAVELLING_PACKAGE));
	if(tx_package) {
		tx_package->package = package;
		tx_package->position = 0;
		/* Size is header size + data size and 2 if data length != 0*/
		tx_package->size = sizeof(RS485_HEADER);
		if(package->package.header.length) {
			 tx_package->size += 2 + package->package.header.length;
#ifdef USE_RS485_CRC_CHECKSUM
			 package->package.body.checksum = crc_calculate_bytes(package->package.body.data, package->package.header.length, RS485_CRC16_START, RS485_CRC16_POLY);
#else
#warning	Clients with CRC checksum disabled may only send packages without data
			 critical_error(ERR_SEND_WITHOUT_CHECKSUM);
#endif
		}
		rs485_status.sending = 1;
		tx_delay = 0;
		tx_retries = 0;
		rs485_addressed();
		rs485_send_byte();
	} else {
		/* Allocation error */
		last_error = ERR_DATA_ALLOC_FAILED;
		rs485_status.last_send_result = 0;
		rs485_stop_sending();
	}
}

/*
 * Received ACK or NACK from slave
 */
void rs485_received_ack(uint8_t ack, uint16_t origin) {
	if(ack) {
		/* Packet was received properly */
		rs485_status.last_send_result = 1;
		rs485_stop_sending();
	} else {
		/* Packet was not received properly */
		rs485_postpone_sending(0xFF);
	}
}

/*
 * A complete package was received, interpret it
 */
void rs485_package_received(RS485_PACKAGE* package) {
	uint8_t handled = 0;

	/* ACK or NACK */
	if(package->package.header.flags.flags.ack) {
		rs485_received_ack(1, package->package.header.origin);
		handled = 1;
	}
	if(package->package.header.flags.flags.nack) {
		rs485_received_ack(0, package->package.header.origin);
		handled = 1;
	}

	if(!handled) {
#ifdef RS485_USE_CRC_CHECKSUM
		uint16_t crc;
		crc = crc_calculate_bytes(package->package.body.data, package->package.header.length, RS485_CRC16_START, RS485_CRC16_POLY);
		crc = crc_calculate_bytes((uint8_t*)&(package->package.body.checksum), 2, crc, RS485_CRC16_POLY);
		if(crc) {
			/* CRC invalid */
			if(package->package.header.flags.flags.ack_req) {
				rs485_send_ack(package->package.header.origin, 0);
			}
			return;
		}
#endif

		if(package->package.header.flags.flags.ack_req) {
			rs485_send_ack(package->package.header.origin, 1);
		}

		uint8_t res;
		if((res = fifo_add(rx_buffer, package)) != ERR_NONE) {
			/* FIFO error */
			last_error = res;
			if(package->package.header.flags.flags.ack_req) {
				rs485_send_ack(package->package.header.origin, 0);
			}
			free(package); // Dropped
		} else {
			rs485_status.package_received = 1;
		}
	}
}

/*
 * Free memory for the travelling rxtx and underlying rs485 package
 */
void rs485_free_package(RS485_TRAVELLING_PACKAGE* package) {
	if(package) {
		if(package->package) {
			free(package->package);
		}
		free(package);
	}
}

/*
 * Returns the next package in the rx buffer or NULL if there is none.
 */
RS485_PACKAGE* rs485_next_package() {
	if(rx_buffer->count) {
		return (RS485_PACKAGE*) fifo_next(rx_buffer);
	} else {
		return NULL;
	}
}

/*
 * Sends an ACK/NACK package to the given address
 */
void rs485_send_ack(uint16_t dest, uint8_t ack) {
	RS485_PACKAGE* package = (RS485_PACKAGE*) calloc(1, sizeof(RS485_PACKAGE));
	package->package.header.destination = dest;
	if(ack) {
		package->package.header.flags.flags.ack = 1;
	} else {
		package->package.header.flags.flags.nack = 1;
	}
	package->package.header.origin = address;
	rs485_send_package(package);
}

/*****
 * RXTX byte handling
 *****/

/*
 * Called when the UART detected an error while receiving a byte.
 * Receiving of the package is then cancelled.
 */
void rs485_uart_receive_error(uint8_t error) {
	rs485_cancel_receiving();
	last_error = error;
}

/*
 * Called when a byte marked with address type is received from
 * UART. This should indicate the beginning of a new incoming transmission.
 */
void rs485_uart_address_received(uint8_t addr) {
	if(rs485_status.sending && !rs485_status.waiting_for_ack) {
		/* When we are sending and not waiting for ack, the received address
		 * simply is the one we just send. So treat it as usual byte. */
		rs485_uart_received(addr);
	} else {
		if((!rs485_status.sending || rs485_status.waiting_for_ack) && !rs485_status.receiving) {
			/* When we are waiting for a packet, either because we are idle or we
			 * expect an ack, the address is the beginning of a new package to be received */
			if(address != RS485_BROADCAST_ADDRESS && addr != address) {
#ifdef USE_RS485_ADDRESS_GROUPS
				if(!list_contains_data(groups, &addr, rs485_address_equals)) {
					/* Address doesn't match */
					return;
				}
#else
				/* Address doesn't match */
				return;
#endif
			}
			rx_package = (RS485_TRAVELLING_PACKAGE*) malloc(sizeof(RS485_TRAVELLING_PACKAGE));
			if(!rx_package) {
				last_error = ERR_DATA_ALLOC_FAILED;
			}
			if(last_error == ERR_NONE) {
				rx_package->package = (RS485_PACKAGE*) calloc(1, sizeof(RS485_PACKAGE));
				if(!rx_package->package) {
					last_error = ERR_DATA_ALLOC_FAILED;
				}
				if(last_error == ERR_NONE) {
					rx_package->position = 0;
					rx_package->size = sizeof(RS485_HEADER);
					rs485_status.receiving = 1;
					rs485_status.header_rcvd = 0;
					rx_timeout = RS485_RX_TIMEOUT; // Time before next byte should be received
				}
			}
			if(last_error != ERR_NONE) {
				rs485_cancel_receiving();
			}
		} else {
			/* We received an address while already receiving, this is an error condition */
			if(rs485_status.receiving) {
				rs485_cancel_receiving();
			}
		}
	}
}

/*
 * Called from UART, when one byte was received. The received byte is
 * put at the end of current rx package.
 */
void rs485_uart_received(uint8_t byte) {
	// Only accept if we currently expect to be receiving
	if(rs485_status.receiving) {
		if(rx_package) {
			rx_timeout = RS485_RX_TIMEOUT; // Time before next byte should be received
			rx_package->package->bytes[rx_package->position] = byte;
			rx_package->position++;
			if(rx_package->position >= rx_package->size) {
				if(rs485_status.header_rcvd) {
					/* Package received completely */
					rs485_status.receiving = 0;
					rs485_wait_for_address();
					rs485_package_received(rx_package->package);
					free(rx_package); //NOTE: Only free rx package, not the rs485 package
				} else {
					/* Header complete, evaluate it */
					rs485_status.header_rcvd = 1;
					/* Read length and prepare to receive body */
					uint8_t length = rx_package->package->package.header.length;
					if(length) {
						/* Receive another 'length' bytes + checksum */
						rx_package->package->package.body.data = (uint8_t*) malloc(length);
						if(rx_package->package->package.body.data) {
							rx_package->size += length + 2;
						} else {
							/* Allocation fault */
							last_error = ERR_DATA_ALLOC_FAILED;
							rs485_cancel_receiving();
						}
					} else {
						/* Package received completely */
						rs485_status.receiving = 0;
						rs485_wait_for_address();
						rs485_package_received(rx_package->package);
						free(rx_package); //NOTE: Only free rxtx package, not the rs485 package
					}
				}
			}
		}
	}
	if(rs485_status.sending) {
		/* Received our own byte, check it against what we thought we sent */
		if(byte != tx_package->package->bytes[tx_package->position-1]) {
			rs485_postpone_sending(0xFF); // Try to re-send later
		}
	}
}

/*
 * Sends the next byte via the UART. As this blocks until UART is ready it should
 * only be called after the UART signaled ready.
 */
void rs485_send_byte() {
	if(rs485_status.sending && tx_package) {
		if(tx_package && rx_package->position < tx_package->size) {
			/* Send the next byte */
			rs485_uart_puts(rx_package->package->bytes[tx_package->position], rx_package->position == 0);
			tx_package->position++;
		} else {
			/* Package was sent */
			if(tx_package->package->package.header.flags.flags.ack_req) {
				/* Wait for ack before finishing sending */
				rs485_status.waiting_for_ack = 1;
				rs485_wait_for_address();
				rx_timeout = RS485_ACK_TIMEOUT; // Give the slave some ticks to answer
			} else {
				/* We don't have to wait for ack */
				rs485_status.last_send_result = 1;
				rs485_stop_sending();
			}
		}
	}
}

/*
 * Called when the UART finished sending a byte.
 */
void rs485_uart_sent() {
	rs485_send_byte();
}

/*****
 * Control helpers
 *****/

/*
 * Indicate that the UART should only receive address bytes.
 */
void rs485_wait_for_address() {

	rs485_status.addressed = 0;
}

/*
 * Indicate that we have received a matching address and are now receiving
 * a packet.
 */
void rs485_addressed() {

	rs485_status.addressed = 1;
}

/*
 * This routine should be called regularly externally to ensure proper operation.
 * It updates timers, delays, sends events, etc.
 */
void rs485_proccess(uint8_t ticks_passed) {
	// TODO: Update timers, delays, send events etc.
	if(last_error != ERR_NONE) {
		if(rs485_error_cb) {
			(*rs485_error_cb)(last_error);
		}
		last_error = ERR_NONE;
	}
	if(rs485_status.package_received) {
		if(rs485_package_received_cb) {
			(*rs485_package_received_cb)();
		}
	}
	if(rs485_status.package_sent) {
		if(rs485_package_sent_cb) {
			(*rs485_package_sent_cb)();
		}
	}
	if(rs485_status.sending && tx_delay) {
		tx_delay -= ticks_passed;
		if(tx_delay <= 0) {
			/* Start sending */
			tx_delay = 0;
			rs485_send_byte();
		}
	}
	if(rx_timeout) {
		if(!rs485_status.receiving && rs485_status.waiting_for_ack) {
			rx_timeout -= ticks_passed;
			if(rx_timeout <= 0) {
				/* ACK was not received in time, try to resend */
				rx_timeout = 0;
				rs485_postpone_sending(0); // Resend now
			}
		}
		if(rs485_status.receiving) {
			rx_timeout -= ticks_passed;
			if(rx_timeout <= 0) {
				/* Timeout while receiving */
				rx_timeout = 0;
				rs485_cancel_receiving();
			}
		}
	}
}

/*****
 * Settings
 *****/

#ifdef USE_RS485_ADDRESS_GROUPS
/*
 * Add to group
 */
uint8_t rs485_add_group(RS485_ADDRESS addr) {
	if(!list_contains_data(groups, &addr, rs485_address_equals)) {
		RS485_ADDRESS* address = (RS485_ADDRESS*) malloc(sizeof(RS485_ADDRESS));
		if(address) {
			if(list_add(groups, address)) {
				return 1;
			} else {
				free(address);
			}
		}
		return 0;
	}
	return 1;
}

/*
 * Remove from group
 */
void rs485_remove_group(RS485_ADDRESS addr) {
	list_remove_item(groups, list_item_for(groups, &addr, rs485_address_equals));
}

#endif

/*
 * Initializes the rs485. Call this first.
 */
void rs485_init() {
	rs485_status.addressed = 0;
	address = RS485_DEFAULT_ADDRESS;
	rx_buffer = fifo_new(RS485_RX_PACKAGE_BUFFER_SIZE);
	if(!rx_buffer) {
		critical_error(ERR_BUFFER_ALLOC_FAILED);
	}
#ifdef USE_RS485_ADDRESS_GROUPS
	groups = list_new();
	if(!groups) {
		critical_error(ERR_LIST_ALLOC_FAILED);
	}
#endif
	rs485_uart_init();
}

/*
 * Returns 1 if we are currently sending to the bus
 */
uint8_t rs485_is_sending() {
	return rs485_status.sending;
}

/*
 * Returns 1 if we are currently receiving from the bus
 */
uint8_t rs485_is_receiving() {
	return rs485_status.receiving;
}

/*
 * Returns 1 only if we are ready to send a message
 */
uint8_t rs485_is_ready_to_send() {
	return !(rs485_status.sending || rs485_status.receiving);
}

/*
 * Returns != 0 if there are received packages waiting in the queue
 */
uint8_t rs485_is_package_waiting() {
	return rx_buffer->count;
}

/*
 * Returns 1 if the last package was sent successfully
 */
uint8_t rs485_last_package_sent_successfully() {
	return rs485_status.last_send_result;
}

/*****
 * Callback registration
 *****/

/*
 * Given function is called when a package was received successfully
 */
void rs485_register_package_received_cb(void (*cb)(void)) {
	rs485_package_received_cb = cb;
}

/*
 * Given function is called when a package was sent. Note that this
 * doesn't mean that the package was successfully sent.
 * To determine that, use rs485_last_package_sent_successfully();
 */
void rs485_register_package_sent_cb(void (*cb)(void)) {
	rs485_package_sent_cb = cb;
}

/*
 * Given function is called when an error happened
 */
void rs485_register_error_cb(void (*cb)(uint8_t)) {
	rs485_error_cb = cb;
}

uint8_t rs485_address_equals(void* a1, void* a2) {
	return *((RS485_ADDRESS*)a1) == *((RS485_ADDRESS*)a2);
}
