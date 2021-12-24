#ifndef _AUTO_CONFIG_
#define _AUTO_CONFIG_


extern boolean uploadOn;
extern String mqttServer;
extern int mqttPort;
extern String mqttId;
extern String mqttUsername;
extern String mqttPassword;

extern int DetectorID;
extern String DetectorLocation;

extern boolean gpsTimeOn;
//extern boolean detector1On;
//extern boolean detector2On;
extern boolean buzzerOn;
extern boolean LEDOn;
extern boolean displayOn;
extern boolean resetSW;

extern int detector1_threshold_mV;
extern int detector2_threshold_mV;
extern int buzzer_volume1 ; //100 of 255
extern int buzzer_volume2 ;  //100 of 255

extern String ntpserver;

extern char timestr[22];

extern boolean wifi_status ;  //0= no wifi, 1-3 = signal strength
extern int sync_status ;      // 1=syncing, 2=sync done
extern int clock_status ; //0=no clock, 1=rtc, 2=gps
extern boolean sd_status ;
extern boolean cloud_status ;

extern unsigned int last_upload_file_line ;
extern unsigned int last_upload_file_position ;

extern char file_path[50];
extern char upload_path[50];

extern float avg_reading_mv;
//x extern int cpm ;
//x extern unsigned int count;
extern float raw_readings_ADC_mv[3];
extern boolean uploadOn;

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

//extern char timestr_D1[22][10];
//extern char timestr_D2[22][10];
//extern char timestr_coincident[22][10];

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

//void getParams_serverSettings(AutoConnectAux& aux);
//void getParams_generalSettings(AutoConnectAux& aux);
//String loadParams_serverSettings(AutoConnectAux& aux, PageArgument& args);
//String loadParams_generalSettings(AutoConnectAux& aux, PageArgument& args);

void portal_handleClient();
void initiate_auto_connect();

String webpage_button(String name,String style);

extern void createNewFile();
extern RTC_DS3231 rtc;

#endif



