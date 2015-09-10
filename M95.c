#include "M95.h"
#include <msp430f5308.h>
#include "UART_functions.h"
#include "main.h"
#include "flash.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const char *ptr;
unsigned short ptr1, ptr2;
unsigned char GPRS_message[500];
//char GPRSbuffer[220];
char object_ID[16];
char cell_nr=0;
char LAC[6]={0};
char COORDINATES[20];
char COORDINATES1[10];
char COORDINATES2[10];
char MC[8]={0};
char TA[3]={0};

char laikas[11];
char band[2];
short laiks=11, zone=12;

char busena=1;

struct phone_no {
  char number[13];
}phone[10];

struct M95_info {
  char cell_id[8];
  char level[10];
}M95_i[10];

void M95_ONOFF (void){
    P2OUT |= BIT0;
    //__delay_cycles(1500000);
    delay_cnt=0;
    while(delay_cnt<800)
      WDTCTL = WDTPW+WDTCNTCL; 
    P2OUT &= ~BIT0;
}

unsigned char check_M95(void) {
  Clear_flags();
  Send_string("AT\r",10);
  if (strstr((const char*)USARTReceiveBuffer,"OK") != 0)
    return 1;
  else
    return 0;
}

void get_phones(void){
  unsigned char * pho = (unsigned char*)(ADDRESS+2*SLOT);
  for (int i=0;i<10;i++){
        phone[i].number[0]=0;
    }
  ptr = strstr((const char*)pho,";");
  if(ptr) {
    ptr1 = ptr - (const char*)pho;
    for (int i=0;i<round((ptr1)/12);i++){
        memcpy(phone[i].number,pho+i*13,12);
    }
  }
}

void Clear_flags(){
      bufferSize=0;
      memset(USARTReceiveBuffer,0,USART_BUF_SIZE);
}

unsigned char check_connection(void) {
  Clear_flags();
  Send_string("AT+CREG?\r",4);
  if (strstr((const char*)USARTReceiveBuffer,"0,1") != 0)
    return 1;
  else
    return 0;
}

void SMS (char *type){
    char bat[5];
    battery(bat);
    strcpy(SMS_message,type);
    strcat(SMS_message,"\n");
    strcat(SMS_message,"Baterija=");
    strcat(SMS_message,bat);
    strcat(SMS_message," mV");
    sendSMS(SMS_message);
}

void sendSMS(char * smstext){
    char str[50] = {0};
    //unsigned char nr[3] = {0};
//    strcpy(str,"AT+CMGW=\"");
//    strcat(str,phone[0].number);
//    strcat(str,"\"\r");
//    Send_string(str, 20);
//    Send_string(smstext, 0);  // Send body of message
//    Send_string("\x1a", 2);   //terminating the message require ctrl-z
//    Send_string("\r", 50);
//    ptr = strstr(USARTReceiveBuffer,"+CMGW:");
//    if(ptr) {
//      ptr1 = ptr - USARTReceiveBuffer;
//      
//      ptr = strstr(USARTReceiveBuffer+ptr1,"\r");
//      if(ptr) {
//        ptr2 = ptr - USARTReceiveBuffer;
//        USARTReceiveBuffer[ptr2]=0;
//        strcpy(nr,USARTReceiveBuffer+ptr1+7);
        for (int i=0;i<10;i++){
          if(phone[i].number[0]==0x2B){ //ar yra +
//            strcpy(str,"AT+CMSS=");
//            strcat(str,nr);
//            strcat(str,",\"");
//            strcat(str,phone[i].number);
//            strcat(str,"\"\r");
//            Send_string(str, 50);
            strcpy(str,"AT+CMGS=\"");
            strcat(str,phone[i].number);
            strcat(str,"\"\r");
            Send_string(str, 5);
            delay_cnt=0;
            while(!strstr((const char*)USARTReceiveBuffer,">") && delay_cnt<100){
              __delay_cycles(5000);
            }
            Send_string(SMS_message, 2);  // Send body of message
            Send_string("\x1a", 0);   //terminating the message require ctrl-z
            Send_string("\r", 10);
            delay_cnt=0;
            while(!strstr((const char*)USARTReceiveBuffer,"CMGS:") && delay_cnt<300){
              __delay_cycles(5000);
            }
          }
        }

}

void majakas (void){
    Send_string("ATD",0);
    Send_string(phone[0].number,0);
    Send_string(";\r",4);
    delay_cnt=0;
    while(delay_cnt<200); 
    Send_string("ATH\r",4);  
}


void GPRS_connect(void){
  Send_string("at+qiopen=\"tcp\",\"193.219.36.241\",80\r",10);
  delay_cnt=0;
  while(!strstr((const char*)USARTReceiveBuffer,"CONNECT OK") && !strstr((const char*)USARTReceiveBuffer,"FAIL") && delay_cnt<500)
    __delay_cycles(5000);
}


void battery(char* ats){
  Send_string("at+cbc\r",20); 
  
  ptr = strstr((const char*)USARTReceiveBuffer,"CBC:");
  if(ptr) {
    ptr1 = ptr - (const char*)USARTReceiveBuffer;
  }
      memcpy(ats,USARTReceiveBuffer+ptr1+10,4);
      ats[4]=0;

}

void pharse_GPRS_string(char *type) {
  char bat[5];
  char str[15];
  battery(bat);
  strcpy((char*)GPRS_message,"GET /exchange_info.php?object_id=");
  strcat((char*)GPRS_message,object_ID);
  strcat((char*)GPRS_message,"&cell=");
//  for(int i=0;i<cell_nr;i++){
//      strcat((char*)GPRS_message,M95_i[i].cell_id);
//      strcat((char*)GPRS_message,",");
//      strcat((char*)GPRS_message,M95_i[i].level);
//      strcat((char*)GPRS_message,",");
//      strcat((char*)GPRS_message,LAC);
//      strcat((char*)GPRS_message,",");
//      strcat((char*)GPRS_message,TA); //TA
//      strcat((char*)GPRS_message,",");
//      strcat((char*)GPRS_message,MC);
//      if(i!=cell_nr-1)
//       strcat((char*)GPRS_message,";");
//  }
      strcat((char*)GPRS_message,M95_i[0].cell_id);
      strcat((char*)GPRS_message,",");
      strcat((char*)GPRS_message,M95_i[0].level);
      strcat((char*)GPRS_message,",");
      strcat((char*)GPRS_message,LAC);
      strcat((char*)GPRS_message,",");
      strcat((char*)GPRS_message,TA); //TA
      strcat((char*)GPRS_message,",");
      strcat((char*)GPRS_message,MC);
       strcat((char*)GPRS_message,";");
      
      
      strcat((char*)GPRS_message,"&battery=");
      strcat((char*)GPRS_message,bat);
      strcat((char*)GPRS_message,"&type=");
      strcat((char*)GPRS_message,type);
//      strcat((char*)GPRS_message,COORDINATES2);
//      strcat((char*)GPRS_message,",");
//      strcat((char*)GPRS_message,COORDINATES1);
      strcat((char*)GPRS_message,"&x=");
      sprintf(str, "%d", x);
      strcat((char*)GPRS_message,str);
      //strcat((char*)GPRS_message,COORDINATES2);
      strcat((char*)GPRS_message,"&y=");
      sprintf(str, "%d", y);
      strcat((char*)GPRS_message,str);
      //strcat((char*)GPRS_message,COORDINATES1);
      strcat((char*)GPRS_message,"&z=");
      sprintf(str, "%d", z);
      strcat((char*)GPRS_message,str);
      strcat((char*)GPRS_message,"&coord=");
      strcat((char*)GPRS_message,COORDINATES);
      strcat((char*)GPRS_message," HTTP/1.1\r\n");
}


void get_obj_ID(void){
  Send_string("at+gsn\r",20); 
  ptr = strstr((const char*)USARTReceiveBuffer,"OK");
  if(ptr) {
    ptr1 = ptr - (const char*)USARTReceiveBuffer;
    memcpy(object_ID,USARTReceiveBuffer+ptr1-14,9);
  }
}

void get_cells(void){
  char MCa[6], MCb[6], buff[300]={0};
  char * pch;
  char * ach;
  short i=0;
  //char cell_nr=0;
  for(int i=0;i<10;i++){
    M95_i[i].cell_id[0]=0;
    M95_i[0].level[0]=0;
  }
  
 // while(M95_i[0].cell_id[0]==0x78 || M95_i[0].cell_id[0]==0x00){ //jei gauna x kartoti
      Send_string("at+qeng=1,3\r",10); 
      
      Send_string("AT+qeng?\r",50); 
      delay_cnt=0;
      while(!strstr((const char*)USARTReceiveBuffer,"OK") && delay_cnt<100)
        __delay_cycles(5000);
      cell_nr=0;
      
      
      ptr = strstr((const char*)USARTReceiveBuffer+9,"+QENG: 1,1");
      if(ptr) {
        ptr1 = ptr - (const char*)USARTReceiveBuffer;

        ptr = strstr((const char*)USARTReceiveBuffer+ptr1,"x");
        if(ptr) {
          ptr2 = ptr - (const char*)USARTReceiveBuffer;
          if((ptr2-ptr1)>0 && (ptr2-ptr1)<=300)
            memcpy(buff,USARTReceiveBuffer + ptr1,ptr2-ptr1-1);
        }
        else {
          ptr = strstr((const char*)USARTReceiveBuffer+ptr1,"+QENG: 2");
          if(ptr) {
            ptr2 = ptr - (const char*)USARTReceiveBuffer;
            if((ptr2-ptr1)>0 && (ptr2-ptr1)<=300)
              memcpy(buff,USARTReceiveBuffer + ptr1,ptr2-ptr1-1);
          }
        }
      }
      ptr = strstr((const char*)USARTReceiveBuffer+9,"+QENG: 0");

      if(ptr) {
        ptr1 = ptr - (const char*)USARTReceiveBuffer;
      }
      pch = strtok ((char*)USARTReceiveBuffer+ptr1,",");
      while (pch != NULL)
      {
        pch = strtok (NULL, ",");
        if(i==3)
          memcpy(M95_i[0].cell_id,pch,6);
        if(i==6){
          memcpy(M95_i[0].level,pch,6);
          strcat(M95_i[0].level,"dbm");
          cell_nr++;
        }
        if(i==2)
          memcpy(LAC,pch,6);
        if(i==0)
          memcpy(MCa,pch,6);
        if(i==1){
          memcpy(MCb,pch,6);
          strcpy(MC,MCa);
          strcat(MC,MCb);
        }
        i++;
      }
      
//      ach = strtok (buff,",");
//      i=0;
//      while (ach != NULL)
//      {
//        ach = strtok (NULL, ",");
//        
//        if(i==9+10*(cell_nr-1)){
//          if(strcmp(ach,"x")!=0){
//            memcpy(M95_i[cell_nr].cell_id,ach,6);
//            if(M95_i[cell_nr].cell_id[2]=='\r')
//              M95_i[cell_nr].cell_id[2]='\0';
//            if(M95_i[cell_nr].cell_id[3]=='\r')
//              M95_i[cell_nr].cell_id[3]='\0';
//            if(M95_i[cell_nr].cell_id[4]=='\r')
//              M95_i[cell_nr].cell_id[4]='\0';
//            cell_nr++;
//          }
//        }
//        if(i==2+10*(cell_nr-1)){
//          if(strcmp(ach,"x")!=0){
//              memcpy(M95_i[cell_nr].level,ach,6);
//              strcat(M95_i[cell_nr].level,"dbm");
//          }
//        }
//        i++;
//        
//      }
//      
//      Send_string("at+qlastta\r",10);
//      ptr = strstr((const char*)USARTReceiveBuffer,"+QLASTTA");
//      if(ptr) {
//        ptr1 = ptr - (const char*)USARTReceiveBuffer;
//        
//        ptr = strstr((const char*)USARTReceiveBuffer+ptr1,"\r");
//        if(ptr) 
//          ptr2 = ptr - (const char*)USARTReceiveBuffer;
//        
//       if(ptr2-ptr1==11){
//          memcpy(TA,USARTReceiveBuffer+ptr1+10,1);
//          TA[1]=0;
//        }
//        if(ptr1-ptr2==12){
//          memcpy(TA,USARTReceiveBuffer+ptr1+10,2);
//          TA[2]=0;
//        }  
//      }
 // }
}


void duomenu_siuntimas_GPRS(char param, char code){
 char str[10];
 
  //Send_string("at+qifgcnt\r",20);
 // Send_string("at+qiact\r",20);
  Send_string("at+qcellloc=1\r",5);
  delay_cnt=0;
  while(!strstr((const char*)USARTReceiveBuffer,"QCELLLOC:") && delay_cnt<400)
      __delay_cycles(5000);
//Send_string("at+qgsmloc=7\r",500);
 
  ptr = strstr((char const*)USARTReceiveBuffer,"QCELLLOC:");
    if(ptr) {
      ptr1 = ptr - (const char*)USARTReceiveBuffer;
      
      memcpy(COORDINATES,USARTReceiveBuffer+ptr1+10,19);
      memcpy(COORDINATES1,USARTReceiveBuffer+ptr1+10,9);
      memcpy(COORDINATES2,USARTReceiveBuffer+ptr1+20,9);
 
    }
    else {
     strcpy(COORDINATES,"0,0"); 
     strcpy(COORDINATES1,"0");
     strcpy(COORDINATES2,"0");
    }
 
 
 
 
 
 
 
 
 
 
 
 
    GPRS_connect();
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    memcpy(GPRS_message,0,500);
        
    if(code==0)    
        pharse_GPRS_string("regular");
    if(code==1)    
        pharse_GPRS_string("nurimo");
    if(code==2)    
        pharse_GPRS_string("alarm");
    if(code==3)    
        pharse_GPRS_string("deactivated");
    if(code==5)    
        pharse_GPRS_string("activated");

    Clear_flags();
    Send_string("at+qisend\r",5);  
    delay_cnt=0;
    while(!strstr((const char*)USARTReceiveBuffer,">") && delay_cnt<200)
      __delay_cycles(5000);
    Send_string((char*)GPRS_message,1);
    // Send_string("GET /exchange_info.php?object_id=863071017428399&cell=434B,-68dbm,0004,1;208C,-84dbm,0004,1&battery=4290&type=alarm&x=2&y=3&z=4 HTTP/1.1\r\n",0);
    Send_string("Host: mediena.ktueik.tk\r\n",0);
    Send_string("Connection: keep-alive\r\n",0);
    Send_string("\r\n\x1a",10);
    
    
   
    
    
    delay_cnt=0;
    while(!strstr((const char*)USARTReceiveBuffer,"&#193") && delay_cnt<300)
      __delay_cycles(5000);
    
    
    ptr = strstr((char const*)USARTReceiveBuffer,"GMT"); //laiko pasiemimas is serverio
    if(ptr) {
      ptr1 = ptr - (const char*)USARTReceiveBuffer;
      
      
      memcpy(laikas,USARTReceiveBuffer+ptr1-9,11);
      
//      
//      Send_string("PradLaikas=",0);
//      Send_string(laikas,0);
//      
      memcpy(band,laikas,2);
      sscanf (band,"%2x",&laiks);

//      memcpy(band,laikas+9,2);
//      sscanf (band,"%2d",&zone);
      
      RTCCTL01 |= RTCHOLD;                   // Stop RTC calendar mode
      RTCCTL01 |= RTCAIE+  RTCBCD + RTCHOLD + RTCMODE;
          
      RTCHOUR = laiks;//+(zone/4);
          
      memcpy(band,laikas+3,2);
      sscanf (band,"%2x",&laiks);
      RTCMIN = laiks;
      memcpy(band,laikas+6,2);
      sscanf (band,"%2x",&laiks);
      RTCSEC = laiks;
      
      RTCADOWDAY = 0x00;                         // RTC Day of week alarm = 0x2
      RTCADAY = 0x00;                           // RTC Day Alarm = 0x20
      RTCAHOUR = 0x12;                          // RTC Hour Alarm
      RTCAMIN = 0x00;   
      RTCAHOUR |= (1<<7);   
      RTCCTL01 &= ~(RTCHOLD);                   // Start RTC calendar mode
    }
  
    ptr = strstr((char const*)USARTReceiveBuffer,"&#192");
    if(ptr) {
      ptr1 = ptr - (const char*)USARTReceiveBuffer;
       
    ptr = strstr((char const*)USARTReceiveBuffer,"&#193");
    if(ptr) {
      ptr2 = ptr - (const char*)USARTReceiveBuffer;
    }

    memcpy(GPRS_message,0,500);
    if((ptr2-ptr1-4)>0 && (ptr2-ptr1-4)<500)
      memcpy(GPRS_message,USARTReceiveBuffer+ptr1+5,ptr2-ptr1-4);

    ptr = strstr((char const*)GPRS_message,"+");
    if(ptr) {
      ptr1 = ptr - (const char*)GPRS_message;
      
      ptr = strstr((char const*)GPRS_message,";");

      if(ptr) 
        ptr2 = ptr - (const char*)GPRS_message;
      

      if(ptr2-ptr1>11){
        write_2_flash_string (GPRS_message+ptr1,ADDRESS+2*SLOT,(ptr2-ptr1));
        for (int i=0;i<10;i++){
          phone[i].number[0]=0; //istrinami visi numeriai
        }
        for (int i=0;i<round((ptr2-ptr1)/12);i++){
            memcpy(phone[i].number,GPRS_message+ptr1+i*13,12);
        }
      }
    }
    ptr = strstr((char const*)GPRS_message,"activate");
    if(ptr) {
      if(one_time_flag==0){
        state=5;
      }
      busena=1;
    }
    ptr = strstr((char const*)GPRS_message,"deactivate");
    if(ptr) {
      state=3;
      busena=0;
    }
    

    
    
    ptr = strstr((char const*)GPRS_message,"&");
    if(ptr) {
      ptr1 = ptr - (const char*)GPRS_message;
    }  
    
    ptr = strstr((char const*)GPRS_message+ptr1-4,";");
    if(ptr) {
      ptr2 = ptr - (const char*)GPRS_message;
    }
    
    RTCCTL01 |= RTCHOLD;                   // Stop RTC calendar mode
    RTCCTL01 |= RTCAIE+  RTCBCD + RTCHOLD + RTCMODE;
    if(ptr1-ptr2==3){
      memcpy(band,GPRS_message+ptr2+1,2);
      sscanf (band,"%2x",&laiks);
      RTCAHOUR = laiks;
      RTCAHOUR |= (1<<7); 
    }
    if(ptr1-ptr2==2){
      memcpy(band,GPRS_message+ptr2+1,1);
      sscanf (band,"%2x",&laiks);
      RTCAHOUR = laiks;
      RTCAHOUR |= (1<<7); 
    }  
  }
   RTCCTL01 &= ~(RTCHOLD);                   // Start RTC calendar mode
   
   
    Send_string("AL_LAIKAS=",0);
    sprintf(str,"%x",RTCAHOUR);
    Send_string(str,0);  
    Send_string("\nLaikas=",0);
    
    
    sprintf(str,"%x",RTCHOUR);
    Send_string(str,0);  
    Send_string(":",0); 
    sprintf(str,"%x",RTCMIN);
    Send_string(str,0); 
    
    Send_string("at+qiclose\r",2);
    Send_string("ATE0\r",2);
    memset(USARTReceiveBuffer,0,USART_BUF_SIZE);
    bufferSize=0;
}

void Set_RTC(){
  
  Send_string("AT+CTZU=1\r",4);
  
  Send_string("AT+CCLK?\r",3);


  RTCCTL01 |= RTCHOLD;                   // Stop RTC calendar mode
  RTCCTL01 |= RTCAIE+  RTCBCD + RTCHOLD + RTCMODE;

  ptr = strstr((char const*)USARTReceiveBuffer,",");
  if(ptr)
    ptr1 = ptr - (const char*)USARTReceiveBuffer;
   
  memcpy(laikas,USARTReceiveBuffer+ptr1+1,11);
  memcpy(band,laikas,2);
  sscanf (band,"%2x",&laiks);

  memcpy(band,laikas+9,2);
  sscanf (band,"%2d",&zone);
      
  RTCHOUR = laiks+(zone/4);
      
  memcpy(band,laikas+3,2);
  sscanf (band,"%2x",&laiks);
  RTCMIN = laiks;
  memcpy(band,laikas+6,2);
  sscanf (band,"%2x",&laiks);
  RTCSEC = laiks;


  RTCADOWDAY = 0x00;                         // RTC Day of week alarm = 0x2
  RTCADAY = 0x00;                           // RTC Day Alarm = 0x20
  RTCAHOUR = 0x12;                          // RTC Hour Alarm
  RTCAMIN = 0x00;   
  // RTC Minute Alarm
  RTCAHOUR |= (1<<7);    

  RTCCTL01 &= ~(RTCHOLD);                   // Start RTC calendar mode
}

void M95_on(void){
  unsigned char kiek=0;
  Send_string("ON/OFF\n",2);
  //P4OUT &= ~BIT7; //su tranzu 
  __delay_cycles(5000000);
//  while(!(P1IN&0x80)){
//      WDTCTL = WDTPW+WDTCNTCL; 
//    P2OUT |= BIT0;
//  __delay_cycles(1500);
//  while(!(P1IN&0x80))
//     WDTCTL = WDTPW+WDTCNTCL;
//  __delay_cycles(500);
//  while(!(P1IN&0x80)
//     WDTCTL = WDTPW+WDTCNTCL;
//  __delay_cycles(500000);
//  P2OUT &= ~BIT0;
    if((P1IN&0x80)==0){
      P2OUT |= BIT0;
      Send_string("ON/OFF HIGH\n",2);
      while(!(P1IN&0x80))
        WDTCTL = WDTPW+WDTCNTCL;
      __delay_cycles(500);
      while(!(P1IN&0x80))
        WDTCTL = WDTPW+WDTCNTCL;
      __delay_cycles(500000);
      P2OUT &= ~BIT0;
      Send_string("ON/OFF  LOW\n",2);
  }

  while((!check_connection()) && kiek++<10){
            delay_cnt=0;
            while(delay_cnt<80)
              WDTCTL = WDTPW+WDTCNTCL; 
            Send_string("at+csq\r",20);
          }  
          if(kiek==11){
           Send_string("NEPRISIJUNGIU PRIE TINKLO\n",0); 
           Send_string("at+qpowd=1\r",0);
           __delay_cycles(2000000);
           WDTCTL = 0;//SW reset
          }
  
//  while((!check_connection())){
//    kiek++;
//    if (kiek>10)
//      break;
//    //__delay_cycles(5000000);
//    delay_cnt=0;
//    while(delay_cnt<80)
//      WDTCTL = WDTPW+WDTCNTCL; 
//    }  
//  if(kiek==11){
//    Send_string("at+qpowd=1\r",0);
//    //__delay_cycles(5000000);
//    delay_cnt=0;
//    while(delay_cnt<200)
//      WDTCTL = WDTPW+WDTCNTCL; 
//    while(!(P1IN&0x80)){
//      P2OUT |= BIT0;
//      __delay_cycles(1500);
//      while(!(P1IN&0x80))
//        WDTCTL = WDTPW+WDTCNTCL; 
//      __delay_cycles(500);
//      while(!(P1IN&0x80))
//        WDTCTL = WDTPW+WDTCNTCL;
//      //__delay_cycles(500000);
//      delay_cnt=0;
//      while(delay_cnt<100)
//        WDTCTL = WDTPW+WDTCNTCL; 
//      P2OUT &= ~BIT0;
//  }
//  }
}