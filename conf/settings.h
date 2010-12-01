/*
 * settings.h
 *
 *  Created on: 28.11.2010
 *      Author: sven
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

/*
 * Modules to be included in the compilation.
 * Entries that are not commented out will be loaded.
 */

//#define USE_DOUBLE_LINKED_LISTS
//#define USE_RS485_ADDRESS_GROUPS
#define USE_RS485_CRC_CHECKSUM

/*
 * System wide configurations
 */

/* Length of a tick in us */
#define SYSTEM_TICK_LENGTH	100U // 0.1 ms

/* Use system timer in interrupt mode? */
/*
 * 		Interrupt mode interrupts every system tick to increase counter value.
 * 		Non-interrupt (or polling) mode calculates counter value only when asked.
 */
#define TIMER_INTERRUPT_MODE 0

#endif /* SETTINGS_H_ */
