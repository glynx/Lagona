#include <includes/funk_tx.h>
#include <includes/funk_rx.h>
#include <includes/funk.h>
#include <main.h>


unsigned char build_package(unsigned char destination, unsigned char datalength, struct funk_flags flags)
{
  funk_write_data[0] = destination; //Ziel des Packetes
  funk_write_data[1] = device_mac; //Quelle des packetes
  char *pflags = (char *) &flags;
  funk_write_data[2] = *pflags;          //Flags
  unsigned char write_position = DATA_OFFSET+datalength;
  uint16_t check_sum = 0;
  funk_write_data[3] = write_position + 2; //laenge des packetes
  tx_package_length = funk_write_data[3];
  for(int i = 0; i < write_position; i++) //TODO: Modularisieren kommt vor in build_package und in packet_rx
    check_sum += funk_write_data[i];
  check_sum += 21930;
  funk_write_data[write_position] = (unsigned char) check_sum;
  write_position++;
  funk_write_data[write_position] = (unsigned char) (check_sum>>8);
  write_position++;
  
  funk_write_data[write_position] = '\0';
  uart_send_var_string((unsigned char *) funk_write_data, write_position);
  return write_position;
}
/*
  unsigned char write_position = DATA_OFFSET;
  while(write_position < FUNK_MAX && uart_string[write_position - 2] != 0 && (write_position - 2) < UART_MAXSTRLEN)
  {
    if(uart_string[write_position - 2] >= 0x30 && uart_string[write_position - 2] < 0x40)
      funk_write_data[write_position] = uart_string[write_position - 2] - 0x30;
    else
      funk_write_data[write_position] = uart_string[write_position - 2];
    write_position++;
  }*/

void send_ack(char dest)
{
  struct funk_flags flags;
  flags.is_ack = 1;
  funk_send_data((unsigned char *) funk_write_data, build_package(dest, 0, flags));
  uart_send_char('o');
}