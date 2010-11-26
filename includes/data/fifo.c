/*
 * fifo.c
 *
 *  Created on: 24.11.2010
 *      Author: sven
 */

#include <stdlib.h>
#include <avr/io.h>
#include "fifo.h"
#include "../../conf/errors.h"

FIFO* fifo_new(uint8_t size) {
	FIFO* f = (FIFO*) malloc(sizeof(FIFO));
	if(f) {
		f->data = (void**) malloc(sizeof(void*)*size);
		if(f->data) {
			f->size = size;
			f->count = 0;
			f->pos = 0;
		} else {
			free(f);
			f = NULL;
		}
	}
	return f;
}

void* fifo_next(FIFO* fifo) {
	void* e = NULL;
	if (fifo->count > 0) {
		e = fifo->data[fifo->pos];
		fifo->pos++;
		fifo->count--;
		if (fifo->pos >= fifo->size) {
			fifo->pos = 0;
		}
	}
	return e;
}

uint8_t fifo_add(FIFO* fifo, void* element) {
	if (fifo->count < fifo->size) {
		fifo->data[fifo->pos + fifo->size] = element;
		fifo->count++;
		return ERR_NONE;
	} else {
		return ERR_FIFO_OVERRUN;
	}
}
