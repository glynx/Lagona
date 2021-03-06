#ifndef __FUNK_H_
#define __FUNK_H_ 1

#ifndef F_CPU
#warning "F_CPU war noch nicht definiert, wird nun mit 12MHz definiert"
#define F_CPU 12000000UL     /* Quarz mit 12 Mhz */
#endif
#include <util/delay.h>     /* in älteren avr-libc Versionen <avr/delay.h> */ 

#define FUNK_CACHE_SIZE 48
//Max 255
 
//volatile unsigned char funk_data_complete = 0;     // 1 .. Wenn alle daten empfangen wurden! --- Muss auf 0 wenn daten verarbeitet
//volatile unsigned char funk_data_count = 0;
//volatile unsigned char funk_data[FUNK_CACHE_SIZE] = "";




//Funktions aliases
//Funktionen für den select port
#define SEL_OUTPUT() DDR_SEL |= (1<<RFXX_SEL)
#define HI_SEL() PORT_SEL|= (1<<RFXX_SEL)
#define LOW_SEL() PORT_SEL&=~(1<<RFXX_SEL)


//Funktionen für sdi also das was von controller raus geht Master Out
#define SDI_OUTPUT() DDR_SDI |= (1<<RFXX_SDI)
#define HI_SDI() PORT_SDI|= (1<<RFXX_SDI)
#define LOW_SDI() PORT_SDI&=~(1<<RFXX_SDI)



//Master in
#define SDO_INPUT() DDR_SDO&= ~(1<<RFXX_SDO)
#define LOW_SDO() PORT_SDO&= (1<<RFXX_SDO)
#define SDO_HI() PIN_SDO&(1<<RFXX_SDO)


//SCK
#define SCK_OUTPUT() DDR_SCK |= (1<<RFXX_SCK)
#define HI_SCK() PORT_SCK|= (1<<RFXX_SCK)
#define LOW_SCK() PORT_SCK&=~(1<<RFXX_SCK)


//INT0
#define IRQ_IN() DDR_IRQ &=~(1<<RF12_IRQ) 
#define WAIT_IRQ_LOW() while(PIND&(1<<RF12_IRQ))


//Data
#define DATA_OUT() DDR_DATA|=1<<RF12_DATA
#define HI_DATA() PORT_DATA|=1<<RF12_DATA


//POWER
#define MODULE_POWER_OUT() DDR_POWER|=1<<RF12_POWER  //Setze Module-Power als Ausgang
#define MODULE_ON() PORT_POWER|=(1<<RF12_POWER)
#define MODULE_OFF() PORT_POWER&=~(1<<RF12_POWER)

/*
#define LED2_OUTPUT() DDRD|=((1<<5)|(1<<4))//+Module-Power
#define LED2_OFF() PORTD&=~(1<<5)
#define LED2_ON() PORTD|= (1<<5)
*/

//fifo
#define funk_enable_fifo() funk_cmd(0xCA83)
#define funk_disable_fifo() funk_cmd(0xCA81)
//Funktionen


void funk_init(void);

unsigned int funk_cmd(unsigned int aCmd);

unsigned char funk_receive(void);

void Delay_ms( unsigned char amS );

void funk_send_packet( unsigned char* d, unsigned char l );

void funk_send_data( unsigned char* d, unsigned char l );

#define FUNK_MAX 20

#define DATA_OFFSET 4 //1 source, 1 destination, 1 flags, 1 length... TODO: Auf 16 bit Adressen erweitern
volatile unsigned char funk_packets_to_read = 0;
volatile unsigned char funk_read_position = 0;
volatile unsigned char funk_data[FUNK_MAX];
volatile unsigned char funk_write_data[FUNK_MAX];
volatile unsigned char funk_data_complete = 0;
volatile unsigned char funk_data_length = 0;
volatile unsigned char funk_read_proces = 0;

typedef struct funk_flags {
  unsigned is_ack:1;
  unsigned need_ack:1;
  unsigned need_resend:1;
  unsigned bla:1;
  unsigned bla2:4;
} ff;
union ff_c
{
  ff flags;
  unsigned char c;
} ;


#endif