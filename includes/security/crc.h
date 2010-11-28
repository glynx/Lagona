/*
 * crc.h
 *
 *  Created on: 28.11.2010
 *      Author: sven
 */

#ifndef CRC_H_
#define CRC_H_

uint16_t crc_calculate_bytes(uint8_t* data, uint8_t nbytes, uint16_t start, uint16_t polynome);
uint16_t crc_calculate_byte(uint8_t byte, uint16_t start, uint16_t polynome);

#endif /* CRC_H_ */
