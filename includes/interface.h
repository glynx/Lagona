#ifndef __INTERFACE_H_
#define __INTERFACE_H_ 1
#include <avr/pgmspace.h>
unsigned char read_position = 0;
int set(void);
int set_rgb(void);
int get(void);
int auswertung(void);

void test(void);
static const char cmd_not_fnd[] PROGMEM = "Command not found: ";
#if funk == 1
void packet(void);
#if testing == 1
int packet_generator(void);
static const char pghelp[] PROGMEM = "Usage: pg for package generation\n\rd[xxx] destination adress in dec. Default 000\n\rn[xx] number of data packages. Default 08\n\rt[x] packet type. Default 1 = data!\n\r";
#endif
#endif

#endif