#include <Arduino.h>
#include "Serial_print.h"
#include "hardwareConfig.h"
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
#include "display_TFT.h"
#include "RTCs.h"
#include "display_OLED.h"


#ifndef rtcClass
#define rtcClass
RTC_DS3231 rtc;
#endif

//initialize RTC
void init_RTC()
{
  //try to begin the RTC
  if (!rtc.begin())
  {
    //if failed, print/dispaly error message
    clock_status = 0;
    Serial.println("Couldn't find RTC");

    display_printError("  NO RTC",5000);

  }
  else
  {
    //initialization successful
    clock_status = 1;
    display_icons_OLED();
    display_icons_TFT();
  }
  
}

//read RTC and get time
void RTC_get_time()
{
  DateTime now = rtc.now();
  //construct the timestr
  //ex: timestr => 2020:02:13 01:20:21
  snprintf(timestr, 22, "%04d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
}