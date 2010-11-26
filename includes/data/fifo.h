/*
 * fifo.h
 *
 *  Created on: 24.11.2010
 *      Author: sven
 */

#ifndef FIFO_H_
#define FIFO_H_

typedef struct fifo {
	uint8_t size;
	uint8_t pos;
	uint8_t count;
	void **data;
} FIFO;

FIFO* fifo_new(uint8_t size);
void* fifo_next(FIFO* fifo);
uint8_t fifo_add(FIFO* fifo, void* element);

#endif /* FIFO_H_ */
