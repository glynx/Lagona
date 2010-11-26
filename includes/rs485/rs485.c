/*
 * rs485.c
 *
 *  Created on: 24.11.2010
 *      Author: sven
 */

#include <avr/io.h>

void (*error_cb)(uint8_t);

void rs485_register_error_handler(void (*err_cb)(uint8_t)) {
	error_cb = err_cb;
}

void rs485_error(uint8_t err) {
	(*error_cb)(err);
}
