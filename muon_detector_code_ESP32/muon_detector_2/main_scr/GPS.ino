#include <Arduino.h>
#include "Serial_print.h"
#include "hardwareConfig.h"
#include <TinyGPS++.h>
#include <GPS.h>

//https://lastminuteengineers.com/neo6m-gps-arduino-tutorial/

//initialize GPS
void init_gps(){
  gpsSerial.begin(9600); //GPS serial baud rate
  delay(100);
  update_gps_data(); 
}

//this function will read the GPS and try to fetch data
void update_gps_data(){
  while (gpsSerial.available() > 0) {
    // if data available in the serial buffer
    if (gps.encode(gpsSerial.read())) //enode and read
    {
      if (gps.location.isValid()) //if the location data valid
      { 
        //update values
        gps_isUpdated=gps.altitude.isUpdated() ;
        gps_age=gps.altitude.age();         //milliseconds since its last update
        latitude= gps.location.lat();       
        longitude=gps.location.lng();       
        altitude=gps.altitude.meters();
        satellites=gps.satellites.value();  //number of visible, participating satellites.
        fix_mode=(int)(fix.value()[0]-'0'); //fix mode; 3D or 2D fix
      }
      else
      {
        //set fix mode to 'no fix'
        fix_mode=0;
        //if last update time is more than 60 seconds, our location data is too old, reset values to zero, no fix
        if(gps_age>60000){
          latitude= 0;
          longitude=0;
          altitude=0;
        }

      }
    }
  }
}