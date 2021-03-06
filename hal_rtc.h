#ifndef RTC_H_
#define RTC_H_

struct Time { int Year, Month, DayOfWeek, Day, Hour, Minute, Second; };

extern int SetRTCYEAR(int year); 	
extern int SetRTCMON(int month);
extern int SetRTCDAY(int day);
extern int SetRTCDOW(int dow);
extern int SetRTCHOUR(int hour);
extern int SetRTCMIN(int min);
extern int SetRTCSEC(int sec);

extern int GetRTCTIM0(void); 	
extern int GetRTCTIM1(void); 	
extern int GetRTCDATE(void); 	
extern int GetRTCYEAR(void); 	

extern int GetRTCMON(void);
extern int GetRTCDOW(void);
extern int GetRTCDAY(void);
extern int GetRTCHOUR(void);
extern int GetRTCMIN(void);
extern int GetRTCSEC(void);

#endif /*RTC_H_*/
