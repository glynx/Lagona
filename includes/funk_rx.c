#include <includes/funk_rx.h>
#include <includes/funk_tx.h>
#include <includes/funk.h>
#include <includes/time.h>
#include <includes/onoff.h>
#include <includes/uart.h>

char packet_rx( unsigned char *d, unsigned char l)
{

  struct funk_flags ff;// = (struct funk_flags) d[2];
  unsigned char *tmp;
  tmp = ( unsigned char *) &ff;
  *tmp = d[2];
  if(packet_correct(d, l))
  {

    if(ff.need_ack)
      send_ack(d[1]);
    
    packet_evaluation(d, l);
  }
  else
  {
    uart_send_char('f');
  }
  return 1;
}

char packet_correct( unsigned char *d, unsigned char l)
{
  uint16_t checksum = d[l-1];
  checksum = checksum << 8;
  checksum = checksum + d[l-2];
  uart_send_var(checksum);
  for(int i = 0; i < l - 2; i++)
    checksum -= d[i];
  checksum -= 21930;
  uart_send_var(checksum);
  if(checksum == 0)
    return 1;
  else
    return 0;
}


void send_resend(unsigned char d)
{
}

void packet_evaluation(unsigned char *d, unsigned char l)
{
  unsigned char pos = DATA_OFFSET;     //Commando Bytes nach folgenden aufbau:
			      //databyte 0: Die angesprochende funktion
			      //databyte 1: Die Nummer des gerätes zb kann eine funksteckdose ja mehr als einen regelbaren ausgang haben
			      //databyte 2...: Die zu übergebenden Parameter
  switch(d[pos])
  {
#if onoff == 1
    case 9:		      //im Falle einer 9 handelt es sich um eine onoff funksteckdose
      onoff_dev(d[pos+1], d[pos+2]);
      break;
#endif

#if time == 1
    case 21:		      //im Falle einer 9 handelt es sich um eine onoff funksteckdose
      time_sync(d);
      break;
#endif

#if uart == 1
    default:
      uart_send_pgm_string(wrong_dev_type);
#endif
  }
}