#ifndef __MAIN_H_
#define __MAIN_H_

#include <avr/io.h>

//Einstellungen



//funktionen
#define uart 0
#define rgb 0
#define time 0
#define funk 0
#define interface 0
#define rs485 1

#if defined (__AVR_ATmega16__)
	//unsigned char device_mac = 1;
	#define onoff 0
#else
//unsigned char device_mac = 2;
#endif

#define testing 1

/*****
 * Prototypes
 *****/

void critical_error(uint8_t err);



//Pin Configs


#if defined (__AVR_ATmega16__)

//------------------------------------------------------------FUNK---------------------------------------------------------//
//Config für das rfm12
#if funk == 1
//select port
#define PORT_SEL PORTB
#define PIN_SEL PINB
#define DDR_SEL DDRB
#define RFXX_SEL 4

//sdi also das was von controller raus geht Master Out
#define PORT_SDI PORTB
#define PIN_SDI PINB
#define DDR_SDI DDRB
#define RFXX_SDI 5

//Master in
#define PORT_SDO PORTB
#define PIN_SDO PINB
#define DDR_SDO DDRB
#define RFXX_SDO 6

//SCK
#define PORT_SCK PORTB
#define PIN_SCK PINB
#define DDR_SCK DDRB
#define RFXX_SCK 7

//INT0
#define PORT_IRQ PORTD
#define PIN_IRQ PIND
#define DDR_IRQ DDRD
#define RF12_IRQ 2

//Data
#define PORT_DATA PORTB
#define PIN_DATA PINB
#define DDR_DATA DDRB
#define RF12_DATA PB2

//power module
#define PORT_POWER PORTB
#define PIN_POWER PINB
#define DDR_POWER DDRB
#define RF12_POWER 3


#endif



#elif defined (__AVR_ATmega48__) || defined (__AVR_ATmega48A__) || defined (__AVR_ATmega8__)

//------------------------------------------------------------FUNK---------------------------------------------------------//
//Config für das rfm12
#if funk == 1
//select port
#define PORT_SEL PORTB
#define PIN_SEL PINB
#define DDR_SEL DDRB
#define RFXX_SEL 2

//sdi also das was von controller raus geht Master Out
#define PORT_SDI PORTB
#define PIN_SDI PINB
#define DDR_SDI DDRB
#define RFXX_SDI 3

//Master in
#define PORT_SDO PORTB
#define PIN_SDO PINB
#define DDR_SDO DDRB
#define RFXX_SDO 4

//SCK
#define PORT_SCK PORTB
#define PIN_SCK PINB
#define DDR_SCK DDRB
#define RFXX_SCK 5

//INT0
#define PORT_IRQ PORTD
#define PIN_IRQ PIND
#define DDR_IRQ DDRD
#define RF12_IRQ 2

//Data
#define PORT_DATA PORTD
#define PIN_DATA PIND
#define DDR_DATA DDRD
#define RF12_DATA PD7

//power module
#define PORT_POWER PORTD
#define PIN_POWER PIND
#define DDR_POWER DDRD
#define RF12_POWER 4


#endif

#else
#  if !defined(__COMPILING_AVR_LIBC__)
#    warning "device type not defined"
#  endif
#endif

#endif
