/*
 * crc.h
 *
 *  Created on: 28.11.2010
 *      Author: sven
 */

#ifndef CRC_H_
#define CRC_H_

#include "../../conf/settings.h"

#ifdef USE_RS485_CRC_CHECKSUM

/*
 * Prototypes
 */

uint16_t crc_calculate_bytes(uint8_t* data, uint8_t nbytes, uint16_t start, uint16_t polynomial);
uint16_t crc_calculate_byte(uint8_t byte, uint16_t start, uint16_t polynomial);

#endif

#endif /* CRC_H_ */
