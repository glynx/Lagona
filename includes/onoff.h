#ifndef __onoff_h_
#define __onoff_h_

void onoff_init(void);

void onoff_dev(char dev_num, char setto);

#define onoff_n_devices 1



#define onoff_dev0_PORT	PORTC

#define onoff_dev0_DDR	DDRC

#define onoff_dev0_PIN	PC1


//Strings

#if uart == 1

static const char wrong_dev[] PROGMEM = "onoff Device not found!\n\r";

#endif


#endif