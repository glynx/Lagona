#ifndef __CONTROLLER_H_
#define __CONTROLLER_H_

#if defined (__AVR_ATmega48__) || defined (__AVR_ATmega48A__)

//##################################### ATMEGA 48 #############################################

//################### UART #######################
#define defRXEN		RXEN0	//Default RX enable
#define defTXEN		TXEN0	//Default TX enable
#define defRXCIE	RXCIE0 	//Default RX interrupt enable
#define defUCSRB	UCSR0B	//Default Uart Status Register B
#define defUCSRC	UCSR0C	//Default Uart Status Register C
#define defURSE		URSEL	//Default Register Select
#define defSETURSEL 	0
#define defUCSZ0	UCSZ00	//Default Uart Character Size 0
#define defUCSZ1	UCSZ01	//Default Uart Character Size 1
#define defUCSZ2	UCSZ02	//Default Uart Character Size 2
#define defUBRRH	UBRR0H	//Default Uart Baudrate Register High
#define defUBRRL	UBRR0L	//Default Uart Baudrate Register Low
#define defUDR		UDR0
#define defUCSRA	UCSR0A
#define defUDRE		UDRE0
#define defUARTVEC	USART_RX_vect //Default Uart IRQ Vektor
#define defPE		PE
#define defFE		FE
#define defDOR		DOR
#define defU2X		U2X
#define defMPCM		MPCM
#define defUARTTXTEV USART_TX_vect
#define defRXCIE	RXCIE0
#define defRXB8		RXB8
#define defTXB8		TXB8
#define defUMSEL	UMSEL
#define defUPM0		UPM0
#define defUPM1		UPM1
#define defUSBS		USBS

//################### TIME #######################

#define defTCCRB	TCCR0B	//Default Timer Counter Control Register
#define defTCCRA	TCCR0A  //Default Timer Counter Control Register
#define defCS0		CS00	//Default Counter divider Bit0
#define defCS1		CS01	//Default Counter divider Bit1
#define defCS2		CS02	//Default Counter divider Bit2
#define defWGM1		WGM01	//Default Waveform Generator mode bit 2
#define defCOM1		COM0A1
#define defOCR		OCR0A	//Default Overflow Controll Register ....
//#define defOCRL		OCR1AL	//Default Overflow Controll Register LOW
#define defTIMSK	TIMSK0	//Default Timercounter Interrupt Mask  
#define defOCIE		OCIE0A	//Default Enable Counter Overflow bit
#define defTIMEVEC	TIMER0_COMPA_vect //Default Time/Clock Vektor



#elif defined (__AVR_ATmega8__)

//##################################### ATMEGA 8 ##############################################


//################### UART #######################
#define defRXEN RXEN		//Default RX enable
#define defTXEN TXEN  	//Default TX enable
#define defRXCIE RXCIE 	//Default RX interrupt enable
#define defUDRIE UDRIE	// UDR empty interrupt enable
#define defUCSRB UCSRB		//Default Uart Status Register B
#define defUCSRC UCSRC		//Default Uart Status Register C
#define defURSEL URSEL	//Default Register Select
#define defSETURSEL (1<<defURSEL)
#define defUCSZ0 UCSZ0	//Default Uart Character Size 0
#define defUCSZ1 UCSZ1	//Default Uart Character Size 1
#define defUCSZ2 UCSZ2	//Default Uart Character Size 2
#define defUBRRH UBRRH	//Default Uart Baudrate Register High
#define defUBRRL UBRRL	//Default Uart Baudrate Register Low
#define defUDR UDR
#define defUCSRA UCSRA
#define defUDRE UDRE
#define defUARTVEC	USART_RXC_vect //Default Uart IRQ Vektor
#define defPE		PE
#define defFE		FE
#define defDOR		DOR
#define defU2X		U2X
#define defMPCM		MPCM
#define defUARTTXTEV USART_TXC_vect
#define defTXCIE	TXCIE
#define defRXB8		RXB8
#define defTXB8		TXB8
#define defUMSEL	UMSEL
#define defUPM0		UPM0
#define defUPM1		UPM1
#define defUSBS		USBS

//################### TIME #######################

#define defTCCRA	TCCR1A   //Default Timer Counter Control Register
#define defTCCRB	TCCR1B	//Default Timer Counter Control Register
#define defCS0		CS10	//Default Counter divider Bit0
#define defCS1		CS11	//Default Counter divider Bit1
#define defCS2		CS12	//Default Counter divider Bit2
#define defWGM1		WGM01	//Default Waveform Generator mode bit 2
#define defOCR		OCR1A	//Default on compare value register
#define defCOM1		COM01
#define defTIMSK	TIMSK	//Default Timercounter Interrupt Mask  
#define defOCIE		OCIE1A	//Default Enable Counter Compare interrupt bit
#define defTIMEVEC	TIMER1_COMPA_vect //Default Time/Clock vector
#define defOVFLVEC	TIMER1_OVF_vect // Default Time/Clock overflow vector
#define defTIME		TCNT1	// Default Time/Clock value
#define defCTCBit	WGM12


//##################################### ATMEGA 16 #############################################


//################### UART #######################
#elif defined (__AVR_ATmega16__)
#define defRXEN		RXEN	//Default RX enable
#define defTXEN		TXEN	//Default TX enable
#define defRXCIE	RXCIE 	//Default RX interrupt enable
#define defUCSRB	UCSRB	//Default Uart Status Register B
#define defUCSRC	UCSRC	//Default Uart Status Register C
#define defURSEL	URSEL	//Default Register Select
#define defSETURSEL	(1<<defURSEL)
#define defUCSZ0	UCSZ0	//Default Uart Character Size 0
#define defUCSZ1	UCSZ1	//Default Uart Character Size 1
#define defUCSZ2	UCSZ2	//Default Uart Character Size 2
#define defUBRRH	UBRRH	//Default Uart Baudrate Register High
#define defUBRRL	UBRRL	//Default Uart Baudrate Register Low
#define defUDR		UDR
#define defUCSRA	UCSRA
#define defUDRE		UDRE
#define defUARTVEC	USART_RXC_vect //Default Uart IRQ Vektor
#define defPE		PE
#define defFE		FE
#define defDOR		DOR
#define defU2X		U2X
#define defMPCM		MPCM
#define defUARTTXTEV USART_TXC_vect
#define defRXCIE	RXCIE
#define defRXB8		RXB8
#define defTXB8		TXB8
#define defUMSEL	UMSEL
#define defUPM0		UPM0
#define defUPM1		UPM1
#define defUSBS		USBS

//################### TIME #######################

#define defTCCRB	TCCR0	//Default Timer Counter Control Register
#define defTCCRA	TCCR0   //Default Timer Counter Control Register
#define defCS0		CS00	//Default Counter divider Bit0
#define defCS1		CS01	//Default Counter divider Bit1
#define defCS2		CS02	//Default Counter divider Bit2
#define defWGM1		WGM01	//Default Waveform Generator mode bit 2
#define defCOM1		COM01
#define defOCR		OCR0	//Default Overflow Controll Register High
//#define defOCRL		OCR1AL	//Default Overflow Controll Register LOW
#define defTIMSK	TIMSK	//Default Timercounter Interrupt Mask  
#define defOCIE		OCIE0	//Default Enable Counter Overflow bit
#define defTIMEVEC	TIMER0_COMP_vect //Default Time/Clock Vektor

/*
#elif defined (__AVR_ATmega48__) || defined (__AVR_ATmega48A__)
void time_init(void)
{
  // Timer 0 konfigurieren
  TCCR1B |= (1<<CS12)|(1<<CS10)|(1<<WGM12); // Prescaler 8
  // ((1000000/8)/1000) = 125
  

  OCR1AH = OCR_VAL >> 8;
  OCR1AL = OCR_VAL & 0xFF;
 
  // Compare Interrupt erlauben
  TIMSK1 |= (1<<OCIE1A);
 
}*/
#else
#  if !defined(__COMPILING_AVR_LIBC__)
#    warning "device type not defined"
#  endif
#endif

#endif
