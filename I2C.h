
#define NR3
////Defines the port to be used like:

#ifdef NR1
#define I2C_MASTER_REN P10REN
#define I2C_MASTER_DIR P10DIR
#define I2C_MASTER_OUT P10OUT
#define I2C_MASTER_IN  P10IN
#endif
#ifdef NR2
#define I2C_MASTER_REN P9REN
#define I2C_MASTER_DIR P9DIR
#define I2C_MASTER_OUT P9OUT
#define I2C_MASTER_IN  P9IN
#endif
#ifdef NR3
#define I2C_MASTER_REN P4REN
#define I2C_MASTER_DIR P4DIR
#define I2C_MASTER_OUT P4OUT
#define I2C_MASTER_IN  P4IN
#endif

//Start and Stop delay, most devices need this
#define I2C_MASTER_SDLY		0x05//0x20//
//for long lines or very fast MCLK, unremark and set
#define I2C_MASTER_DDLY		0x01
#define I2C_MASTER_UDLY         0x02

//port pins
#define I2C_MASTER_SCL BIT2
#define I2C_MASTER_SDA BIT1



extern unsigned char I2Cbuf[20];

unsigned char i2cm_out( register unsigned int data );
void i2cm_start( void );
void i2cm_stop( void );
void i2cm_in( unsigned char* buf, int count );

void I2C_Init(unsigned char slave_address,  unsigned char prescale);
void I2CWrite_Register(char reg_address, char new_value);
void I2CRead_Register(unsigned char reg_address, unsigned char number_of_bytes, unsigned char *readBuffer);