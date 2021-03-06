#include <includes/time.h>
#include <includes/funk.h>
#include <main.h>
#include <includes/uart.h>
#include <conf/controller.h>

void time_init(void)
{
  // Timer 0 konfigurieren
  defTCCRB |= (1<<defCS2)|(1<<defCS0);//|(1<<defWGM2); // Prescaler 8
  defTCCRA |= (1<<defWGM1); // | (1<<defCOM1);
  // ((1000000/8)/1000) = 125
  

  defOCR = OCR_VAL;
 
  // Compare Interrupt erlauben
  defTIMSK |= (1<<defOCIE);
}



void send_time_sync ( void )
{
  funk_write_data[2] = 0;          //Flags
  funk_write_data[4] = 21;
  funk_write_data[5] = 0;
  funk_write_data[6] = stunde;
  funk_write_data[7] = minute;
  funk_write_data[8] = sekunde;
  
  uart_send_var_string( (unsigned char *) funk_write_data, funk_write_data[3]);
  struct funk_flags f;
  funk_send_data( (unsigned char *) funk_write_data, build_package(255, 5, f));
}

void time_sync (unsigned char *d)
{
  stunde = d[DATA_OFFSET + 2];
  minute = d[DATA_OFFSET + 3];
  sekunde = d[DATA_OFFSET + 4];
}


//Der interrupt läuft jede sekunde an
ISR(defTIMEVEC)
{
  unsigned char sreg;
  sreg=SREG;
/*#if defined (__AVR_ATmega16__)
  char read_position = 0;
  for(; read_position < 16; read_position++)
  {
    funk_write_data[read_position] = read_position + pfb;
  }
  pfb++;

  funk_send_data(funk_write_data, 16);
#endif*/
#if funk == 1
#include "funk_tx.h"
  if(waiting_for_ack)
    if(ack_timeout_count >= 0)
      ack_timeout_count--;
#endif
  _8ms++;
  if(_8ms == 124)
  {
    _8ms = 0;
    sekunde++;

    if(sekunde == 60)
    {
      minute++;
      if(minute == 60)
      {
	stunde++;
	minute = 0;
	if(stunde == 24)
	{
	  stunde = 0;
	}
      }
      sekunde = 0;
    }
  }
  SREG=sreg;

}
