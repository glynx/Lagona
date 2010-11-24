#ifndef __TIME_H_
#define __TIME_H_ 1

#define OCR_VAL (F_CPU/1024/(1000/8)) //Die compare match variable fuer den timer1 wird berechnet!
#include <avr/interrupt.h>


void time_init(void); //Uhrzeit initialisierung immer am anfang!

void send_time_sync(void);
void time_sync( unsigned char* d);


volatile unsigned char _8ms = 0;
volatile unsigned char sekunde = 0;
volatile unsigned char minute = 0;
volatile unsigned char stunde = 0;

#endif