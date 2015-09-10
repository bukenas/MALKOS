#ifndef UART_FUNCTIONS_H_
#define UART_FUNCTIONS_H_

#include <msp430.h>

//void write_char(char);
//void write_int(int);
//void write_int_digit(int);
//void write_float(float,int);
//void write_ascii(char);
//void write_array(char [], char);
//char write_string(char * s);
//void write_string2(char * a,int length);
char UART_receive_packet(char RXBUF, char *USARTReceiveBuffer);
void USARTSendChar(char character);
void Send_string(char* sdata, unsigned long del);

//Define UART PORT configuration
#define UART_FLAG_REG 		UCA1IFG
#define UART_TX_FLAG		UCTXIFG
#define UART_RX_FLAG		UCRXIFG
#define UART_TX_BUF		UCA1TXBUF
#define UART_RX_BUF		UCA1RXBUF

//UART_receive_packet function defines
#define USART_BUF_SIZE  	1024
#define MASK_SYMBOL  		0xEF
#define RX_START_SYMBOL 	0xC0
#define RX_STOP_SYMBOL 		0xC1
#define TX_START_SYMBOL 	0xC2
#define TX_STOP_SYMBOL 		0xC3


#endif /* UART_FUNCTIONS_H_ */

