#include <includes/interface.h>
#include <includes/funk.h>
#include <includes/funk_tx.h>
#include <includes/uart.h>
#include <includes/time.h>
#include <main.h>
#include <avr/pgmspace.h>

int auswertung()
{
  int result = -1;

  switch(uart_string[0])
  {

    case 's':
      result=set();
      break;

    case 'g':
      result=get();
      break;
    case 'c':
      send_time_sync();
      break;
#if funk == 1
    case 'p':
      packet();
      break;
#endif
#if testing == 1
    case 't':
      test();
      break;
#endif
    default:
      uart_send_pgm_string(cmd_not_fnd);
      uart_send_char(uart_string[0]);
      uart_send_string("\n\r\0");
  }
  return result;

}

void test(void)
{
  unsigned char write_position = DATA_OFFSET;
  while((write_position+3) < FUNK_MAX && uart_string[write_position - 2] != 0 && (write_position - 2) < UART_MAXSTRLEN)
  {
    if(uart_string[write_position - 2] >= 0x30 && uart_string[write_position - 2] < 0x40)
      funk_write_data[write_position] = uart_string[write_position - 2] - 0x30;
    else
      funk_write_data[write_position] = uart_string[write_position - 2];
    write_position++;
  }
  struct funk_flags t;
  t.need_ack = 1;
  funk_send_data( (unsigned char *) funk_write_data, build_package(uart_string[1] - 0x30, write_position - DATA_OFFSET + 1, t));
  
}

/*
void test(void)
{
  funk_write_data[0] = uart_string[1] - 0x30; //Ziel des Packetes
  funk_write_data[1] = device_mac; //Quelle des packetes
  funk_write_data[2] = 0;          //Flags
  unsigned char write_position = DATA_OFFSET;
  while((write_position+3) < FUNK_MAX && uart_string[write_position - 2] != 0 && (write_position - 2) < UART_MAXSTRLEN)
  {
    if(uart_string[write_position - 2] >= 0x30 && uart_string[write_position - 2] < 0x40)
      funk_write_data[write_position] = uart_string[write_position - 2] - 0x30;
    else
      funk_write_data[write_position] = uart_string[write_position - 2];
    write_position++;
  }

  funk_write_data[3] = write_position;
  funk_write_data[write_position] = '\0';
  uart_send_var_string((char *) funk_write_data, write_position);
  funk_send_data((char *) funk_write_data, write_position);
}*/
//------------------------------------------------Funktionen fuer den gebrauch von Funk-----------------------------------------------------
#if funk == 1
void packet()
{
  read_position = 1;
  if(uart_string[read_position] == 'h')
  {
    uart_send_pgm_string(pghelp);
  }
  else
#if testing == 1
  if(uart_string[read_position] == 'g')
  {
    read_position++;
    packet_generator();
  }
  else
#endif
  {
    uart_send_pgm_string(pghelp);
  }
  read_position = 0;
}
#if testing == 1
int packet_generator()
{
  unsigned char generator[3] = {0, 255, 255};
  unsigned char g_type = 0; //0 = ziel, 1 = laenge, 2 = type 
  while(uart_string[read_position] != '\0' && read_position < UART_MAXSTRLEN)
  {
    if(uart_string[read_position] == 'd')
    {
      g_type = 0;
      generator[0] = 0;
    }
    else if(uart_string[read_position] == 'n')
    {
      g_type = 1;
      generator[1] = 0;
    }
    else if(uart_string[read_position] == 't')
    {
      g_type = 2;
      generator[2] = 0;
    }
    else if(uart_string[read_position] < 0x3A && uart_string[read_position] > 0x29)
    {
      generator[g_type] *= 10;
      generator[g_type] += (uart_string[read_position] - 0x30);
    }
    else
    {
      static const char wrong_command[] PROGMEM = "Wrong command!\n\r";
      uart_send_pgm_string(wrong_command);
      uart_send_pgm_string(pghelp);
      return -1;
    }
    read_position++;
  }
  
  if(generator[1] > (FUNK_MAX - 3))
    generator[1] = 16;

  if(generator[2] > 7)
    generator[2] = 1;
  
  static const char packet1[] PROGMEM = "Das generierte packet: \n\rPacket Ziel:";
  uart_send_pgm_string(packet1);

  uart_send_var(generator[0]);

  static const char packet2[] PROGMEM = "\n\rPacket Laenge:";
  uart_send_pgm_string(packet2);
  uart_send_var(generator[1]);

  static const char packet3[] PROGMEM = "\n\rPacket Typ:";
  uart_send_pgm_string(packet3);
  uart_send_var(generator[2]);

  read_position = 0;
  //funk_write_data[0] = generator[0]; // Erstes byte => Adresse
  funk_write_data[0] = generator[0];
  for(read_position = 0 ; read_position < generator[1]; read_position++)
  {
    funk_write_data[read_position + 1] = read_position + pfb;
  }
  pfb++;
  funk_send_data((unsigned char *) funk_write_data, generator[1]);
  return 0;
}

#endif
#endif

//------------------------------------------------Funktionen fuer das setzen von werten-----------------------------------------------------
int set()
{
  static const char set_help[] PROGMEM = "Usage: s\n\rt[hh:mm:ss] set time\n\r";
  if(uart_string[1] == 'h')
  {
    uart_send_pgm_string(set_help);
  }
#if rgb == 1
  if(uart_string[1] == 'c')
    set_rgb();
  else
#endif
#if time == 1
  if(uart_string[1] == 't')
  {
    stunde = ascii_to_char((char *) &uart_string[2]);
    minute = ascii_to_char((char *) &uart_string[5]);
    sekunde = ascii_to_char((char *) &uart_string[8]);
  }
  else
#endif
  {
    uart_send_pgm_string(set_help);
    return -1;
  }
  return 0;
}


#if rgb == 1
int set_rgb()
{
  unsigned char color;
  unsigned char i;
  color = uart_string[2];
  if(color == 'R')
    match[0] = (uart_string[3] - 0x30) * 100 + (uart_string[4] - 0x30) * 10 + (uart_string[5] - 0x30);
  else if(color == 'G')
    match[1] = (uart_string[3] - 0x30) * 100 + (uart_string[4] - 0x30) * 10 + (uart_string[5] - 0x30);
  else if(color == 'B')
    match[2] = (uart_string[3] - 0x30) * 100 + (uart_string[4] - 0x30) * 10 + (uart_string[5] - 0x30);
  else
  {
    uart_send_string("Fehler\n\r");
    return -1;
  }
  i = 6;
  uart_send_char(uart_string[i]);
  uart_send_char(uart_string[i+1]);
  uart_send_char(uart_string[i+2]);
  uart_send_char(uart_string[i+3]);
  uart_send_var((uart_string[i+1] - 0x30) * 100 + (uart_string[i+2] - 0x30) * 10 + (uart_string[i+3] - 0x30));
  if(uart_string[i] == 'R' || uart_string[i] == 'G' || uart_string[i] == 'B')
  {
    uart_send_char('r');
    if(uart_string[i] == 'R')
    {
      match[0] = (uart_string[i+1] - 0x30) * 100 + (uart_string[i+2] - 0x30) * 10 + (uart_string[i+3] - 0x30);
      uart_send_var((uart_string[i+1] - 0x30) * 100 + (uart_string[i+2] - 0x30) * 10 + (uart_string[i+3] - 0x30));
    }
    else if(uart_string[i] == 'G')
      match[1] = (uart_string[i+1] - 0x30) * 100 + (uart_string[i+2] - 0x30) * 10 + (uart_string[i+3] - 0x30);
    else
      match[2] = (uart_string[i+1] - 0x30) * 100 + (uart_string[i+2] - 0x30) * 10 + (uart_string[i+3] - 0x30);
  i=i+4;
  }
  sort_matches();
}
#endif

//-------------------------------------------------------Funktionen fuer das auslesen von Werten------------------------------------------------------
int get()
{
  static const char get_help[] PROGMEM = "Usage: g\n\rt for get time\n\rc for geting color\n\r";
  if(uart_string[1] == 'h')
  {
    uart_send_pgm_string(get_help);
  }
  else
#if rgb == 1
  if(uart_string[1] == 'c')
  {
    uart_send_string("R: ");
    uart_send_var(match[0]);
    uart_send_string(", G: ");
    uart_send_var(match[1]);
    uart_send_string(", B: ");
    uart_send_var(match[2]);
    uart_send_string("R: ");
    uart_send_var(match_tmp[0]);
    uart_send_string(", G: ");
    uart_send_var(match_tmp[1]);
    uart_send_string(", B: ");
    uart_send_var(match_tmp[2]);
  }
  else
#endif
#if time == 1
    if(uart_string[1] == 't')
  {
    uart_send_var_zero_dec(stunde, 2);
    uart_send_char(':');
    uart_send_var_zero_dec(minute, 2);
    uart_send_char(':');
    uart_send_var_zero_dec(sekunde, 2);
    uart_send_char('\n');
    uart_send_char('\r');
  }
  else
#endif
  {
    uart_send_pgm_string(get_help);
    return -1;
  }

  return 0;
}