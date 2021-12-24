//=============================================================================
// Main SCR
//=============================================================================

//include libraries
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <display_TFT.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <RTClib.h>

// include some custom files that contains code
#include "hardwareConfig.h"    //PIN mapping
#include "SD_card.h"           //SD card functions
#include "RTCs.h"              //Real time clock functions
#include "autoconfig.h"        //wifi portal handling and config loading
#include "Server_upload.h"     //Upload to server functions and MQTT
#include "Buzzer.h"            //buzzer sounds
#include "iconset_16x16.h"     //header files containing  display icons
#include "GPS.h"               // GPS
#include "display_OLED.h"      //OLED i2C display
#include "display_TFT.h"       // TFT colour display
#include "Serial_print.h"      //Serial print functions

#define TFT //using TFT LCD
Adafruit_SSD1306 display(128, 64, &Wire, -1);

//RTC object
RTC_DS3231 rtc;
DateTime now;
#define rtcClass
//Wifi Object
WiFiClient espClient;
//MQTT object
PubSubClient client(espClient);

// default values. 
//NOTE: *** This can be changed by logging into portal *** (portle IP address will be displayed on the screen)
const char *ssid = "D-Link-s";           //default WIFI SSDI
const char *password = "sasanka123";     //default WIFI password
String mqttServer = "18.219.239.122";    //default MQTT server address
int mqttPort = 1883;                     //default MQTT port
String mqttId = "ESP8266Client";         //default MQTT userID
String mqttUsername = "sasa";            //default MQTT username
String mqttPassword = "sasanka123";      //default MQTT password
String ntpserver = "pool.ntp.org";       //default NTP server
int DetectorID = 0;                      //ID of the detector
String DetectorLocation = "UOC";         //Location string of the detector
int detector1_threshold_mV = 750;        //Voltage threshold of detector 1
int detector2_threshold_mV = 750;        //Voltage threshold of detector 2
int buzzer_volume1 = 100; //out of 255   //Buzzer volume (when detect a event)
int buzzer_volume2 = 30;  //out of 255   //Buzzer volume (General beeps, warnings, etc)
boolean uploadOn = false;                //Enable/disable upload to server
boolean buzzerOn = true;                 //Enable/disable Buzzer
boolean gpsTimeOn = false;               //Enable/disable GPS time sync
boolean LEDOn = true;                    //Enable/disable LED
boolean displayOn = true;                //Enable/disable TFT display
boolean resetSW = false;                 //Enable/disable external reset MCU switch

// declare some variables to store current status
boolean wifi_status = 0;    //0= no wifi, 1=wifi connected
int sync_status = 0;        //0=sync error, 1=syncing, 2=sync done
int clock_status = 0;       //0=no clock/RTC error, 1=using rtc, 2=using gps time
boolean sd_status = 0;      //0=SD card error, 1=SD card OK
boolean cloud_status = 0;   //0=no Connection to cloud server/connection dropped, 1=has a Connection

boolean prv_wifi_status = 0;   //previous status. (only use for LCD. LCD will only update if current and prv status dont match)
int prv_sync_status = 0;      
int prv_clock_status = 0;     
boolean prv_sd_status = 0;     
boolean prv_cloud_status = 0;  

// variables for GPS 
double latitude;        //latitude        
double longitude;       //longitude
double altitude;        //altitude
int satellites;         ////number of visible, participating satellites.
int fix_mode;           //3D or 2D fix
boolean gps_isUpdated;  //Update success or failed
unsigned int gps_age;   //GPS last update time

//varible (char array) to hold current time string
char timestr[22];

// variables for SD logging file name and folder location
char folder_name[17];     //temporary variable to store name  Folder to save data files
char file_name[26];       //temporary variable to store name of the file
byte file_created_day = 0;//file created day (new file will be created for every day)
char file_path[50];       //path of the data file saves coincident events **(THIS IS THE MAIN FILE. file will be uplaoded to the server)**
char file_path_CPMD1[50]; //path of the data file that saves detector1 CPMs *(this file will not be uploaded to the server)
char file_path_CPMD2[50]; //path of the data file that saves detector2 CPMs *(this file will not be uploaded to the server)
char file_path_D1[50];    //path of the data file that saves detector1 events *(this file will not be uploaded to the server)
char file_path_D2[50];    //path of the data file that saves detector2 events *(this file will not be uploaded to the server)


//variables to store last upload position
// ** in case of a power failure device can resume the upload from the last position **
char upload_path[50];                       //file path of the currently uploading file to the cloud server
unsigned int last_upload_file_line = 0;     //last upload line of the data file
unsigned int last_upload_file_position = 0; //last upload file position of the data file
#define UPLOAD_STATUS_FILE "/last_upload_state.dat" //last upload file_path and position will be save to this will
int upload_failed_count = 0;                    //how many times upload failed in a row, If this is more than 3, give a 1 minute timeout           

//Ip address of the detector
String ip;

// varibles for counting events
unsigned long count_D1;           //current number count (detector1)
unsigned long count_D2;           //current number count (detector2)
unsigned long count_coincident;   //current number coincident count

unsigned long countPrv_D1;        //previous count up to the last minute (to calculate CPM)
unsigned long countPrv_D2;        //
unsigned long countPrv_coincident;//

unsigned long count_SD_D1;        //how many counts have save into the SD card (Core0 will save remaining events to the SD card)
unsigned long count_SD_D2;
unsigned long count_SD_coincident;

int cpm_D1 = 0;                  //count per minute for the detector 1
int cpm_D2 = 0;                  //count per minute for the detector 2
int cpm_coincident = 0;          //count per minute for coincident events 

// variables for detectors
int D1_threshold; // in ADC units (0-4096) (this will be calculate from the detectorX_threshold_mV in the main section)
int D2_threshold; // in ADC units (0-4096)

float avg_reading_mv;         //average detector pulse voltage in coincident mode
float raw_readings_ADC_mv[3]; //ADC readings 

//temporary buffers to save events temporary (ring buffer "like"). 
//Can handle 10 events in the queue. Can change according to your needs 
char timestr_D1[10][22];          //temporary buffer to save time of events (detector 1 events)
char timestr_D2[10][22];          //temporary buffer to save time of events (detector 2 events)
char timestr_coincident[10][22];  //temporary buffer to save time of coincident events
float reading_mv_D1[10];          //temporary buffer to save ADC reading of evens (detector 1 events)
float reading_mv_D2[10];          //temporary buffer to save ADC reading of evens (detector 2 events)
float reading_mv_coincident[10];  //temporary buffer to save ADC reading of coincident evens

//some variables keep track of time
unsigned long prv_minute = 0;                //used in the main loop
unsigned long prv_minute2 = 0;               //used in the core0 (2nd core) loop
unsigned long minute_count = 1;    //minutes till detector last restart
unsigned long prv_millies = 0;    //used in the main loop
unsigned long prv_millies2 = 0;   //used in the core0 loop

//buzzer and led turn on in non-blocking way
boolean buzzer_active = false;
boolean LED_active = false;
unsigned long  buzzer_start_millis;
unsigned long  LED_start_millis;

//some temporary variables for debug stuff
unsigned int cn3;
unsigned long t = 0;

//task object for 2nd core of MCU
TaskHandle_t core0; //2nd core of MCU

unsigned int cn1;

//setup code. this run once when powered up
void setup()
{
  Serial.begin(115200);         //initialize Serial port for debugging

  //set I/O directions
  pinMode(PUSH_BUTTON, INPUT);  //Push button as an input 
  pinMode(LED, OUTPUT);         //LED as an output
  digitalWrite(LED, HIGH);      //LED is active LOW, write HIGH to turn off

  pinMode(DETECTOR1_RESET, OUTPUT); //reset pin of the detector 1 as an output
  pinMode(DETECTOR2_RESET, OUTPUT); //reset pin of the detector 2 as an output

  //if the reset switch is enable from the portal, attach the reset interrupt vector to the reset pin
  if (resetSW == true)
  {
    attachInterrupt(PUSH_BUTTON, isr_button_press, RISING);
  }

  init_buzzer();      // initialize the buzzer
  buzzer_short_beep();//short beep sound

  // initialize the displays
  init_display_TFT();    //TFT
  init_display_OLED();   //OLED
  delay(100);

  // initialize bluetooth (not using currently)
  #ifdef _BLUETOOTH_ON           
    SerialBT.begin("ESP32test"); //Bluetooth device name
    Serial.println("The device started, now you can pair it with bluetooth!");
    BTserialBuffer[0] = 0;
    delay(3000);
  #endif

  serial_print_dashes(30);
  Serial.println(" ");

  //initialize and connect to WIFI network
  //20 = number of retries
  connect_wifi();
  delay(500);

  //Show display welcome texts and icons (OLED display)
  display.clearDisplay();
  display_icons_OLED();
  display.setCursor(0, 32);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println("  WELCOME");
  display.setTextSize(1);
  display.println();
  display.println("   Initializing....");
  display.display();

  //Show display welcome texts and icons (TFT display)
  tft.fillScreen(ST77XX_BLACK);
  display_icons_TFT();
  tft.setCursor(0, 60);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_ORANGE);
  tft.println("  WELCOME");
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.println();
  tft.println("   Initializing....");

  serial_print_dashes(30);

  // assign a new task for the 2nd core of the MCU.
  // all the communications were handled by this core
  // create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  // wifi_baton = xSemaphoreCreateMutex();
  xTaskCreatePinnedToCore(
      core0_code, //Task function.
      "Task1",    //name of task.
      10000,      // Stack size of task
      NULL,       // parameter of the task
      1,          //priority of the task
      &core0,     //Task handle to keep track of created task
      0);         // pin task to core 0

  delay(100);

  // initialize the Real Time Clock module
  init_RTC();
  //rtc.adjust(DateTime(2019, 8, 9, 19, 15, 0));

  //update icons on TFT or OLED screen
  display_icons_OLED();
  display_icons_TFT();

  // initialize the SD card
  init_SDcard();

  //update icons on TFT or OLED screen
  display_icons_OLED();
  display_icons_TFT();

  buzzer_short_beep();

  //calculate threshold in ADC units (0-4096) from the mC unites. detectorX_threshold_mV can be set from the portal
  D1_threshold = (float)detector1_threshold_mV * 4096.00 / 3300.00;
  D2_threshold = (float)detector2_threshold_mV * 4096.00 / 3300.00;
  //          3300.0 = 3300 mV = Vcc voltage
  //          4096 = ADC resolution =  2^(12) -1

  //Display another welcome message
  //On the OLED display
  display.clearDisplay();
  display_icons_OLED();
  display.setCursor(0, 32);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println("  WELCOME");
  display.setTextSize(1);
  display.println();
  display.println("        Ready");
  display.display();
  //on TFT display
  tft.fillScreen(ST77XX_BLACK);
  display_icons_TFT();
  tft.setCursor(0, 60);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_ORANGE);
  tft.println("  WELCOME");
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(1);
  tft.println();
  tft.println("        Ready");

  //create new files to write data
  //will open a fresh file after a restart
  createNewFile();
  //load last upload data file path and positions
  load_last_upload_state();
}

//the loop. this will run repeatedly:
void loop()
{

  //runs section will run once per every one second
  if ((millis() - prv_millies) > 1000)
  {
    //update time and get new time String. (this will be used when saving events data/time to the SD card)
    now = rtc.now();
    snprintf(timestr, 22, "%04d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());

    //this code section will run once per every minute
    if (now.minute() != prv_minute)
    {
      //calculate CPM (count per minute)
      cpm_D1 = count_D1 - countPrv_D1;
      cpm_D2 = count_D2 - countPrv_D2;
      cpm_coincident = count_coincident - countPrv_coincident;

      //rest with current values
      prv_minute = now.minute();
      countPrv_D1 = count_D1;
      countPrv_D2 = count_D2;
      countPrv_coincident = count_coincident;
      minute_count++;
    }

    prv_millies = millis();
    Serial.printf(">> Loop Count %d \r\n", cn1);
    cn1=0;
      
  }

//////////////////PULSE DETECTION CODE\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

  //These variables become True if ADC registers a pulse
  boolean pulse_detected_1 = false; 
  boolean pulse_detected_2 = false;

  ///////////////////////////Read Detector 1\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
  int a1[3];
  a1[0] = analogRead(DETECTOR1_ADC);  //read adc
  if (a1[0] > D1_threshold)
  {
    a1[1] = analogRead(DETECTOR1_ADC);
    if (a1[1] > D1_threshold)
    {

        
        pulse_detected_1 = true;    //event detected on the detector1
        count_D1++;                 //increment the number of counts
        
        //Push data to the ring buffer. (Buffer length = 10)
        int pos = count_D1 % 10;   //take the index of the ring buffer reserved for the event (depend on num of counts)
        for (int p = 0; p < 22; p++)  timestr_D1[pos][p] = timestr[p]; //Copy timestr string to the correct index (pos) of ring buffer 
        reading_mv_D1[pos] = (float)(a1[0] + a1[1]) * 3300.0 / 4096.00 / 2.0;//Copy reading_mv to the correct index (pos) of ring buffer
        /*
        Note 1:
          Ring buffer can hold maximum of 10 events in the queue. (may change according to your needs)
          Todo: implement a code to detect buffer overload condition. (this  condition is highly unlikely, But good have)

          Current ring buffer index will be taken by taking the modules of the count number.
          ex:   if the count number is 12, Data will go to 2 nd index of the ring buffer. (when count=12, pos=2)  
          Core0 (2nd core) will read this buffer and put data the SD card.
        
        Note 2:
          when copying timestr, 22 is the length of timestr array

        Note 3:
          3300.0 = 3300 mV = Vcc voltage
          4096 = ADC resolution =  2^(12) -1
          3 = 3 samples

        Note 4:
          These pushing to ring buffer code section takes negligible amount of time compared to time take to read the ADC
        */
        
        //Todo: Move Serial print to 2nd Core
        //Serial.printf(" ###D1 %d , %s , %f mv \r\n", count_D1, timestr_D1[pos], reading_mv_D1[pos]);
      
    }
  }

  ///////////////////////////Read Detector 2\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
  int a2[3];
  a2[0] = analogRead(DETECTOR2_ADC);  //read ADC
  if (a2[0] > D2_threshold)           
  {
    a2[1] = analogRead(DETECTOR2_ADC); 
    if (a2[1] > D2_threshold)
    {

        pulse_detected_2 = true;  //event detected on the detector2
        count_D2++;               //increment the number of counts

        //Push data to the ring buffer. (Buffer length = 10) [Read notes above]
        int pos = count_D2 % 10;  //take the index of the ring buffer reserved for the event (depend on num of counts)
        for (int p = 0; p < 22; p++) timestr_D2[pos][p] = timestr[p]; //Copy timestr string to the correct index (pos) of ring buffer 
        reading_mv_D2[pos] = (float)(a2[0] + a2[1]) * 3300.0 / 4096.00 / 2.0;//Copy reading_mv to the correct index (pos) of ring buffer

        //Todo: Move Serial priint to 2nd Core
        //Serial.printf(" ###D2 %d , %s , %f mv \r\n", count_D2, timestr_D2[pos], reading_mv_D2[pos]);
      
    }
  }

  /////////////////////Check coincident logic\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
  if ((pulse_detected_1 == true) && (pulse_detected_2 == true))
  {

    //x count++;
    //x avg_cpm = (float)count / (float)minute_count;

    count_coincident++;     //increment the number of counts

    //calculate average ADC voltage
    float a1_avg = (float)(a1[0] + a1[1] ) * 3300.0 / 4096.00 / 2.0;
    float a2_avg = (float)(a2[0] + a2[1] ) * 3300.0 / 4096.00 / 2.0;
    raw_readings_ADC_mv[0] = a1_avg;    //only need for web page
    raw_readings_ADC_mv[1] = a2_avg;    //only need for web page
    avg_reading_mv = (raw_readings_ADC_mv[0] + raw_readings_ADC_mv[1]) / 2.0;
    
    //Push data to the ring buffer. (Buffer length = 10) [Read notes above]
    int pos = count_coincident % 10;
    reading_mv_coincident[pos] = avg_reading_mv; //Copy reading_mv to the correct index (pos) of ring buffer
    for (int p = 0; p < 22; p++) timestr_coincident[pos][p] = timestr[p]; // //Copy timestr string to the correct index (pos) of ring buffer 

    //Todo: Move Serial print to 2nd Core  
    Serial.printf(" ###D1/D2 %d , %s , %f mv \r\n", count_coincident, timestr_coincident[pos], reading_mv_coincident[pos]);

    //set turn on and set start millis for Buzzer and led
    if (LEDOn == true){
        digitalWrite(LED, LOW);     //turn on LED, (active lOW LED)
        LED_active = true;          //set variable to true
        LED_start_millis = millis();//keep track of turn on time
    }
    if (buzzerOn == true){
        int buzzer_fre = 1.5 * avg_reading_mv; //beep sound frequency
        buzzer(buzzer_fre, buzzer_volume2);    //turn on buzzer
        buzzer_active = true;                 //set variable to true
        buzzer_start_millis = millis();//keep track of turn on time
     }
    
    
  }

  /////////////////////Send reset signals\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
  //Send reset pulses to the detectors1
  if (pulse_detected_1 == true)
  {
    digitalWrite(DETECTOR1_RESET, HIGH);
    delayMicroseconds(10);
    digitalWrite(DETECTOR1_RESET, LOW);
  }
  //Send reset pulse to the detectors2
  if (pulse_detected_2 == true)
  {
    digitalWrite(DETECTOR2_RESET, HIGH);
    delayMicroseconds(10);
    digitalWrite(DETECTOR2_RESET, LOW);
  }


   /////////////////////Buzzer and led\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
   //Turn on buzzer and LED after an event in non-blocking way
  if ( (buzzer_active == true) && (millis()-buzzer_start_millis > 300)){
      //stop buzzer
      buzzer(2000, 0);
      buzzer_active = false;
  }
  if ( (LED_active == true) && (millis()-LED_start_millis > 500)){
      //turn off LED 
      digitalWrite(LED, HIGH); //active low LED
      LED_active = false;
  }
  cn1++;
  //delay(1000);

}



/*
TODO:
  Move serial prints to secondary core
*/

///////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///////////////////////////////////////////////Core0 code\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//this is the setup and loop of the core0 (2nd core)
//cores can share global variable
void core0_code(void *pvParameters)
{

  //debug message
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  //10 seconds delay until main core initialize modules
  delay(10000);

  //initialize GPS module
  init_gps();
  //xSemaphoreTake( wifi_baton, portMAX_DELAY );

  delay(100);

  //initialize MQTT server
  client.setServer(mqttServer.c_str(), mqttPort);

  display_clear_TFT();

  //This is the loop of the CORE0 (this will run repeatedly:)
  for (;;)
  {
    ///////////////////////////Upload to the server\\\\\\\\\\\\\\\\\\\\\\\\\\//
    if ((uploadOn == true) && (upload_failed_count<3)) 
    {
      t = millis();
      upload_file();
      Serial.print("U :> ");
      Serial.print(millis() - t);
      Serial.println(" ms time in upload loop");
      delay(10);
    }

    ///////////////////////////Wifi portal\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
    //if a wifi client (smart phone) try to connect to the device, this function will handle it
    portal_handleClient();

    //////////////////////////////GPS\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
    if (gpsTimeOn == true) //if GPS is enabled (thru the portal)
    {
      update_gps_data();
      //wait for a 3D fix
      if (fix_mode == 3)
      { 
        clock_status = 2;     //set clock states to GPS
        if (gps_isUpdated == true)// wait for confirm update
        {
          //Serial.printf("<GPS> %.6f,%.6f,%.1f  SAT:%d  FIX:%d  AGE:%ds \t%d\n\r", latitude, longitude, altitude, satellites, fix_mode, gps_age/1000,cn3);
          gps_isUpdated = false;  //clear update_status variable
        }
      }
      else
      {
        clock_status = 1;         // if no GPS, set clock mode to RTC
      }
    }
    cn3++;

    ///////////////////////////save to SD\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
    //save events (data) from detector 1 to the SD card
    //while the number of detected counts are higher that than number of events saved in the SD card
    //save these events to the card (append new events to the data file)
    //if (count_D1 > count_SD_D1)  <NEW EDIT>
    while (count_D1 > count_SD_D1) 
    {
      //index of the ring buffer to retrieve data
      int pos1 = (count_SD_D1 + 1) % 10;
      //construct the string 
      char wbuffer[100]; 
      snprintf(wbuffer, 100, "%d,%s,%f,%.6f,%.6f,%.1f\r\n", count_SD_D1 + 1, timestr_D1[pos1], reading_mv_D1[pos1], latitude, longitude, altitude);
      //append to the end of file
      if (appendFile(SD, file_path_D1, wbuffer)) 
      {
        //append file function will return True if write to SD is success
        Serial.printf("SD1:> Added to SD -> %s \r\n", wbuffer);
        count_SD_D1++;  //we have added to event to the SD card, increment SD count by one
      }else{
        break; //if cant write to the SD card, exit form the loop
      }

      /*
      NOTE:
        appendFile() functions
          SD = SD card object from the library
          file_path_D1 = path of the file to write data (including file name)
          wbuffer = String to append to the end of the file (append as a new row)
      */

      // detect buffer overflow condition
      if (count_D1>count_SD_D1+10){
        Serial.println("SD1:> Buffer overflow. Some data may loss");
        display_printError("D1 Buffer","Overflow",1000);
        count_SD_D1=count_D1;
        break;
      }

    }

    //save events from detector 2 to the SD card
    while (count_D2 > count_SD_D2)
    {
      //update SD file
      int pos1 = (count_SD_D2 + 1) % 10;
      char wbuffer[100];
      snprintf(wbuffer, 100, "%d,%s,%f,%.6f,%.6f,%.1f\r\n", count_SD_D2 + 1, timestr_D2[pos1], reading_mv_D2[pos1], latitude, longitude, altitude);
      if (appendFile(SD, file_path_D2, wbuffer))
      {
        Serial.printf("SD2:> Added to SD -> %s \r\n", wbuffer);
        count_SD_D2++;
      } else{
        break;
      }

      // detect buffer overflow condition
      if (count_D2>count_SD_D2+10){
        Serial.println("SD2:> Buffer overflow. Some data may loss");
        display_printError("D2 Buffer","Overflow",1000);
        count_SD_D2=count_D2;
        break;
      }
    }

    //save coincident events to the SD card
    while (count_coincident > count_SD_coincident)
    {
      //update SD file
      int pos1 = (count_SD_coincident + 1) % 10;
      char wbuffer[100];
      snprintf(wbuffer, 100, "%d,%s,%f,%.6f,%.6f,%.1f,%s\r\n", count_SD_coincident + 1, timestr_coincident[pos1], reading_mv_coincident[pos1], latitude, longitude, altitude, DetectorLocation);
      if (appendFile(SD, file_path, wbuffer))
      {
        Serial.printf("SD_C:> Added to SD -> %s \r\n", wbuffer);
        count_SD_coincident++;
      } else{
        break;
      }

      // detect buffer overflow condition
      if (count_coincident>count_SD_coincident+10){
        Serial.println("SD:> count_SD_coincident Buffer overflow. Some data may loss");
        display_printError("DC Buffer","Overflow",1000);
        count_SD_coincident=count_coincident;
        break;
      }
    }

    //this section will once run every minute
    if (now.minute() != prv_minute2)
    {
      ///////////////////////////Save CPM to SD\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
      /*
      char wbuffer1[100];
      snprintf(wbuffer1, 100, "%d,%s,%d,%.6f,%.6f,%.1f,%s\r\n", minute_count, timestr, cpm_D1,latitude,longitude,altitude, DetectorLocation);
      if (appendFile(SD, file_path_CPMD1, wbuffer1))
      {
        Serial.printf("SD_D1_CPM:> Added to SD -> %s \r\n", wbuffer1);
      }

      char wbuffer2[100];
      snprintf(wbuffer2, 100, "%d,%s,%d,%.6f,%.6f,%.1f,%s\r\n", minute_count, timestr, cpm_D2,latitude,longitude,altitude, DetectorLocation);
      if (appendFile(SD, file_path_CPMD2, wbuffer2))
      {
        Serial.printf("SD_D2_CPM:> Added to SD -> %s \r\n", wbuffer2);
      }
      */
      
      #ifdef TFT
        if (displayOn == true)
        {

          //clear up the display fully and show information on the display once per minute (like restarting the LCD)
          display_clear_TFT();
          display_icons_TFT();
          display_backlight_TFT(HIGH);  //turn on backlight ON the LCD
        } else {
          display_backlight_TFT(LOW);  //turn on backlight OFF the LCD
        }
      #endif

      upload_failed_count=0; //reset upload_failed_count every 1 minute

      prv_minute2 = now.minute();

      //this section will run once per day
      if (now.day() != file_created_day)
      {
        //create new set of file at 12.00 AM each day
        createNewFile();
      }
    }
    
    ///////////////////////////update display\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
    //update display every 300 ms
    if ((millis() - prv_millies2) > 300)
    {
      #ifdef TFT
        if (displayOn == true)
        {
          //show information on the display. 
          //(not a restart. Just draw texts on top of the existing texts. This is faster than fully clear and redraw the display)
          update_display_TFT();
          display_backlight_TFT(HIGH);
        } else {
          display_backlight_TFT(LOW);
        }
      #else 
        //OLED display
        if (displayOn = true)
            update_icons_OLED();  
            update_display_OLED();
      #endif
      prv_millies2 = millis();
    }
    
    /*
      Serial debug prints
    */


    delay(10); //small delay for stability
  }
}

///////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////wifi\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void connect_wifi()
{
  // xSemaphoreTake( wifi_baton, portMAX_DELAY );

  int cnt = 0;

  //show infomation on OLED display
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println(F("Connecting to WIFI"));
  display.display();

  //show information on TFT display
  tft.setCursor(0, 40);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(1);
  tft.println(F("Connecting to WIFI..."));

  /*
  initiate portal (form autoConnect library)and try to connect to last know wifi AP
  if cant find the last known AP, this will automatically create a wifi access point (Hotspot)
  Then user can connect to this wifi AP and configure the detector
  IP address of the IP will show on the display
  debug information will send to the serial port also
  */ 
  Serial.print(F("Connecting to via auto connect"));
  initiate_auto_connect(); //connect to wifi with autoconnect 

  //if WIFI connected
  if (WiFi.status() == WL_CONNECTED)
  {
    
    //send some debug information to the serial port
    Serial.println("");
    Serial.print(F("WIFI Connected :"));
    char ssid2[20];
    WiFi.SSID().toCharArray(ssid2, 20);
    Serial.println(ssid2);
    Serial.println(F("IP address: "));
    ip = WiFi.localIP().toString();
    char ip_buffer[15];
    ip.toCharArray(ip_buffer, 14);
    Serial.println(ip_buffer);

    //set wifi status to Ture
    wifi_status = 1; 

    //display info on OLED
    display.println();
    display.println(F("        WIFI"));
    display.setTextSize(2);
    display.println(F(" Connected"));
    display.setTextSize(1);
    display.print("SSID :");
    display.println(ssid2);
    display.print("   ");
    display.println(ip_buffer);
    display.display();

    //display info on TFT
    tft.println();
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(10,tft.getCursorY());
    tft.println(F("  WIFI"));
    tft.setCursor(10,tft.getCursorY());
    tft.println(F("Connected"));
    tft.setTextSize(1);
    tft.println();
    tft.print(" SSID :");
    tft.println(ssid2);
    tft.print(" IP : ");
    tft.println(ip_buffer);

    delay(3000);
    
  }
  else
  { 
    //Print error on displays and serial monitor if could not connect to a wifi AP or could not initiate hotspot
    Serial.println(F("couldnt connect to wifi"));

    display.println();
    display.setTextSize(2);
    display.println(F(" ERROR"));
    display.print(F("NO WIFI"));
    display.display();

    tft.println();
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_RED);
    tft.println(F("  NO WIFI"));
    delay(3000);

    wifi_status = 0;
  }

  //xSemaphoreGive(wifi_baton);
}

///////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
////////////////////////////////////////////new file\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void createNewFile()
{  //this function will create new set of files


  DateTime now = rtc.now(); //get the current time from the RTC
 
  //construct folder name and file name
  //ex: folder_name= > "DATA-2020-06-23"
  //ex: file_name= > "DATA-2020-06-23_01:12:00"
  snprintf(folder_name, 16, "DATA-%04d-%02d-%02d", now.year(), now.month(), now.day());
  snprintf(file_name, 25, "DATA-%04d-%02d-%02d_%02d-%02d-%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
  

  //file to save coincident events
  //ex: name => DATA-2020-06-23_01:12:00.x.csv
  //ex:: file path => DATA-2020-06-23/DATA-2020-06-23_01:12:00.x.csv
  snprintf(file_path, 49, "/%s/%s.x.csv", folder_name, file_name);
  Serial.println(file_path);
  makeFile(SD, file_path, "Count#,Date/Time,Amplitude,LAT,LON,ALT,Detector\r\n"); //create a new  file


  //file to save events from the detector 1
  //ex: name => D1-2020-06-23_01:12:00.x.csv
  //ex:: file path => DATA-2020-06-23/D1-2020-06-23_01:12:00.csv
  snprintf(file_name, 25, "D1-%04d-%02d-%02d_%02d-%02d", now.year(), now.month(), now.day(), now.hour(), now.minute());
  snprintf(file_path_D1, 49, "/%s/%s.csv", folder_name, file_name);
  Serial.println(file_path_D1);
  makeFile(SD, file_path_D1, "Count#,Date/Time,Amplitude,LAT,LON,ALT \r\n");

  //file to save events from the detector 2
  //ex: name => D2-2020-06-23_01:12:00.x.csv
  //ex:: file path => DATA-2020-06-23/D2-2020-06-23_01:12:00.csv
  snprintf(file_name, 25, "D2-%04d-%02d-%02d_%02d-%02d", now.year(), now.month(), now.day(), now.hour(), now.minute());
  snprintf(file_path_D2, 49, "/%s/%s.csv", folder_name, file_name);
  Serial.println(file_path_D2);
  makeFile(SD, file_path_D2, "Count#,Date/Time,Amplitude,LAT,LON,ALT \r\n");
  
  /*
  snprintf(file_name, 25, "CPM_D1-%04d-%02d-%02d_%02d-%02d", now.year(), now.month(), now.day(), now.hour(), now.minute());
  snprintf(file_path_CPMD1, 49, "/%s/%s.csv", folder_name, file_name);
  Serial.println(file_path_CPMD1);
  makeFile(SD, file_path_CPMD1, "Minute_Count,Date/Time,CPM,LAT,LON,ALT,Detector \r\n");

  snprintf(file_name, 25, "CPM_D2-%04d-%02d-%02d_%02d-%02d", now.year(), now.month(), now.day(), now.hour(), now.minute());
  snprintf(file_path_CPMD2, 49, "/%s/%s.csv", folder_name, file_name);
  Serial.println(file_path_CPMD2);
  makeFile(SD, file_path_CPMD2, "Minute_Count,Date/Time,CPM,LAT,LON,ALT,Detector \r\n");
  */
  
  //update lasr created day
  file_created_day = now.day();
}

//ESP 32 restart interrupt vector for button press
void IRAM_ATTR isr_button_press()
{
  ESP.restart();
}