#ifndef __FUNK_RX_H_
#define __FUNK_RX_H_ 1

char packet_rx( unsigned char* d, unsigned char l);

char packet_correct( unsigned char* d, unsigned char l);

void packet_evaluation( unsigned char* d, unsigned char l);
static const char wrong_dev_type[] PROGMEM = "Geraetetyp nicht programmiert!\n\r";


#endif