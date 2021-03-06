#include <includes/funk.h>
#include <includes/uart.h>
#include <includes/funk_rx.c>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>

void funk_init(void)
{
  MODULE_POWER_OUT();
  MODULE_OFF();
  Delay_ms(50);
  MODULE_ON();
  Delay_ms(20);
  //int0 vorbereiten!

#if defined (__AVR_ATmega8__)
  MCUCR |= (1<<ISC01); // | (1<<ISC00); //int0 auf fallende flanke setzen
  GIMSK |= (1<<INT0);  //int0 aktivieren
#elif defined (__AVR_ATmega16__)
  MCUCR |= (1<<ISC01); //int0 auf fallende flanke setzen
  GICR |= (1<<INT0);  //int0 aktivieren
#elif defined (__AVR_ATmega48__) || defined (__AVR_ATmega48A__)
  EICRA |= (1<<ISC01); //int0 auf fallender flanke
  EIMSK |= (1<<INT0);  //int0 aktivieren
#else
#  if !defined(__COMPILING_AVR_LIBC__)
#    warning "device type not defined for funk_init"
#  endif
#endif


  HI_SEL();
  HI_SDI();
  LOW_SCK();
  HI_DATA(); //SET nFFS pin HI when using FIFO
  SEL_OUTPUT();
  SDI_OUTPUT();
  SDO_INPUT();
  SCK_OUTPUT();
  IRQ_IN();
  //IRQ_PU();
  DATA_OUT();

  
  funk_cmd(0x80D7);//EL,EF,11.5pF
  funk_cmd(0x82F9);//er,!ebb,ET,ES,EX,!eb,!ew,DC --- funk_cmd(0x8279);//!er,!ebb,ET,ES,EX,!eb,!ew,DC --- funk_cmd(0x82D9);//er,!ebb,!ET,ES,EX,!eb,!ew,DC
  funk_cmd(0xA640);//434MHz
  funk_cmd(0xC647);//4.8kbps
  funk_cmd(0x94A0);//VDI,FAST,134kHz,0dBm,-103dBm
  funk_cmd(0xC2AC);//AL,!ml,DIG,DQD4
  funk_cmd(0xCA81);//FIFO8,SYNC,!ff,DR
  funk_cmd(0xC483);//@PWR,NO RSTRIC,!st,!fi,OE,EN
  funk_cmd(0x9850);//!mp,9810=30kHz,MAX OUT
  funk_cmd(0xE000);//NOT USE
  funk_cmd(0xC800);//NOT USE
  funk_cmd(0xC400);//1.66MHz,2.2V
  

}

unsigned int funk_cmd(unsigned int aCmd)
{
  unsigned char i;
  unsigned int temp;
  temp=0;
  LOW_SCK();
  LOW_SEL();
  for(i=0;i<16;i++)
  {
    if(aCmd&0x8000)
    {
      HI_SDI();
    }
    else
    {
      LOW_SDI();
    }
    HI_SCK();
    temp<<=1;
    if(SDO_HI())
    {
      temp|=0x0001;
    }
    LOW_SCK();
    aCmd<<=1;
  };
  HI_SEL();
  return(temp);
}

unsigned char funk_receive(void)
{
  unsigned int FIFO_data;
  //WAIT_IRQ_LOW();
  funk_cmd(0x0000);
  FIFO_data=funk_cmd(0xB000);
  return(FIFO_data&0x00FF);
}

void Delay_ms(unsigned char amS)
{
  unsigned char i;
  for(i=0;i<amS;i++)
  {
    _delay_ms(10);
  }
}

void funk_send(unsigned char aByte){
  while(PIN_IRQ & (1<<RF12_IRQ)); //wait for previously TX over
  funk_cmd(0xB800+aByte);
  _delay_ms(1);
}

void funk_send_packet( unsigned char* d, unsigned char l)
{
  while(funk_read_proces)
    uart_send_var(PIND&(1<<PD2));
  uart_send_char('p');
  //uart_send_char('1');
#if defined (__AVR_ATmega8__)
  GIMSK &= ~(1<<INT0);  //int0 deaktivieren
#elif defined (__AVR_ATmega16__)
  GICR  &= ~(1<<INT0);  //int0 deaktivieren
#elif defined (__AVR_ATmega48__) || defined (__AVR_ATmega48A__)
  EIMSK &= ~(1<<INT0);  //int0 deaktivieren
#else
#  if !defined(__COMPILING_AVR_LIBC__)
#    warning "device type not defined for funk_init"
#  endif
#endif

  funk_cmd(0x0000);//read status register
  funk_cmd(0x8239); //enable transmit
  funk_send(0xAA);//PREAMBLE
  funk_send(0xAA);//PREAMBLE
  funk_send(0xAA);//PREAMBLE
  funk_send(0x2D);//SYNC HI BYTE
  funk_send(0xD4);//SYNC LOW BYTE
  for(unsigned char i = 0; i < l; i++)
  {
    funk_send(d[i]);
  }
  funk_send(0xAA);//DUMMY BYTE
  funk_send(0xAA);//DUMMY BYTE
  funk_send(0xAA);//DUMMY BYTE
  funk_cmd(0x82F9); //enable read
  funk_disable_fifo();
  funk_enable_fifo();
  funk_cmd(0x0000);

#if defined (__AVR_ATmega8__)
  GIMSK |= (1<<INT0);  //int0 aktivieren TODO: Hier nochmal das flag cleanen!
#elif defined (__AVR_ATmega16__)
  GIFR = (1<<INTF0);
  GICR |= (1<<INT0);  //int0 aktivieren
#elif defined (__AVR_ATmega48__) || defined (__AVR_ATmega48A__)
  EIFR = (1<<INTF0);
  EIMSK |= (1<<INT0);  //int0 aktivieren
#else
#  if !defined(__COMPILING_AVR_LIBC__)
#    warning "device type not defined for funk_init"
#  endif
#endif
}

void funk_send_data( unsigned char* d, unsigned char l)
{
  while(funk_read_proces)
    uart_send_var(PIND&(1<<PD2));
  uart_send_char('p');
  //uart_send_char('1');
#if defined (__AVR_ATmega8__)
  GIMSK &= ~(1<<INT0);  //int0 deaktivieren
#elif defined (__AVR_ATmega16__)
  GICR  &= ~(1<<INT0);  //int0 deaktivieren
#elif defined (__AVR_ATmega48__) || defined (__AVR_ATmega48A__)
  EIMSK &= ~(1<<INT0);  //int0 deaktivieren
#else
#  if !defined(__COMPILING_AVR_LIBC__)
#    warning "device type not defined for funk_init"
#  endif
#endif

  //PORT_DATA |= (1<<RF12_DATA);// SET nFFS pin HI when using TX register
  funk_cmd(0x0000);//read status register
  funk_cmd(0x8239); //enable transmit
  funk_send(0xAA);//PREAMBLE
  funk_send(0xAA);//PREAMBLE
  funk_send(0xAA);//PREAMBLE
  funk_send(0x2D);//SYNC HI BYTE
  funk_send(0xD4);//SYNC LOW BYTE
  for(unsigned char i = 0; i < l; i++)
  {
    funk_send(d[i]);
  }
  while(l < 16)
  {
    funk_send(0xAA);//DUMMY BYTE
    l++;
  }
  funk_send(0xAA);//DUMMY BYTE
  funk_send(0xAA);//DUMMY BYTE
  funk_send(0xAA);//DUMMY BYTE
  funk_cmd(0x82F9); //enable read
  funk_disable_fifo();
  funk_enable_fifo();
  funk_cmd(0x0000);
  //TIFR = INT0;
#if defined (__AVR_ATmega8__)
  GIMSK |= (1<<INT0);  //int0 aktivieren TODO: Hier nochmal das flag cleanen!
#elif defined (__AVR_ATmega16__)
  GIFR = (1<<INTF0);
  //uart_send_var(GIFR);
  GICR |= (1<<INT0);  //int0 aktivieren
#elif defined (__AVR_ATmega48__) || defined (__AVR_ATmega48A__)
 // uart_send_string("\n\r");
  //uart_send_var(EIFR);
  //uart_send_var(PIND&(1<<PD2)>>PD2);
  //uart_send_string("\n\r");
  EIFR = (1<<INTF0);
  //uart_send_var(EIFR);
  EIMSK |= (1<<INT0);  //int0 aktivieren
  //uart_send_char('1');

  ff_c a;
  a.c = d[2];
  if(a.flags.need_ack == 1)
  {
	make_wait_for_ack(d, l, def_ack_timeout, def_tx_failed);
  }

#else
#  if !defined(__COMPILING_AVR_LIBC__)
#    warning "device type not defined for funk_init"
#  endif
#endif
}

void make_wait_for_ack(unsigned char * d, unsigned char l, unsigned char timeout, unsigned char retries)
{
	ack_timeout = timeout;
	tx_retries = retries;
	tx_package_length = l;
	waiting_for_ack = 1;
}

ISR(INT0_vect)
{
  unsigned char sreg;
  sreg=SREG;
  unsigned char temp;
//  uart_send_char('f');
//  uart_send_var(PIND&(1<<PD2)>>PD2);
  if(!funk_data_complete) //der uC uberprueft ob alle daten verarbeitet wurden
  {
    temp = funk_receive();
    if((funk_read_proces == 1) && (funk_read_position < 4 || funk_read_position < funk_packets_to_read))
    {
      if(funk_read_position == 3)
      {
	funk_packets_to_read = temp;
      }
      funk_data[funk_read_position] = temp;
      funk_read_position++;
      
      if(funk_read_position >= funk_packets_to_read && funk_read_position > 4)
      {
	funk_data_complete = 1;
	funk_data_length = funk_read_position;
	funk_read_proces = 0;
	funk_read_position = 0;
	funk_disable_fifo();
	funk_enable_fifo();
	funk_cmd(0x0000);
      }
      

    }
    else if(funk_read_proces == 0)
    {
      if(temp == device_mac || temp == 255)
      {
	funk_data[funk_read_position] = temp;
	funk_read_proces = 1;
	funk_read_position++;
      }
      else
      {
	funk_disable_fifo();
	funk_enable_fifo();
	funk_cmd(0x0000);
      }
    }
    else
    {
      funk_data_complete = 1;
      funk_data_length = funk_packets_to_read;
      funk_read_position = 0;
      funk_disable_fifo();
      funk_enable_fifo();
      funk_cmd(0x0000);
      funk_read_proces = 0; //erlaube wieder zu senden
    }
  }
  else
  {
    funk_disable_fifo();
    funk_enable_fifo();
    funk_cmd(0x0000);
  }
  SREG=sreg;

}
