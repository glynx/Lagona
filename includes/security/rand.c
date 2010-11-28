/*
 * rand.c
 *
 *  Created on: 28.11.2010
 *      Author: sven
 */

#include <avr/io.h>
#include <stdlib.h>

unsigned short get_seed()
{
   unsigned short seed = 0;
   unsigned short *p = (unsigned short*) (RAMEND+1);
   extern unsigned short __heap_start;

   while (p >= &__heap_start + 1)
      seed ^= * (--p);

   return seed;
}

void rand_init() {
	srand(get_seed());
}

/*
 * Get next random number between 0 and 255
 */
uint8_t rand_next() {
	return rand() % 256;
}

/*
 * Get random number between from to to
 */
uint8_t rand_in(uint8_t from, uint8_t to) {
	return from + (rand() % (to - from + 1));
}
