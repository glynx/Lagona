#include <avr/io.h>
#include <includes/uart.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <conf/controller.h>
#include <stdlib.h>


void uart_init(void)
{
  defUBRRH = UBRR_VAL >> 8;
  defUBRRL = UBRR_VAL & 0xFF;

  defUCSRB |= (1<<defRXEN)|(1<<defTXEN)|(1<<defRXCIE);  // UART RX, TX und RX Interrupt einschalten
  defUCSRC |= defSETURSEL|(1<<defUCSZ1)|(1<<defUCSZ0);  // Asynchron 8N1 
}


void uart_send_var(int x)
{
  char VarinDec[10];
  itoa(x, VarinDec, 10);
  uart_send_string(VarinDec);
}


int uart_send_char(char x)
{
  while (!(defUCSRA & (1<<defUDRE)))  /* warten bis Senden moeglich     */
  {      asm("nop");
  }
  defUDR=x;
  
  return 0;
}


void uart_send_string(char *s)
{
  while (*s)
  {
    uart_send_char(*s);
    s++;
  }
}

void uart_send_pgm_string( PGM_P s )
{
  char c;
  while( ( c = pgm_read_byte( s++ ) ) != '\0' ) {
    uart_send_char(c);
  }

}

void uart_send_var_zero_dec(int x, char l)
{
  char s = 1;
  char VarinDec[10];
  itoa(x, VarinDec, 10);
  for(; x > 9; s++)
    x = x/10;
  s = l - s;
  for(;s > 0; s--)
    uart_send_char('0');

  uart_send_string(VarinDec);
  
}

void uart_send_var_string(unsigned char *s, char l)
{
  if(l != 0)
  {
    for(;l > 0; l--)
    {
      
      uart_send_var_zero_dec(*s, 3);
      s++;
      uart_send_char(',');
      uart_send_char(' ');
    }
  }
  else
  {
    while (*s)
    {
      
      uart_send_var_zero_dec(*s, 3);
      s++;
      uart_send_char(' ');
      uart_send_char(',');
    }
  }
}

int ascii_to_char(char* string)
{
  unsigned char result;
  result= (string[0] - 0x30) * 10 + (string[1] - 0x30);
//  result = string[0];
  if(result < 60 && result >= 0)
    return result;
  else
    return -1;
}

ISR(defUARTVEC)
{
  unsigned char sreg;
  sreg=SREG;
  unsigned char nextChar;
 
  // Daten aus dem Puffer lesen
  nextChar = defUDR;
  if( uart_str_complete == 0 )
  {	// wenn uart_string gerade in Verwendung, neues Zeichen verwerfen
 
    // Daten werden erst in string geschrieben, wenn nicht String-Ende/max Zeichenlänge erreicht ist/string gerade verarbeitet wird
    if(nextChar != '\n' && nextChar != '\r' && uart_str_count < UART_MAXSTRLEN && nextChar != 127 )
    {
      uart_string[uart_str_count] = nextChar;
      uart_str_count++;
      uart_send_char(nextChar);
    }
    else
    {
      if(nextChar == '\r' || nextChar == '\n')
      {
	uart_send_char('\n');
	uart_send_char('\r');
	uart_string[uart_str_count] = '\0';
	uart_str_size = uart_str_count;
	uart_str_count = 0;
	uart_str_complete = 1;
      }
      else if(nextChar == 127  && uart_str_count > 0)
      {
	uart_str_count--;
      }
    }
  }
  else
    uart_send_string("nr"); //not Ready
  SREG=sreg;
}