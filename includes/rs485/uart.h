/*
 * uart.h
 *
 *  Created on: 25.11.2010
 *      Author: sven
 */


/*
 * Prototypes
 */

void rs485_uart_puts(uint8_t byte, uint8_t addr);
void rs485_uart_transmit(uint8_t byte, uint8_t addr);
void rs485_uart_addressed(uint8_t b);
void rs485_uart_init();
void rs485_uart_start_transmission();
void rs485_uart_stop_transmission();
