#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
#include "main.h"
#include "conf/controller.h"
#include "includes/security/rand.h"
#include "conf/errors.h"
#include "includes/rs485/uart.h"
#include "includes/time/timer.h"

#if rs485 == 1
#include "includes/rs485/rs485.h"
#endif

#if uart == 1
#include "includes/uart.c"
#endif

#if funk == 1
#include "includes/funk.c"
#include "includes/funk_tx.c"
char pfb = 0;
#endif

#if time == 1
#include "includes/time.c"
#endif

#if interface == 1
#include "includes/interface.c"
#endif

#if onoff == 1
#include "includes/onoff.c"
#endif

/*****
 * Status
 *****/
typedef struct main_status {
	uint8_t rest:8;
} MAIN_STATUS;
MAIN_STATUS m_status;

void package_received() {
	while(rs485_is_package_waiting()) {
		RS485_PACKAGE* pck = rs485_next_package();
		PORTC |= (1<<PC5);
		if(pck) {
			if(pck->body->data[0] == 0xFF) {
				if (pck->body->data[1] == 0x01) {
					PORTC |= (1<<PC5);
				} else {
					PORTC &= ~(1<<PC5);
				}
			}
		}
		rs485_free_package(pck);
	}
}

uint16_t process() {
	uint16_t ticks_passed = timer_get_ticks_passed();
	rs485_process(ticks_passed);
	return ticks_passed;
}

/*
 * Initialization
 */
void init() {
	/*
	 * Port settings
	 */
	DDRC |= (1<<PC5) | (1<<PC4);
	PORTC &= ~(1<<PC5) & ~(1<<PC4);

	/*
	 * Initialization
	 */
	rand_init();
	timer_init();
	rs485_init();

	/*
	 * RS485 settings
	 */
	rs485_register_package_received_cb(package_received);
	rs485_set_address(0x02);

	/* Enable interrupts */
	sei();
}

int main(void)
{
	init(); /* Initialize */
	uint16_t timer = 10000; // 1 second
	uint8_t switch_on = 1;
	//uint8_t led_on = 0;
	while(1) {
		uint16_t ticks_passed = process(); /* Handle all process routines */
		//continue;
		if (timer <= ticks_passed) {
			/*if(led_on) {
				PORTC &= ~(1<<PC4);
				led_on = 0;
			} else {
				PORTC |= (1<<PC4);
				led_on = 1;
			}*/
			if(rs485_is_ready_to_send()) {
				//PORTC &= ~(1<<PC4);
				timer = 1 - (ticks_passed - timer);
				RS485_PACKAGE* pck = rs485_new_package(2);
				if(!pck) {
					critical_error(ERR_DATA_ALLOC_FAILED);
				}
				pck->header->destination = 0x02;
				pck->header->origin = 0x01;
				pck->header->flags.flags.ack_req = 0;
				pck->body->data[0] = 0xFF;
				if(switch_on) {
					pck->body->data[1] = 0x01;
					switch_on = 0;
				} else {
					pck->body->data[1] = 0x00;
					switch_on = 1;
				}
				rs485_send_package(pck);
			} else {
				//PORTC |= (1<<PC4);
			}
		} else {
			timer -= ticks_passed;
		}
	}
	/*
  //uart_init();
#if funk == 1
  funk_init();
  //DDR_IRQ &= ~(1 << RF12_IRQ);
  //PORT_IRQ |= (1 << RF12_IRQ);
  funk_cmd(0xCA81);
  funk_enable_fifo();
  funk_cmd(0x0000); //Auslesen des status bytes nur dann wird ein interrupt an int0 ausgelöst

#endif

#if time == 1
  time_init();
#endif

#if onoff == 1
    onoff_init();
    onoff_dev(0, 1);
#endif
 //uart_send_string("\n\r");
  sei();

    
  while(1)
  {
    //uart_send_char('T');
#if funk == 1
    if(funk_data_complete) 	//ist ein datenpacket complett empfangen worden?
    {
      uart_send_char('\n');
      uart_send_char('\r');
      uart_send_var_string( (unsigned char *) funk_data, funk_data_length);
      packet_rx((unsigned char *) funk_data, funk_data_length);		//Hier wird das empfangende Packet bearbeitet
      if(device_mac != 0 && device_mac != 254)
	funk_data[0]++;
      else
	funk_data[0] += 5;

      funk_data_complete = 0;
    }
    if(waiting_for_ack == 1)
    {
      if(ack_timeout_count == 0)
      {
		if(tx_retry == 0)
		{
		  tx_failed++;
		  waiting_for_ack = 0;
		}
		else
		{
		  funk_send_data((unsigned char *) funk_write_data, tx_package_length);
		  tx_retry--;
		  ack_timeout_count = ack_timeout;
		}
      }
    }
    
#endif

#if uart == 1 && interface == 1
    if(uart_str_complete == 1) //wurden am uart befehle übergeben?
    {
      //funk_send_data(uart_string, uart_str_size);
      auswertung();
      uart_send_char('\r');
      uart_send_char('\n');
      uart_str_complete = 0;
    }
#endif

  }
  */
}

/*
 * Call this when a unrecoverable error occurs.
 * This will halt the whole program.
 */
void critical_error(uint8_t err) {
	cli(); // No more interrupts
	while(1) {
		_delay_ms(1000);
		for(uint8_t i = 0; i < err; i++) {
			PORTC |= (1<<PC5);
			_delay_ms(250);
			PORTC &= ~(1<<PC5);
			_delay_ms(250);
		}
		// TODO: Error report? (LED i.e.)
	}
}
