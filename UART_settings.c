#include "UART_settings.h"


void UART_Setup(void)
{
    UCA1CTL1 |= UCSWRST;        // Put the USCI state machine in reset
    UCA1CTL1 |= UCSSEL_2;  		// Use SMCLK as the bit clock

    P4SEL |= BIT4 + BIT5;		// Configure these pins as TXD/RXD

    // Set the baudrate
    UCA1BR0 = UART_BR0;
    UCA1BR1 = UART_BR1;
    UCA1MCTL = (UART_BRF << 4) | (UART_BRS << 1) | (UART_OS);


    UCA1CTL1 &= ~UCSWRST;       				// Take the USCI out of reset
    UCA1IE |= UCRXIE;          // Enable the RX interrupt.  Now, when bytes are
    											// rcv'ed, the USCI_A1 vector will be generated.
    //while(!(UART_FLAG_REG & UART_TX_FLAG));     // TX buffer ready?
	//UART_TX_BUF='*';							// UART OK symbol
}
