/*
 * crc.c
 *
 *  Created on: 28.11.2010
 *      Author: sven
 */

#include <avr/io.h>
#include "crc.h"

#ifdef USE_RS485_CRC_CHECKSUM

/*
 * Calculate checksum for given byte array
 */
uint16_t crc_calculate_bytes(uint8_t* data, uint8_t nbytes, uint16_t start, uint16_t polynomial) {
	uint16_t crc = start;
	for(uint8_t i = 0; i < nbytes; i++) {
		crc = crc_calculate_byte(data[i], crc, polynomial);
	}
	return crc;
}

/*
 * Calculate checksum for given byte
 */
uint16_t crc_calculate_byte(uint8_t byte, uint16_t start, uint16_t polynomial) {
	uint16_t crc = start;
	for(uint8_t i = 0; i < 8; i++) {
		if(((crc & 0x8000) ? 1 : 0) != (byte & (1<<i))) {
			crc = (crc << 1) ^ polynomial;
		} else {
			crc <<= 1;
		}
	}
	return crc;
}

#endif
