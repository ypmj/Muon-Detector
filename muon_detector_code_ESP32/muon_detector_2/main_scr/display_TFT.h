#ifndef _DISPLAY_TFT_H_
#define _DISPLAY_TFT_H_

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include "hardwareConfig.h"
#include <RTClib.h>

//display object
Adafruit_ST7735 tft = Adafruit_ST7735(DISPLAY5100_CS, DISPLAY5100_DC, DISPLAY5100_RST);

//function prototype
void init_display_TFT();
void chk_tft();
void update_display__TFT_icon();
void display_icons_TFT();
void display_backlight_TFT(boolean sts);
void update_display_TFT();
void display_clear_TFT();

//import varibales form main_scr file
extern boolean wifi_status ;  //0= no wifi, 1-3 = signal strength
extern int sync_status ;      // 1=syncing, 2=sync done
extern int clock_status ; //0=no clock, 1=rtc, 2=gps
extern boolean sd_status ;
extern boolean cloud_status ;


extern boolean prv_wifi_status ;  //0= no wifi, 1-3 = signal strength
extern int prv_sync_status ;      // 1=syncing, 2=sync done
extern int prv_clock_status ; //0=no clock, 1=rtc, 2=gps
extern boolean prv_sd_status ;
extern boolean prv_cloud_status ;

extern DateTime now;
extern String ip;
extern String DetectorLocation;

extern unsigned long count_D1;
extern unsigned long count_D2;
extern unsigned long count_coincident;

extern unsigned long countPrv_D1;
extern unsigned long countPrv_D2;
extern unsigned long countPrv_coincident;
 
extern unsigned long count_SD_D1;
extern unsigned long count_SD_D2;
extern unsigned long count_SD_coincident;

extern unsigned long minute_count;

extern float avg_reading_mv;
extern float reading_mv_D1[10];
extern float reading_mv_D2[10];
extern float reading_mv_coincident[10];

extern int cpm_D1 ;
extern int cpm_D2 ;
extern int cpm_coincident ;

extern double latitude;
extern double longitude;
extern double altitude;
extern int satellites;
extern int fix_mode;
extern boolean gps_isUpdated;
extern unsigned int gps_age;
extern unsigned int cn3;

extern boolean gpsTimeOn ;
extern boolean uploadOn;

#endif