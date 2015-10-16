#include "UART_functions.h"
#include "main.h"

unsigned long delay_cnt;


//void write_char(char c)
//{
//	while (!(UART_FLAG_REG & UART_TX_FLAG));
//	UART_TX_BUF=c;
//}
//
//
//
//char write_string(char * s)
//{
//   char i = 0;
//   char c;
//   while (c = *(s++))
//   {
//	   i++;
//	   write_char(c);
//   }
//   return i;
//}
//
//void write_string2(char * a,int length)
//{
//	unsigned int i;
//	for(i=0;i<length;i++)
//	{
//		write_char(a[i]);
//	}
//}



void Send_string(char* sdata, unsigned long del) {
   int j=0;
   UCA1IE &= ~UCRXIE;
   while(sdata[j]!=0)
     USARTSendChar(sdata[j++]);

   UCA1IE |= UCRXIE;
   Clear_flags();
   delay_cnt=0;
   while(delay_cnt<del)
     WDTCTL = WDTCNTCL+WDT_ARST_1000;
}

void USARTSendChar(char character)
{
  while (!(UCA1IFG & UCTXIFG));  
  UCA1TXBUF = character;
}

//char UART_receive_packet(char RXBUF, char *USARTReceiveBuffer)
//{
//	static char segmentNum = 0;
//	static char flag_mask=0;
//	static char flag_fillBuff = 0;
//	static char flag_received = 0;
//    if((RXBUF == MASK_SYMBOL) && (flag_mask==0))
//		flag_mask=1;
//	else
//	{
//		if(RXBUF==RX_START_SYMBOL && flag_mask==0)
//		{
//			flag_fillBuff=1;
//			segmentNum=0;
//		}
//		else if(RXBUF==RX_STOP_SYMBOL && flag_mask==0 && flag_fillBuff)
//		{
//			flag_fillBuff=0;
//			if(segmentNum>0)
//				flag_received = 1;
//		}
//		else if (!((RXBUF==TX_START_SYMBOL || RXBUF==TX_STOP_SYMBOL) && flag_mask==0) && flag_fillBuff)
//		{
//			USARTReceiveBuffer[segmentNum++] = RXBUF;
//			flag_received = 0;
//			if(segmentNum>USART_BUF_SIZE)
//				flag_fillBuff=0;						//error overflow
//		}
//		flag_mask=0;
//	}
//    return flag_received;
//}



//void write_ascii(char a)
//{
//	const char b=0xf0 , c=0x0f;
//	char d;
//	d=a&b;
//	d=d>>4;
//	d=d+48;
//	if(d>0x39)
//		d=d+7;
//	write_char(d);
//	d=(a&c)+48;
//	if(d>0x39)
//		d=d+7;
//	write_char(d);
//}


//void write_int(int i)
//{
//	int j=0,k=10000;
//	char a;
//	while(i>0)
//	{
//		j=(int)i/k;
//		i=(int)i%k;
//		k=k/10;
//		a=j+48;
//		write_char(a);
//	}
//}



//void write_int_digit(int i)
//{
//	char a;
//	a=i+48;
//	write_char(a);
//}



//void write_float(float f , int pres)
//{
//	int i,p=0;
//	float j;
//	while(f > 0 && p<pres+1)
//	{
//		i=(int)f;
//		j=f-i;
//		j=j*10;
//		f=j;
//		write_int_digit(i);
//		p++;
//	}
//}

