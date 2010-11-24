#include <includes/pwm.h>

void pwm_init()
{
    TCCR0 = (1<<CS00);// | (1<<CS00);
    TIMSK |= (1<<TOIE0);
    DDRA = 0xFF;
    OCR0  = 20;
}


void sort_matches()
{
  TIMSK &= ~(1<<TOIE0);
  unsigned char i = 0;
  unsigned char match_nr = 0;
  match_do[0] = 0;
  match_do[1] = 0;
  match_do[2] = 0;
  match_tmp[0] = 255;
  match_tmp[1] = 255;
  match_tmp[2] = 255;
  for(i = 0;i<=254;i++)
  {
    for(unsigned char j=0;j<3;j++)
    {
      if(match[j] == i)
      {
	match_do[match_nr] |= (1<<j);
	match_tmp[match_nr] = i;
      }
    }
    if(match_tmp[match_nr] != 255)
    {
      match_nr++;
      if(match_nr == 3)
	break;
    }
  }
  max_types=match_nr;
  match_do[0] = ~match_do[0];
  match_do[1] = ~match_do[1];
  match_do[2] = ~match_do[2];
  TIMSK |= (1<<TOIE0);
}


ISR (TIMER0_OVF_vect)
{
  unsigned char sreg;
//  uart_send_var(count0);
//  uart_send_char(' ');
  sreg=SREG;
//  uart_send_var(match_count);
//  uart_send_string("\n\r");


  if(match_tmp[match_count] == count0)
  {
    PORTA &= match_do[match_count];
    match_count++;
    if(match_count >= max_types)
      match_count = 0;
  }
  if(count0 == 254)
  {
    count0 = 255;
    PORTA |= 0b00000111;
  }
  count0++;
  SREG=sreg;
}