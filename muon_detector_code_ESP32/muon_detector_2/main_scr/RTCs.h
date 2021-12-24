#ifndef _RTC_H_
#define _RTC_H_

//function prototype 
void init_RTC();
void RTC_get_time();

//import variables from the main_scr
extern int clock_status;
extern char timestr[22];


#endif