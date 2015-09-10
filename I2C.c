#include "I2C.h"
#include  <msp430x54xA.h>


unsigned char I2Cbuf[20];


static void inline brief_pause( register unsigned int n );
//unsigned char I2C_MASTER_SCL=BIT2;
//unsigned char I2C_MASTER_SDA=BIT3;
//static void inline brief_pause( register unsigned int n );

//Send data byte out on bang i2c, return false if ack
//Assumes start has been set up or a next byte
//so  both lines are assumed low
// **Lower byte of 'data' is sent**
unsigned char i2cm_out( register unsigned int data )
{
	volatile unsigned int i= 0; //will be register
	//output eight bits of 'data'
	for( ; i < 8; ++i )
	{
		//send the data bit
		if( data & 0x80 )
			I2C_MASTER_DIR&= ~I2C_MASTER_SDA;
		else
			I2C_MASTER_DIR|= I2C_MASTER_SDA;
		
		//shift next data bit
		data<<= 1;

//Toggle SCL, first high
		I2C_MASTER_DIR&=~I2C_MASTER_SCL;
		
#ifdef I2C_MASTER_UDLY
		brief_pause( I2C_MASTER_DDLY );
#endif //I2C_MASTER_UDLY

		//Set SCL Low
		I2C_MASTER_DIR|= I2C_MASTER_SCL;
	}

	//make sure SDA inputs
	I2C_MASTER_DIR&= ~I2C_MASTER_SDA;

	//Set  SCL High
	I2C_MASTER_DIR&= ~I2C_MASTER_SCL;

	//get the ack bit, true if ack
	unsigned int ack= !( I2C_MASTER_IN & I2C_MASTER_SDA );
	
	if( ack ) //leave SDA in last state!
		I2C_MASTER_DIR|= I2C_MASTER_SDA;

	//Set SCL Low
	I2C_MASTER_DIR|= I2C_MASTER_SCL;

	return ack;
}

//Assumes the IC2_MASTER_SCL is low
void i2cm_in( unsigned char* buf, int count )
{
	unsigned char data;
	for( ; count--; )
	{
		data= 0;
		//Release SDA
		I2C_MASTER_DIR&= ~I2C_MASTER_SDA;

		volatile unsigned int i= 0;
		for( ; i < 8; ++i )
		{
			//Set Clock High
		I2C_MASTER_DIR&= ~I2C_MASTER_SCL;
			brief_pause( I2C_MASTER_DDLY ); /////////////////////
			//shift the bit over
			data= data << 1;
		
			if( I2C_MASTER_IN & I2C_MASTER_SDA )
				data|= 0x01;
			
			//Set Clock Low
			I2C_MASTER_DIR|= I2C_MASTER_SCL;
		}
		//put the input data byte into the buffer, inc buffer pointer
		*buf++= data;
		
		//No Ack after last byte
		if( count )
			I2C_MASTER_DIR|= I2C_MASTER_SDA;

//Toggle SCL, first high
		I2C_MASTER_DIR&= ~I2C_MASTER_SCL;
		
#ifdef I2C_MASTER_UDLY
		brief_pause( I2C_MASTER_DDLY );
#endif //I2C_MASTER_UDLY

		//Set SCL Low
		I2C_MASTER_DIR|= I2C_MASTER_SCL;
	}
}

void i2cm_start( void )
{
	//Make sure both pins are high
	I2C_MASTER_DIR&= ~( I2C_MASTER_SDA | I2C_MASTER_SCL );
       
	//Set output low here so'I2C Master Init' is not needed
	I2C_MASTER_OUT&= ~( I2C_MASTER_SDA | I2C_MASTER_SCL );
	//Set SDALow, pause in case both pins were not high
    brief_pause( I2C_MASTER_SDLY );
	I2C_MASTER_DIR|= I2C_MASTER_SDA;
	
	//Set SCL Low
    brief_pause( I2C_MASTER_SDLY );
	I2C_MASTER_DIR|= I2C_MASTER_SCL;
}

//Assumes SCL is low
void i2cm_stop( void )
{
	//make sure SDA is low
	I2C_MASTER_DIR|= I2C_MASTER_SDA;
	
	//SCL to high, pause in case SDA was high
    brief_pause( I2C_MASTER_SDLY );
	I2C_MASTER_DIR&= ~I2C_MASTER_SCL;
	//SDA to high
    brief_pause( I2C_MASTER_SDLY );
	I2C_MASTER_DIR&= ~I2C_MASTER_SDA;
}



void I2C_Init(unsigned char slave_address,  unsigned char prescale) {
	P4SEL |= BIT1 + BIT2;                 		// Assign I2C pins to USCI
	UCB1CTL1 = UCSWRST;                         // Enable SW reset
	UCB1CTL0 = UCMST + UCMODE_3 + UCSYNC;       // I2C Master, synchronous mode
	UCB1CTL1 = UCSSEL_2 + UCSWRST;              // Use SMCLK, keep SW reset
	UCB1BR0 = prescale;                         // set prescaler
	UCB1BR1 = 0;
	UCB1I2CSA = slave_address;                  // set slave address
	UCB1CTL1 &= ~UCSWRST;                       // Clear SW reset, resume operation
}

void I2CWrite_Register(char reg_address, char new_value)
{
	UCB1CTL1 |= UCTR + UCTXSTT;
	while(!(UCB1IFG & UCTXIFG));
	UCB1TXBUF = reg_address;
	while(!(UCB1IFG & UCTXIFG));
	UCB1TXBUF = new_value;
	while(!(UCB1IFG & UCTXIFG));
	UCB1CTL1 |= UCTXSTP;
	while (UCB1CTL1 & UCTXSTP);
}

void I2CRead_Register(unsigned char reg_address, unsigned char number_of_bytes, unsigned char *readBuffer)
{
	UCB1CTL1 |= UCTR + UCTXSTT;
	while(!(UCB1IFG & UCTXIFG));
	UCB1TXBUF = reg_address;
	while(!(UCB1IFG & UCTXIFG));
	UCB1CTL1 |= UCTXSTP;
	while (UCB1CTL1 & UCTXSTP);

	UCB1CTL1 &= ~UCTR;
	UCB1CTL1 |= UCTXSTT;

	unsigned char cnt;
	for(cnt=0; cnt<number_of_bytes; cnt++)
	{
		while(!(UCB1IFG & UCRXIFG));
		readBuffer[cnt] = UCB1RXBUF;
	}
	UCB1CTL1 |= UCTXSTP;
	while (UCB1CTL1 & UCTXSTP);
	char lol = UCB1RXBUF;
}

static void inline brief_pause( register unsigned int n )
{
for(int i=0;i<n;i++)
  i*i;
}