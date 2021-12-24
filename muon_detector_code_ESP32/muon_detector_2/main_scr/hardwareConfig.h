#ifndef _SETTINGS_H_
#define _SETTINGS_H_
/*
    Define pin numbers here
*/

///////////////////////////Detectors\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
#define DETECTOR1_ADC 36
#define DETECTOR2_ADC 39
#define DETECTOR1_RESET 23
#define DETECTOR2_RESET 27

#define DETECTOR3_ADC 34
#define DETECTOR4_ADC 35

#define DETECTOR1_DAC 25   //NOTE: Shared with ETHERNET_CS
#define DETECTOR2_DAC 26   //NOTE: Shared with PUSH_BUTTON
///////////////////////////I/O\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
#define LED 13
#define PUSH_BUTTON 26     //NOTE: Shared with DETECTOR2_DAC

///////////////////////////SD CARD\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
#define SD_CS 5      //chip select pin of SD card,
/* other pins. (Cannot chnange. wired to hardware SPI)
    MOSI - 23
    MISO - 29
    CLK - 28
*/

////////////////////////TFT DISPLAY\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
#define DISPLAY5100_BACKLIGHT 32    
#define DISPLAY5100_CS 4            //NOTE: Shared with GSM
#define DISPLAY5100_DC 2            //NOTE: Shared with GSM
#define DISPLAY5100_RST 15          //NOTE: Shared with ethernet_rst
/* other pins. (Cannot chnange. wired to hardware SPI)
    MOSI - 23
    MISO - 29
    CLK - 28

    NOTE: since display share pins with ethernet and GSM, can not use these modules with TFT display
          Consider OLED i2c display if you need ethernet or GSM 
*/

////////////////////////OLED DISPLAY\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
#define OLED_I2C_ADDRESS 0x3C
/* pins. (Cannot chnange. wired to hardware I2C)
    SDA - 21
    SCL - 22
*/

/////////////////////////RTC MODULE\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
/* pins. (Cannot chnange. wired to hardware I2C)
    SDA - 21
    SCL - 22
*/

/////////////////////OTHER I2C MODULE\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
/* NOTE: 
    Can connect any type of i2c sensor support 3.3V voltage logics
        ex: temperature sensors, humidity sensors, barometers, accelerometer, Lux meters
  pins. (Cannot chnange. wired to hardware I2C)
    SDA - 21
    SCL - 22 
*/

///////////////////////////ETHERNET\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
#define ETHERNET_CS 13    //NOTE: Shared with DETECTOR1_DAC
#define ETHERNET_RST 15   //NOTE: Shared with display_rst
/* other pins. (Cannot chnange. wired to hardware SPI)
    MOSI - 23
    MISO - 29
    CLK - 28
*/

////////////////////////////GPS\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
#define GPS_PPS 14      //Pulse per second pin of the GPS module
/* other pins. (Cannot chnange. wired to hardware serial2)
    TX - 16
    RX - 17
*/
#define gpsSerial Serial2 //use serial2 for GPS


/////////////////////////OTHER I/0\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
#define BUZZER 12       //Shared with pulse_outs

#endif 