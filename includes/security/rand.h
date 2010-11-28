/*
 * rand.h
 *
 *  Created on: 28.11.2010
 *      Author: sven
 */

#ifndef RAND_H_
#define RAND_H_

/*
 * Prototypes
 */

void rand_init();
uint8_t rand_next();
uint8_t rand_in(uint8_t from, uint8_t to);

#endif /* RAND_H_ */
