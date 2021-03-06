#ifndef __uart_h_
#define __uart_h_
#define BAUD 38400L      // Baudrate
 
// Berechnungen
#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)   // clever runden
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1)))     // Reale Baudrate
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD) // Fehler in Promille, 1000 = kein Fehler.
 
#if ((BAUD_ERROR<990) || (BAUD_ERROR>1010))
  #error Systematischer Fehler der Baudrate grösser 1% und damit zu hoch! 
#endif

#define UART_MAXSTRLEN 16
 
volatile uint8_t uart_str_complete = 0;     // 1 .. String komplett empfangen
volatile uint8_t uart_str_count = 0;
volatile uint8_t uart_str_size = 0;
volatile char uart_string[UART_MAXSTRLEN + 1] = "";
char temp[UART_MAXSTRLEN + 1] = "";




int uart_send_char( char x);
void uart_send_string( char* s);
void uart_send_pgm_string(PGM_P s);
void uart_send_var(int x);
void uart_send_var_string( unsigned char* s, char l);
void uart_send_var_zero_dec(int x, char l);
void uart_init(void);
int ascii_to_char( char* string);
//void uart_send_var_string(char *s, ...);

#endif