#ifndef _GPS_H_
#define _GPS_H_

// set up GPS module
#include <TinyGPS++.h>
TinyGPSPlus gps;
TinyGPSCustom fix(gps, "GPGSA", 2); // $GPGSA sentence, 2nd element

//function prototype 
void init_gps();
void update_gps_data();

//import/link variables from the main_scr
extern double latitude;
extern double longitude;
extern double altitude;
extern int satellites;
extern int fix_mode;
extern boolean gps_isUpdated;
extern unsigned int gps_age;




#endif