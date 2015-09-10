#ifndef UART_SETTINGS_H_
#define UART_SETTINGS_H_

#include <msp430.h>

void UART_Setup(void);


//UART speed
#define UART_OS   0    // 1 = oversampling mode, 0 = low-freq mode
#define UART_BR0  9    // Value of UCA1BR0 register
#define UART_BR1  0    // Value of UCA1BR1 register
#define UART_BRS  1    // Value of UCBRS field in UCA1MCTL register
#define UART_BRF  0    // Value of UCBRF field in UCA1MCTL register


//UART configuration
//#define PARITY_EVEN_ENABLED
//#define PARITY_ODD_ENABLED
//#define TWO_STOP_BITS_ENABLED


#endif /* UART_SETTINGS_H_ */
