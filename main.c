#include <msp430f5308.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "flash.h"
#include "main.h"
#include "I2C.h"
#include "UART_settings.h"
#include "UART_functions.h"
#include "M95.h"
#include <math.h>

unsigned char I2Csend(unsigned char addr, unsigned char val);
void Clear_flags();
void write_i2c(unsigned char reg_add,unsigned char data);
void measure_acceleration (void);
void fix_acceleration();
float compare_acc();
unsigned long bufferSize=0;
unsigned char USARTReceiveBuffer[USART_BUF_SIZE];
unsigned char kiek=0, index=0;
unsigned char I2Cbuf1[6];
long x, y, z;
float delt;
unsigned char alarm_flag=0;
char SMS_message[100];
//char str[20];
char state=5, one_time_flag=0, rtc_flag=0;
long xf,yf,zf;
float cosx, cosy, cosxf, cosyf;
float modulis, deltaf;
float mod;
const char *ptra;
unsigned short pt1;

int main(void) {
   // WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    WDTCTL = WDT_ARST_1000;
    
    TA1CCR0 = 65535;
    TA1CTL = TASSEL_2 + MC_2 + TACLR;         // SMCLK, contmode, clear TAR
    TA1CCTL0 = CCIE; //enable timer interrupt
	
    UART_Setup();

    _EINT();
    P1DIR &= ~ BIT7;
    P2DIR |= BIT0;

    P1IES &= ~BIT1;                           // P1.4 Lo/Hi edge
    P1IFG &= ~BIT1;                           // P1.4 IFG cleared
    P1IE |= BIT1;                             // P1.4 interrupt enabled
  
    
    P4DIR |=BIT7;
    P4OUT |=BIT7;
    
    I2C_Init(0x18,16);
    __delay_cycles(200000);
     
    I2CWrite_Register(0x20, 0x27);
    I2CWrite_Register(0x21, 0x01);
    I2CWrite_Register(0x22, 0x40);
    I2CWrite_Register(0x25, 0x40);
    I2CWrite_Register(0x23, 0x88);
    I2CWrite_Register(0x24, 0x80);
    
    I2CWrite_Register(0x32, 0x06); //threshodl
    I2CWrite_Register(0x33, 0x01); // duration
    I2CWrite_Register(0x30, 0x95);
    I2CWrite_Register(0x38, 0x15);
    
   // Send_string("333\n\n",2);
//    write_2_flash_string("333333",0xff50,6);
//    
//    
//    while(1);
    
    unsigned char *fla = (unsigned char*)(ADDRESS+SLOT+6); // nuskaitomi flagai is flasho
    SMS_flag=fla[0];
    Ring_flag=fla[1];
    if(SMS_flag!=0 || SMS_flag!=1)
      SMS_flag=1;
    if(Ring_flag!=0 || Ring_flag!=1)
      Ring_flag=1;
    
    if(fla[3]==1) { //jeigu buvo priverstinis restart
      state=fla[2]; // atstatoma pries restatra buvusi reiksme
      set_flags(0,9);
    }
    
    M95_on();
    
    Send_string("ATE0\r",2);
    Send_string("AT+CMGF=1\r",2); //formatas
    Send_string("AT+IPR=115200\r",2);
   // Send_string("AT+QNITZ=1\r",4); //laikas
    Send_string("AT&W\r",2); //save
    
    get_phones();
    get_obj_ID();
    measure_acceleration();
    fix_acceleration(); 
    
    while(1) {
      WDTCTL = WDTPW+WDTCNTCL;
      measure_acceleration();
      delt=compare_acc(); 
      //sprintf(str,"delta= %1.2f\n",delt);
      //Send_string(str,0);
      battery(bat);
      if(delt>0.1)
        state=1;
      else{
        if(state==1)
          state=0;
        if(state==10)
          state=4;
      }
      if(SMS_state==3){
        state=3;
        SMS_state=0;
      }
      if(SMS_state==5){
        state=5;
        SMS_state=0;
      }
      
      switch (state) {
        case 0:
          //Send_string("CALM\n",0);
         // get_level();
          get_cells();
          if(SMS_flag==1)
            SMS("CALM DOWN");
          duomenu_siuntimas_GPRS(0,1);
          state=4;
          break;
          
        case 1:
          //Send_string("AL\n",0);
          M95_on();
          if(Ring_flag==1)
            majakas();
          //get_level();
          get_cells();
          if(SMS_flag==1)
            SMS("ALARM");
          measure_acceleration();
          fix_acceleration();
          duomenu_siuntimas_GPRS(0,2);
          SMS_check();
          break;
          
        case 2:
          //Send_string("ZAD\n",0);
          M95_on();
          //get_level();
          get_cells();
          SMS("REGULAR");
          duomenu_siuntimas_GPRS(0,0);
          fix_acceleration();
          state=4;
          SMS_check();
          break;
          
        case 3:
          //Send_string("DEACT\n",0);
          M95_on();
          //get_level();
          get_cells();
          SMS("DEACTIVATED");
          duomenu_siuntimas_GPRS(0,3);
          P1IE &= ~ BIT1; //shake sensor off
          one_time_flag=0;
          
          fix_acceleration(); 
          WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
//          Send_string("at+qpowd=1\r",0);
//          kiek=0;
//          while((P1IN&0x80)!=0 && kiek++<10){
//            __delay_cycles(2000000);
//            Send_string("at+qpowd=1\r",0);
//          }
          M95_off();
          __bis_SR_register(LPM3_bits | GIE);
          //state=4;
          break;
          
        case 4:
          //Send_string("MIEGOT\n",0);
          fix_acceleration(); 
          WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
//          Send_string("at+qpowd=1\r",0);
//          kiek=0;
//          while((P1IN&0x80)!=0 && kiek++<10){
//            __delay_cycles(2000000);
//            Send_string("at+qpowd=1\r",0);
//          }
          M95_off();
//          if(kiek==11)
//            ;//Send_string("NEISSIJUNGIA M95\n",0);
          
         // Send_string("Sle\n",0);
          //if(busena==1){
            P1IE |= BIT1;  //shake sensor on
            //Send_string("shoN\n",0);
          //}
          
          __bis_SR_register(LPM3_bits | GIE);
          break;
          
        case 5:
          //Send_string("ACT\n",0);
          M95_on();
          //get_level();
          get_cells();
          SMS("ACTIVATED");
          duomenu_siuntimas_GPRS(0,5);
          fix_acceleration();
          one_time_flag=1;
          state=4;
          SMS_check();
          break;
      }
    } //while(1)
}
//=============================================================================
// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
  P1IFG &= ~BIT1;    
  P1IE &= ~ BIT1; 
  state=10;
  //Send_string("INTER_shake\n",0);  
  WDTCTL = WDTPW+WDTCNTCL;
  __bic_SR_register_on_exit(LPM3_bits);     // Exit LPM4 

}
//=============================================================================
#pragma vector=RTC_VECTOR
__interrupt void RTC(void)
{
  RTCCTL01 &= ~(RTCAIFG); 
  P1IE &= ~ BIT1;
 // rtc_flag=1;
  state=2;
  //Send_string("INTER_clock\n",0);  
  WDTCTL = WDTPW+WDTCNTCL;
  __bic_SR_register_on_exit(LPM3_bits);     // Exit LPM4 

}
//=============================================================================
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR (void)
{
        unsigned char us;
	switch(__even_in_range(UCA1IV,4))
	{
	case 0:break;                             // Vector 0 - no interrupt
	case 2:					  // Vector 2 - RXIFG
          us=UCA1RXBUF;
          USARTReceiveBuffer[bufferSize++] = us;
          if (bufferSize>USART_BUF_SIZE)
             bufferSize=0;
          break;
	case 4:break
          ;                             // Vector 4 - TXIFG
	default: break;
	}
}
//=============================================================================
// Timer A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
  delay_cnt++;
  
}
//=============================================================================
unsigned char I2Csend(unsigned char addr, unsigned char val) {
    i2cm_start( );
      if( i2cm_out( 0x30 ) )//send write control byte
           if( i2cm_out( addr ) ) //send config command byte
             if( i2cm_out( val )) { // single matavimas
                i2cm_stop( ); 
                return 1;  
             }
            else
              return 0;
          else
            return 0;
      else
        return 0;
}
//=============================================================================
void measure_acceleration (void){
  memset(I2Cbuf1,0,6);
  I2CRead_Register(0xA8, 6, I2Cbuf1);
  x=I2Cbuf1[0]| (I2Cbuf1[1]<<8);                  
  y=I2Cbuf1[2]| (I2Cbuf1[3]<<8);
  z=I2Cbuf1[4]| (I2Cbuf1[5]<<8);
}
//=============================================================================
void fix_acceleration(){
  unsigned char dat[6];
  dat[0]=x;
  dat[1]=x>>8;
  dat[2]=y;
  dat[3]=y>>8;
  dat[4]=z;
  dat[5]=z>>8;
  write_2_flash_string(dat,ADDRESS+SLOT,6);
}
//=============================================================================
float compare_acc(){
  unsigned char *acc = (unsigned char*)(ADDRESS+SLOT);
  xf=acc[0]|acc[1]<<8;
  yf=acc[2]|acc[3]<<8;
  zf=acc[4]|acc[5]<<8;
  mod=sqrt(x*x+y*y+z*z);
  cosx=x/mod;
  cosy=y/mod;
  modulis=sqrt(xf*xf+yf*yf+zf*zf);
  cosxf=xf/modulis;
  cosyf=yf/modulis;
  deltaf=sqrt((cosxf-cosx)*(cosxf-cosx)+(cosyf-cosy)*(cosyf-cosy));
  return deltaf;
}
