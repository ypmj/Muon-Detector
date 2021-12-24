#ifndef _SERVER_UPLOAD_H_
#define _SERVER_UPLOAD_H_


//objects
#include <PubSubClient.h>
extern WiFiClient espClient;
extern PubSubClient client;

//function prototype 
void upload_file();
boolean upload_data(String data);
int file_read_line_and_upload();
void load_last_upload_state();
char select_uplaod_file(char *filepath_select, boolean current_file);
void reconnect(int retry);


//import variables from the main_scr
extern boolean uploadOn ;
extern String mqttServer ;
extern int mqttPort;
extern String mqttId ;
extern String mqttUsername ;
extern String mqttPassword ;

extern int DetectorID;
extern String DetectorLocation ;

extern boolean gpsTimeOn ;
//extern boolean detector1On ;
//extern boolean detector2On ;
extern boolean buzzerOn ;
extern boolean LEDOn ;
extern boolean displayOn ;

extern char timestr[22];

extern char folder_name[17];
extern char file_name[26];
extern char file_path[50];
extern char upload_path[50];

extern boolean wifi_status ;  //0= no wifi, 1-3 = signal strength
extern int sync_status ;      // 1=syncing, 2=sync done
extern int clock_status;      //0=no clock, 1=rtc, 2=gps
extern boolean sd_status;
extern boolean cloud_status;

extern unsigned int last_upload_file_line ;
extern unsigned int last_upload_file_position;

extern int upload_failed_count;

#ifndef UPLOAD_STATUS_FILE
    #define UPLOAD_STATUS_FILE "/last_upload_state.dat"
#endif



#endif