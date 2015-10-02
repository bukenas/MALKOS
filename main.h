#include <string.h>
void Clear_flags();
extern unsigned char flash_data[];
extern unsigned long delay_cnt;
extern unsigned char USARTReceiveBuffer[];
extern unsigned long bufferSize;
extern char object_ID[];
extern char state, one_time_flag, SMS_state, Ring_flag, SMS_flag;
extern char SMS_message[];
extern char bat[5];
extern unsigned char kiek;
#define ADDRESS 0x1800
#define SLOT 128//512

extern long x, y, z;