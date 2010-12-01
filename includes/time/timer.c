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

#if TIMER_INTERRUPT_MODE != 1
	volatile uint8_t overflows = 0;
#endif

#if TIMER_INTERRUPT_MODE == 1
ISR(defTIMEVEC) {
	ticks_passed++;
}
#else
ISR(defOVFLVEC) {
	overflows++;
}
#endif

/*
 * Returns system ticks since last call
 */
uint16_t timer_get_ticks_passed() {
#if TIMER_INTERRUPT_MODE == 1
	uint16_t ret = ticks_passed;
	ticks_passed = 0;
	return ret;
#else
	uint32_t ticks = overflows * 65536;
	overflows = 0;
	ticks += defTIME;
	uint16_t divider = 2 * SYSTEM_TICK_LENGTH;
	defTIME = ticks % divider;
	ticks /= divider;
	overflows = 0;
	return (uint16_t) ticks;
#endif
}

/*
 * Initializes the system tick timer
 */
void timer_init() {

#if TIMER_INTERRUPT_MODE == 1
	/*
	 * Clear on compare match
	 */
	defTCCRB |= (1<<defCTCBit);

	defTIMSK |= (1<<defOCIE);

#else
#endif

#if F_CPU == 16000000
	/*
	 * Prescaler 8, 2mio. clks per second
	 * So 2 clks = 1us
	 */
	defTCCRB |= (1<<defCS1);

#if TIMER_INTERRUPT_MODE == 1
	/*
	 * Interrupt every -us/per/tick- us
	 */
	defOCR = 2 * SYSTEM_TICK_LENGTH;
#else
	/* Only use overflow interrupt */
#endif
#endif
}
