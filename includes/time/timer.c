/*
 * timer.c
 *
 *  Created on: 30.11.2010
 *      Author: sven
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "../../conf/controller.h"
#include "../../conf/settings.h"

volatile uint16_t ticks_passed = 0;

ISR(defTIMEVEC) {
	ticks_passed++;
}

/*
 * Returns system ticks since last call
 */
uint16_t timer_get_ticks_passed() {
	uint16_t ret = ticks_passed;
	ticks_passed = 0;
	return ret;
}

/*
 * Initializes the system tick timer
 */
void timer_init() {

	/*
	 * Clear on compare match
	 */
	defTCCRB |= (1<<defCTCBit);

	defTIMSK |= (1<<defOCIE);

#if F_CPU == 16000000
	/*
	 * Prescaler 8, 2mio. clks per second
	 * So 2 clks = 1us
	 */
	defTCCRB |= (1<<defCS1);

	/*
	 * Interrupt every -us/per/tick- us
	 */
	defOCR = 2 * SYSTEM_TICK_LENGTH;
#endif
}
