/*
 * rs485.c
 *
 *  Created on: 24.11.2010
 *      Author: sven
 */

#include <avr/io.h>

void (*error_cb)(uint8_t);			/* Error callback function */

/*
 * Register the function to be called when an rs485 error happened that
 * may have to be known by the application.
 */
void rs485_register_error_handler(void (*err_cb)(uint8_t)) {
	error_cb = err_cb;
}

/*
 * Called when an error happened that the application should know of.
 */
void rs485_error(uint8_t err) {
	(*error_cb)(err);
}
