#ifndef _DISPLAY_OLED_H_
#define _DISPLAY_OLED_H_

//function prototype
void init_display_OLED();
void display_printError(const char *msg);
void display_printError(const char *msg1,int timeOut);
void display_printError(const char *msg1,const char *msg2,int timeOut);
void display_printError_OLED_ONLY(const char *msg1,const char *msg2,int timeOut);

void display_icons_OLED();
void update_icons_OLED();
void update_display_OLED();

//display object 
extern Adafruit_SSD1306 display;

//import varibles from main_scr
extern boolean wifi_status ;  //0= no wifi, 1-3 = signal strength
extern int sync_status ;      // 1=syncing, 2=sync done
extern int clock_status ;     //0=no clock, 1=rtc, 2=gps
extern boolean sd_status ;
extern boolean cloud_status ;


extern boolean prv_wifi_status ;  //0= no wifi, 1-3 = signal strength
extern int prv_sync_status ;      // 1=syncing, 2=sync done
extern int prv_clock_status ;     //0=no clock, 1=rtc, 2=gps
extern boolean prv_sd_status ;
extern boolean prv_cloud_status ;

extern unsigned long count_coincident;
extern float avg_reading_mv;  
extern unsigned long minute_count;
extern char timestr[22];


#endif