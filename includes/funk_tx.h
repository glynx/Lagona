#ifndef __FUNK_TX_H
#define __FUNK_TX_H 1

void send_ack( char dest );

unsigned char build_package(unsigned char destination, unsigned char datalength, struct funk_flags flags);

unsigned char waiting_for_ack = 0;

unsigned char tx_retry = 0;

unsigned char tx_package_length = 0;

unsigned char ack_timeout = 0;
#define		  def_ack_timeout 10

volatile unsigned char ack_timeout_count = 0;

unsigned char tx_failed = 0;
#define		  def_tx_failed 5
#endif