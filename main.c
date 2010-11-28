#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"
#include "conf/controller.h"
#include "includes/security/rand.h"

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

int main(void)
{
	rand_init();
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
}

/*
 * Call this when a unrecoverable error occurs.
 * This will halt the whole program.
 */
void critical_error(uint8_t err) {
	cli(); // No more interrupts
	while(1) {
		// TODO: Error report? (LED i.e.)
	}
}
