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

#define PI 3.141592653
#define NN 20

unsigned char I2Csend(unsigned char addr, unsigned char val);
void Clear_flags();
void write_i2c(unsigned char reg_add,unsigned char data);
void measure_acceleration (void);
void get_angle(void);
void fix_acceleration(char state);
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
long xf, yf, zf;

float modulis, modulisf, deltaf, ANGLE, cosz=0.0;
float avgangle=0.0, var=0.0;
float mod;
const char *ptra;
unsigned short pt1;
char str[20];
short sec_count=0, old_sec_count=0;

long restart_count=0;
long noconnection_count=0;
long noGPRSconnection_count=0;
long wake_up_count=0;
long good_connections=0;

union {
    unsigned long K;
    unsigned char k[4];
}UN;

int main(void) {
    WDTCTL = WDT_ARST_1000;
    
    TA1CCR0 = 65535;
    TA1CTL = TASSEL_2 + MC_2 + TACLR;         // SMCLK, contmode, clear TAR
    TA1CCTL0 = CCIE; //enable timer interrupt
    
    TA0CCR0 = 32535;    
    TA0CTL = TASSEL_1 + MC_1;         // SMCLK, contmode, clear TAR
    TA0CCTL0 = CCIE; //enable timer interrupt

    
    P6DIR |= 1;  //debug koja
	
    UART_Setup();

    _EINT();
    P1DIR &= ~ BIT7;
    P2DIR |= BIT0;

    P1IES &= ~BIT1;                           // P1.4 Lo/Hi edge
    P1IFG &= ~BIT1;                           // P1.4 IFG cleared
    //P1IE |= BIT1;                             // P1.4 interrupt enabled
  
    
    P4DIR |=BIT7;
    P4OUT |=BIT7;
    
    I2C_Init(0x18,16);
    __delay_cycles(200000);
     
    I2CWrite_Register(0x20, 0x97);
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
   // write_2_flash_string("333333",0xf000,6);
//    
// 
    
    memcpy(UN.k,(unsigned long*)0xf000,4); 
    restart_count=UN.K;
    memcpy(UN.k,(unsigned long*)0xf000+1,4);
    noconnection_count=UN.K;
    memcpy(UN.k,(unsigned long*)0xf000+2,4);
    noGPRSconnection_count=UN.K;
    memcpy(UN.k,(unsigned long*)0xf000+3,4);
    wake_up_count=UN.K;
    memcpy(UN.k,(unsigned long*)0xf000+4,4);
    good_connections=UN.K;
    
    
    restart_count++;
    set_debug_flags(restart_count,0);
    
    
//    while(1){
//      __delay_cycles(50000);
//      WDTCTL = WDTPW+WDTCNTCL;
//    }
    
    
    
//    set_debug_flags(0,0);
//    set_debug_flags(0,1);
//    set_debug_flags(0,2);
//    set_debug_flags(0,3);
//    set_debug_flags(0,4);
//    set_debug_flags(0,5);
//    
//    while(1);
    
    
    
//    
//    while(1){
//
//        memcpy(UN.k,(unsigned long*)0xf000,4); 
//        sprintf(str, "%u", UN.K);
//        Send_string("restart_count=",0);
//        Send_string(str,0);
//        Send_string("\n",0);
//        
//        memcpy(UN.k,(unsigned long*)0xf000+1,4); 
//        sprintf(str, "%u", UN.K);
//        Send_string("noconnection_count=",0);
//        Send_string(str,0);
//        Send_string("\n",0);
//        
//        memcpy(UN.k,(unsigned long*)0xf000+2,4); 
//        sprintf(str, "%u", UN.K);
//        Send_string("noGPRSconnection_count=",0);
//        Send_string(str,0);
//        Send_string("\n",0);
//        
//        memcpy(UN.k,(unsigned long*)0xf000+3,4); 
//        sprintf(str, "%u", UN.K);
//        Send_string("wake_up_count=",0);
//        Send_string(str,0);
//        Send_string("\n",0);
//
//        memcpy(UN.k,(unsigned long*)0xf000+4,4); 
//        sprintf(str, "%u", UN.K);
//        Send_string("good_connections=",0);
//        Send_string(str,0);
//        Send_string("\n",0);    
//   
//     __delay_cycles(50000);
//     }
  
    
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
    
    Send_string("---------------------pradzia-------------------",1);
//    while(1)
//      WDTCTL = WDTCNTCL+WDT_ARST_1000;
    
    M95_on();
    
    Send_string("ATE0\r",2);
    Send_string("AT+CMGF=1\r",2); //formatas
    Send_string("AT+IPR=115200\r",2);
   // Send_string("AT+QNITZ=1\r",4); //laikas
    Send_string("AT&W\r",2); //save
    
    Send_string("AT+QIREGAPP\r",20);
    wait_answer("OK", "ERROR",20,2);
    Send_string("AT+QIACT\r",20);
    wait_answer("OK", "ERROR",20,2);
    
    
    
    get_phones();
    get_obj_ID();
    measure_acceleration();
    fix_acceleration(0); 
    state=7;
    
    while(1) {
      
      WDTCTL = WDTCNTCL+WDT_ARST_1000;

      
      switch (state) {
        case 10:
          Send_string("CHECK\n",0);
          measure_acceleration();
          delt=compare_acc(); 
          sprintf(str,"delta= %1.2f\n",delt);
          Send_string(str,0);
          if(delt>0.1)
            state=1;
          else 
            state=4;

          if(SMS_state==3){
            state=3;
            SMS_state=0;
          }
          if(SMS_state==5){
            state=5;
            SMS_state=0;
          }
          break;
          
        case 0:
          //Send_string("CALM\n",0);
          if((P1IN&0x80)!=0){
            battery(bat);
            get_cells();
            if(SMS_flag==1)
              SMS("CALM DOWN");
            duomenu_siuntimas_GPRS(0,1);
            state=4;
          }
          else
            M95_on();
                    
          break;
          
        case 1:
          //Send_string("AL\n",0);
          if(M95_on()){
            battery(bat);
            if(Ring_flag==1){
              if((P1IN&0x80)!=0)
                majakas();
            }
            if((P1IN&0x80)!=0)
              get_cells();
            if(SMS_flag==1){
              if((P1IN&0x80)!=0)
                SMS("ALARM");
            }
            measure_acceleration();
            fix_acceleration(0);
            if((P1IN&0x80)!=0)
              duomenu_siuntimas_GPRS(0,2);
            if((P1IN&0x80)!=0)
              SMS_check();
            if(state!=3)
              state=6;
          }
          else
            state=4;
          break;
          
        case 2:
          //Send_string("ZAD\n",0);
          if(M95_on()){
            battery(bat);
            get_cells();
            SMS("REGULAR");
            duomenu_siuntimas_GPRS(0,0);
            fix_acceleration(0);
            if(state==2)
              state=4;
            SMS_check();
          }
          break;
          
        case 3:
          //Send_string("DEACT\n",0);
          if(M95_on()){
            battery(bat);
            get_cells();
            SMS("DEACTIVATED");
            duomenu_siuntimas_GPRS(0,3);
          }
          P1IE &= ~ BIT1; //shake sensor off
          one_time_flag=0;
          
          fix_acceleration(1); 
          WDTCTL = WDTCNTCL+WDT_ARST_1000;// Stop watchdog timer
          M95_off();
          WDTCTL = WDTPW | WDTHOLD;
          __bis_SR_register(LPM3_bits | GIE);
          break;
          
        case 4:
          Send_string("MIEGOT\n",0);
          WDTCTL = WDTCNTCL+WDT_ARST_1000;	// Stop watchdog timer
          M95_off();
          
          Send_string("po OFF\n",0);

          //measure_acceleration();
          fix_acceleration(1); 
          
          Send_string("miegu\n",0);
          
          WDTCTL = WDTPW | WDTHOLD;
          P1IE |= BIT1;  //shake sensor on
          __bis_SR_register(LPM3_bits | GIE);
          break;
          
        case 5:
          //Send_string("ACT\n",0);
         // M95_on();
          battery(bat);
          get_cells();
          SMS("ACTIVATED");
          duomenu_siuntimas_GPRS(0,5);
          measure_acceleration();
          fix_acceleration(0);
          one_time_flag=1;
          state=4;
          SMS_check();
          break;
          
       case 6:
          Send_string("LAukiu\n",0);
          measure_acceleration();
          get_angle();
          avgangle = ANGLE;
          sec_count=0;
          old_sec_count=0;
          while(sec_count<30){
            measure_acceleration();
            get_angle();
            avgangle = avgangle +(ANGLE-avgangle)/NN;
            var = ((NN-1)*var+(ANGLE-avgangle)*(ANGLE-avgangle))/NN;
            
            if(sec_count!=old_sec_count){
              old_sec_count=sec_count;
              sprintf(str,"kampas= %1.2f\n",avgangle);
              Send_string(str,0);
              sprintf(str,"var= %1.2f\n",var);
              Send_string(str,0);
            }
            WDTCTL = WDTCNTCL+WDT_ARST_1000; 
          }
          if(var<2 && avgangle<3)
            state=0; //nurimo
          else
            state=1; //alarm
          
          sprintf(str,"kampas= %1.2f\n",avgangle);
            Send_string(str,0);
            sprintf(str,"var= %1.2f\n",var);
            Send_string(str,0);
          
          //SMS_check();
          break; 
       case 7:
          //Send_string("ACT\n",0);
          //M95_on();
          battery(bat);
          get_cells();
          SMS("STARTED");
          duomenu_siuntimas_GPRS(0,7);
          measure_acceleration();
          fix_acceleration(0);
          if(one_time_flag==0)
            state=5;
          else
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
  P1IE &= ~ BIT1; 
  state=10;
  Send_string("Shake\n",0);  
  memcpy(UN.k,(unsigned long*)0xf000+3,4);
  wake_up_count=UN.K;
  wake_up_count++; 
  set_debug_flags(wake_up_count,3);
  WDTCTL = WDTCNTCL+WDT_ARST_1000;
  P1IFG &= ~BIT1;
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
  WDTCTL = WDTCNTCL+WDT_ARST_1000;
  __bic_SR_register_on_exit(LPM3_bits);     // Exit LPM4 

}
//=============================================================================
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR (void)
{
        unsigned char us;
	switch(__even_in_range(UCA1IV,4)) {
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
// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
  sec_count++;
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
  //short x0, y0, z0;
  //float modulis=0;
  memset(I2Cbuf1,0,6);
  I2CRead_Register(0xA8, 6, I2Cbuf1);
  x=I2Cbuf1[0]| (I2Cbuf1[1]<<8);                  
  y=I2Cbuf1[2]| (I2Cbuf1[3]<<8);
  z=I2Cbuf1[4]| (I2Cbuf1[5]<<8);
}
//=============================================================================
void get_angle(void){
  modulis=sqrt(x*x+y*y+z*z);
  cosz=(float)(((float)x*(float)xf+(float)y*(float)yf+(float)z*(float)zf)/((float)modulis*(float)modulisf));
  if(cosz>-1 && cosz<1)
    ANGLE = acos(cosz>1?1:cosz)*180/PI; 
  else
    ANGLE=0;
    
 // ANGLE = acos(cosz>1?1:cosz)*180/PI;  
}
//=============================================================================
void fix_acceleration(char state){
  unsigned char dat[6];
  if(state==1){
    dat[0]=x; 
    dat[1]=x>>8;
    dat[2]=y;
    dat[3]=y>>8;
    dat[4]=z;
    dat[5]=z>>8;
    write_2_flash_string(dat,ADDRESS+SLOT,6);
  }
  else {
    xf=x;
    yf=y;
    zf=z;
    modulisf=sqrt(x*x+y*y+z*z);
  }
}
//=============================================================================
float compare_acc(){
  long x0, y0, z0; 
  float modulis0, cosx, cosy, cosxf, cosyf, delta0;
  unsigned char *acc = (unsigned char*)(ADDRESS+SLOT);
  x0=acc[0]|acc[1]<<8;
  y0=acc[2]|acc[3]<<8;
  z0=acc[4]|acc[5]<<8;
  mod=sqrt(x*x+y*y+z*z);
  cosx=x/mod;
  cosy=y/mod;
  modulis0=sqrt(x0*x0+y0*y0+z0*z0);
  cosxf=x0/modulis0;
  cosyf=y0/modulis0;
  
//  sprintf(str,"cosx= %1.2f\n",cosx);
//  Send_string(str,0);
//  sprintf(str,"cosy= %1.2f\n",cosy);
//  Send_string(str,0);
//  sprintf(str,"cosxf= %1.2f\n",cosxf);
//  Send_string(str,0);
//  sprintf(str,"cosyf= %1.2f\n",cosyf);
//  Send_string(str,0);
  
  if(!isnan(cosxf) && !isnan(cosyf) && !isnan(cosx) && !isnan(cosy))    // apsauga nuo nan
    delta0=sqrt((cosxf-cosx)*(cosxf-cosx)+(cosyf-cosy)*(cosyf-cosy));
  else
    delta0=1;
  
  return delta0;
}
