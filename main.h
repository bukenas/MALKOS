void Clear_flags();
extern unsigned char flash_data[];
extern unsigned long delay_cnt;
extern unsigned char USARTReceiveBuffer[];
extern unsigned long bufferSize;
extern char object_ID[], busena;
extern char state, one_time_flag;
extern char SMS_message[];
#define ADDRESS 0x1800
#define SLOT 128//512

extern long x, y, z;