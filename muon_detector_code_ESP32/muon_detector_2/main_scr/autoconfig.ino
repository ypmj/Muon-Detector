//this file handle the portal, connect to wifi and remembering settings

#include <Arduino.h>
#include "autoconfig.h"
#include "time.h"

#include <AutoConnect.h>
#include <SPIFFS.h>
#include <HTTPClient.h>
#include <FS.h>

#include "display_OLED.h"
#include "Buzzer.h"
#include "SD_card.h"
#include "display_TFT.h"


typedef WebServer WiFiWebServer;
AutoConnect portal;
AutoConnectConfig config;
WiFiClient wifiClient;

void getParams_serverSettings(AutoConnectAux &aux);
void getParams_generalSettings(AutoConnectAux &aux);
void getParams_timeServersettings(AutoConnectAux &aux);

String loadParams_serverSettings(AutoConnectAux &aux, PageArgument &args);
String loadParams_generalSettings(AutoConnectAux &aux, PageArgument &args);
String loadParams_timeServersettings(AutoConnectAux &aux, PageArgument &args);

String saveParams_serverSettings(AutoConnectAux &aux, PageArgument &args);
String saveParams_generalSettings(AutoConnectAux &aux, PageArgument &args);
String saveParams_timeServersettings(AutoConnectAux &aux, PageArgument &args);

String saveParams_timeSet(AutoConnectAux &aux, PageArgument &args);
String saveParams_timeSync(AutoConnectAux &aux, PageArgument &args);

//web elemets (custom web page)
//READ: https://hieromon.github.io/AutoConnect/
static const char AUX_setting[] PROGMEM = R"raw(

[
  {
    "title": "Server Setting",
    "uri": "/server_setting",
    "menu": true,
    "element": [
      {
        "name": "style",
        "type": "ACStyle",
        "value": "label+input,label+select{position:sticky;left:120px;width:230px!important;box-sizing:border-box;}"
      },
      {
        "name": "header",
        "type": "ACText",
        "value": "<h2>Server settings</h2>",
        "style": "text-align:center;color:#2f4f4f;padding:10px;"
      },
      {
        "name": "uploadOn",
        "type": "ACCheckbox",
        "value": "uploadon",
        "label": "Upload on/off",
        "checked": true
      },
      {
        "name": "newline",
        "type": "ACElement",
        "value": "<hr>"
      },
      {
        "name": "mqttServer",
        "type": "ACInput",
        "value": "",
        "label": "Mqtt Server",
        "pattern": "^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\\-]*[A-Za-z0-9])$",
        "placeholder": "MQTT broker server"
      },
      {
        "name": "mqttPort",
        "type": "ACInput",
        "label": "Mqtt port",
        "pattern": "^[0-9]{1,6}$"
      },
      {
        "name": "mqttId",
        "type": "ACInput",
        "label": "Mqtt ID"
      },
      {
        "name": "mqttUsername",
        "type": "ACInput",
        "label": "Mqtt Username"
      },
      {
        "name": "mqttPassword",
        "type": "ACInput",
        "label": "Mqtt Password"
      },
      {
        "name": "newline",
        "type": "ACElement",
        "value": "<hr>"
      },
      {
        "name": "save",
        "type": "ACSubmit",
        "value": "Save",
        "uri": "/server_save"
      },
      {
        "name": "discard",
        "type": "ACSubmit",
        "value": "Discard",
        "uri": "/_ac"
      }
    ]
  },
  {
    "title": "Server Setting",
    "uri": "/server_save",
    "menu": false,
    "element": [
      {
        "name": "caption",
        "type": "ACText",
        "value": "<h4>Parameters saved as:</h4>",
        "style": "text-align:center;color:#2f4f4f;padding:10px;"
      },
      {
        "name": "parameters",
        "type": "ACText"
      },
      {
        "name": "back",
        "type": "ACSubmit",
        "value": "Back",
        "uri": "/_ac"
      }
    ]
  },
  {
    "title": "General Setting",
    "uri": "/general_Setting",
    "menu": true,
    "element": [
      {
        "name": "style",
        "type": "ACStyle",
        "value": "label+input,label+select{position:sticky;left:120px;width:230px!important;box-sizing:border-box;}"
      },
      {
        "name": "header",
        "type": "ACText",
        "value": "<h2>General Settings</h2>",
        "style": "text-align:center;color:#2f4f4f;padding:10px;"
      },
      {
        "name": "DetectorID",
        "type": "ACInput",
        "label": "Detector ID",
        "pattern": "^[0-9]{1,2}$"
      },
      {
        "name": "DetectorLocation",
        "type": "ACInput",
        "label": "Detector Location"
      },
      {
        "name": "newline",
        "type": "ACElement",
        "value": "<hr>"
      },
      {
        "name": "gpsTimeOn",
        "type": "ACCheckbox",
        "value": "uploadon",
        "label": "GPS time on/off",
        "checked": true
      },

      
      {
        "name": "buzzerOn",
        "type": "ACCheckbox",
        "value": "uploadon",
        "label": "BUZZER on/off",
        "checked": true
      },
      {
        "name": "LEDOn",
        "type": "ACCheckbox",
        "value": "uploadon",
        "label": "LED 2 on/off",
        "checked": true
      },
      {
        "name": "displayOn",
        "type": "ACCheckbox",
        "label": "DISPLAY on/off",
        "checked": true
      },
      {
        "name": "resetSW",
        "type": "ACCheckbox",
        "value": "uploado1n",
        "label": "External Reset switch on/off",
        "checked": true
      },
      {
        "name": "newline",
        "type": "ACElement",
        "value": "<hr>"
      },
      {
        "name": "detector1_threshold_mV",
        "type": "ACInput",
        "label": "Detector 1 Threshold (mV)",
        "pattern": "^[0-9]{1,4}$"
      },
      {
        "name": "detector2_threshold_mV",
        "type": "ACInput",
        "label": "Detector 2 Threshold (mV)",
        "pattern": "^[0-9]{1,4}$"
      },
      {
        "name": "buzzer_volume1",
        "type": "ACInput",
        "label": "Buzzer volume 1 (0-255)",
        "pattern": "^[0-9]{1,3}$"
      },
      {
        "name": "buzzer_volume2",
        "type": "ACInput",
        "label": "Buzzer volume 2 (0-255)",
        "pattern": "^[0-9]{1,3}$"
      },
      {
        "name": "newline",
        "type": "ACElement",
        "value": "<hr>"
      },
      {
        "name": "save",
        "type": "ACSubmit",
        "value": "Save",
        "uri": "/general_save"
      },
      {
        "name": "discard",
        "type": "ACSubmit",
        "value": "Discard",
        "uri": "/_ac"
      }
    ]
  },
  {
    "title": "General Setting",
    "uri": "/general_save",
    "menu": false,
    "element": [
      {
        "name": "caption",
        "type": "ACText",
        "value": "<h4>Parameters saved as:</h4>",
        "style": "text-align:center;color:#2f4f4f;padding:10px;"
      },
      {
        "name": "parameters",
        "type": "ACText"
      },
      {
        "name": "back",
        "type": "ACSubmit",
        "value": "back",
        "uri": "/_ac"
      }
    ]
  },
  {
    "title": "Date time Setting",
    "uri": "/date_time_setting",
    "menu": true,
    "element": [
      {
        "name": "style",
        "type": "ACStyle",
        "value": "label+input,label+select{position:sticky;left:120px;width:230px!important;box-sizing:border-box;}"
      },
      {
        "name": "header",
        "type": "ACText",
        "value": "<h2>Date time settings</h2>",
        "style": "text-align:center;color:#2f4f4f;padding:10px;"
      },
      {
        "name": "newline",
        "type": "ACElement",
        "value": "<hr>"
      },
      {
        "name": "ntpserver",
        "type": "ACInput",
        "label": "NTP Server",
        "pattern": "^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\\-]*[A-Za-z0-9])$",
        "value": ""
      },
      {
        "name": "date_time",
        "type": "ACInput",
        "label": "Date/Time"
      },
      {
        "name": "setTime",
        "type": "ACSubmit",
        "value": "Manual Set Date/Time",
        "uri": "/time_set"
      },
      {
        "name": "sync",
        "type": "ACSubmit",
        "value": "Sync Date/Time with NTP",
        "uri": "/timeserver_save"
      },
      {
        "name": "discard",
        "type": "ACSubmit",
        "value": "Discard",
        "uri": "/_ac"
      }
    ]
  },
  {
    "title": "Date time Setting",
    "uri": "/timeserver_save",
    "menu": false,
    "element": [
      {
        "name": "caption",
        "type": "ACText",
        "value": "<h4>Parameters saved as:</h4>",
        "style": "text-align:center;color:#2f4f4f;padding:10px;"
      },
      {
        "name": "parameters",
        "type": "ACText"
      },
      {
        "name": "back",
        "type": "ACSubmit",
        "value": "back",
        "uri": "/_ac"
      }
    ]
  }
]

)raw";

//initiate auto_connect functions
void initiate_auto_connect()
{
  //enable SPIFFS in side the ESP32
  SPIFFS.begin(true);

  //congig hotspot (CP) when no wifi connection is available
  config.title = "Muon Detector settings";  //title of the device
  config.apid = "sasa_ap";    //hotspot (CP) name 
  config.psk = "12345678";    //hotspot (CP) password
  config.homeUri = "/";       //home url as root
  config.autoReconnect = true;//turn on auto reconnect if wifi lost  
  config.portalTimeout = 10000;   //exit from the 
  config.retainPortal = true;
  config.apip = IPAddress(192, 168, 1, 1);    //hotspot root ip
  config.gateway = IPAddress(192, 168, 1, 1); //hotspot gateway 
  
  //push config. read documentation
  portal.config(config);
  portal.onDetect(startCP); //call this function if no wifi connection startCP

  //load auxiliary configs saved in the SPIFFS. read documentation
  if (portal.load(FPSTR(AUX_setting)))
  { 
    //MQTT server settings. (MQTT user name, password, ect)
    AutoConnectAux &server_setting = *portal.aux("/server_setting");
    PageArgument server_args;
    loadParams_serverSettings(server_setting, server_args);

    //general settings (ex: detector settings)
    AutoConnectAux &general_setting = *portal.aux("/general_Setting");
    PageArgument general_args;
    loadParams_generalSettings(general_setting, general_args);

    //date time and ntp settings
    AutoConnectAux &time_setting = *portal.aux("/date_time_setting");
    PageArgument date_time_args;
    loadParams_timeServersettings(time_setting, date_time_args);

    //add new web sections
    portal.on("/server_setting", loadParams_serverSettings);
    portal.on("/general_Setting", loadParams_generalSettings);
    portal.on("/date_time_setting", loadParams_timeServersettings);

    portal.on("/server_save", saveParams_serverSettings);
    portal.on("/general_save", saveParams_generalSettings);
    portal.on("/timeserver_save", saveParams_timeServersettings);
  }
  else
    Serial.println("load error");

  //after config is done, try to connect to the last known wifi
  Serial.println("WiFi connecting ...... ");
  if (portal.begin())
  {
    Serial.println("connected:" + WiFi.SSID());
    Serial.println("IP:" + WiFi.localIP().toString());

    WiFiWebServer &webServer = portal.host();
    webServer.on("/", handleRoot);
  }
  else
  {
    Serial.println("connection failed:" + String(WiFi.status()));
    Serial.println("Needs WiFi connection to start publishing messages");
    //portal.end();
  }
}

//calling this function will start the hotspot  when there is no wifi connection
bool startCP(IPAddress ip)
{
  //print hotspot IP address
  //you can open the web page by connecting the ip address
  Serial.println("Captive portal started, IP:  192.168.1.1 " + WiFi.localIP().toString());
  display_printError_OLED_ONLY(" NO WIFI CONNECTION", "   Soft AP started   Portal on 192.168.1.1", 1);

  tft.setTextSize(1);
  tft.println();
  tft.setTextColor(ST77XX_WHITE);
  tft.println(" NO WIFI CONNECTION");
  tft.println("   Soft AP started");
  tft.setTextColor(ST77XX_GREEN);
  tft.println();
  tft.println("Portal on 192.168.1.1");

  delay(10000);

  return true;
}

//call this function periodically to handle Clients conneting to the ESP
void portal_handleClient()
{
  portal.handleClient();
}

//functiions to save paramets by web page
//https://hieromon.github.io/AutoConnect/acintro.html
void getParams_serverSettings(AutoConnectAux &aux)
{
  uploadOn = aux["uploadOn"].as<AutoConnectCheckbox>().checked;
  mqttServer = aux["mqttServer"].value;
  mqttServer.trim();
  String p = aux["mqttPort"].value;
  mqttPort = p.toInt();
  mqttId = aux["mqttId"].value;
  mqttId.trim();
  mqttUsername = aux["mqttUsername"].value;
  mqttUsername.trim();
  mqttPassword = aux["mqttPassword"].value;
  mqttPassword.trim();
}

void getParams_generalSettings(AutoConnectAux &aux)
{
  String id = aux["DetectorID"].value;
  DetectorID = id.toInt();
  DetectorLocation = aux["DetectorLocation"].value;
  DetectorLocation.trim();

  gpsTimeOn = aux["gpsTimeOn"].as<AutoConnectCheckbox>().checked;
  //detector1On = aux["detector1On"].as<AutoConnectCheckbox>().checked;
  //detector2On = aux["detector2On"].as<AutoConnectCheckbox>().checked;
  buzzerOn = aux["buzzerOn"].as<AutoConnectCheckbox>().checked;
  LEDOn = aux["LEDOn"].as<AutoConnectCheckbox>().checked;
  displayOn = aux["displayOn"].as<AutoConnectCheckbox>().checked;
  resetSW = aux["resetSW"].as<AutoConnectCheckbox>().checked;

  String t1 = aux["detector1_threshold_mV"].value;
  detector1_threshold_mV = t1.toInt();
  String t2 = aux["detector2_threshold_mV"].value;
  detector2_threshold_mV = t2.toInt();
  String b1 = aux["buzzer_volume1"].value;
  buzzer_volume1 = b1.toInt();
  String b2 = aux["buzzer_volume2"].value;
  buzzer_volume2 = b2.toInt();
}

String loadParams_serverSettings(AutoConnectAux &aux, PageArgument &args)
{
  (void)(args);
  File param = SPIFFS.open("/server_settings.json", "r");
  if (param)
  {
    if (aux.loadElement(param))
    {
      getParams_serverSettings(aux);
      Serial.println(F("/server_settings.json loaded"));
    }
    else
      Serial.println(F("/server_settings.json  failed to load"));
    param.close();
  }
  else
  {
    Serial.println(F("/server_settings.json open failed"));
  }
  return String("");
}

String loadParams_generalSettings(AutoConnectAux &aux, PageArgument &args)
{
  (void)(args);
  File param = SPIFFS.open("/general_settings.json", "r");
  if (param)
  {
    if (aux.loadElement(param))
    {
      getParams_generalSettings(aux);
      Serial.println(F("/general_settings.json loaded"));
    }
    else
      Serial.println(F("/general_settings.json  failed to load"));
    param.close();
  }
  else
  {
    Serial.println(F("/general_settings.json open failed"));
  }
  return String("");
}

String saveParams_serverSettings(AutoConnectAux &aux, PageArgument &args)
{
  // The 'where()' function returns the AutoConnectAux that caused
  // the transition to this page.
  AutoConnectAux &setting = *portal.aux(portal.where());
  Serial.println(portal.where());
  getParams_serverSettings(setting);

  //AutoConnectInput& mqttserver = setting["mqttserver"].as<AutoConnectInput>();
  // The entered value is owned by AutoConnectAux of /mqtt_setting.
  // To retrieve the elements of /mqtt_setting, it is necessary to get
  // the AutoConnectAux object of /mqtt_setting.

  File param = SPIFFS.open("/server_settings.json", "w");
  setting.saveElement(param, {"uploadOn", "mqttServer", "mqttPort", "mqttId", "mqttUsername", "mqttPassword"});
  param.close();

  // Echo back saved parameters to AutoConnectAux page.
  AutoConnectText &echo = aux["parameters"].as<AutoConnectText>();
  echo.value = "uploadon: " + String(uploadOn == true ? "true" : "false") + "<br>";
  echo.value += "mqttServer: " + mqttServer + "<br>";
  echo.value += "mqttPort : " + String(mqttPort) + "<br>";
  echo.value += "mqttId: " + mqttId + "<br>";
  echo.value += "mqttUsername: " + mqttUsername + "<br>";
  echo.value += "mqttPassword: " + mqttPassword + "<br>";

  return String("");
}

String saveParams_generalSettings(AutoConnectAux &aux, PageArgument &args)
{
  // The 'where()' function returns the AutoConnectAux that caused
  // the transition to this page.
  AutoConnectAux &setting = *portal.aux(portal.where());
  Serial.println(portal.where());
  getParams_generalSettings(setting);

  //AutoConnectInput& mqttserver = setting["mqttserver"].as<AutoConnectInput>();
  // The entered value is owned by AutoConnectAux of /mqtt_setting.
  // To retrieve the elements of /mqtt_setting, it is necessary to get
  // the AutoConnectAux object of /mqtt_setting.

  File param = SPIFFS.open("/general_settings.json", "w");
  setting.saveElement(param, {"DetectorID", "DetectorLocation", "gpsTimeOn", "buzzerOn", "LEDOn", "displayOn", "resetSW", "detector1_threshold_mV", "detector2_threshold_mV", "buzzer_volume1", "buzzer_volume2"});
  //  setting.saveElement(param, {"DetectorID", "DetectorLocation", "gpsTimeOn", "detector1On", "detector2On", "buzzerOn", "LEDOn", "displayOn", "resetSW", "detector1_threshold_mV", "detector2_threshold_mV", "buzzer_volume1", "buzzer_volume2"});

  param.close();

  // Echo back saved parameters to AutoConnectAux page.
  AutoConnectText &echo = aux["parameters"].as<AutoConnectText>();
  echo.value = "DetectorID: " + String(DetectorID) + "<br>";
  echo.value += "DetectorLocation: " + DetectorLocation + "<br> <br>";

  echo.value = "gpsTimeOn: " + String(gpsTimeOn == true ? "true" : "false") + "<br>";
  //echo.value += "detector1On: " + String(detector1On == true ? "true" : "false") + "<br>";
  //echo.value += "detector2On: " + String(detector2On == true ? "true" : "false") + "<br>";
  echo.value += "buzzerOn: " + String(buzzerOn == true ? "true" : "false") + "<br>";
  echo.value += "LEDOn: " + String(LEDOn == true ? "true" : "false") + "<br>";
  echo.value += "displayOn: " + String(displayOn == true ? "true" : "false") + "<br> ";
  echo.value += "resetSW: " + String(resetSW == true ? "true" : "false") + "<br> <br>";

  echo.value += "detector1_threshold_mV: " + String(detector1_threshold_mV) + "<br>";
  echo.value += "detector2_threshold_mV : " + String(detector2_threshold_mV) + "<br>";
  echo.value += "buzzer_volume1: " + String(buzzer_volume1) + "<br>";
  echo.value += "buzzer_volume2: " + String(buzzer_volume2) + "<br>";

  createNewFile();
  display_clear_TFT();
  display_icons_TFT();

  return String("");
}

///////////////////// time\\\\\\\\\\\\\\\\\\\


void getParams_timeServersettings(AutoConnectAux &aux)
{
  ntpserver = aux["ntpserver"].value;
  ntpserver.trim();
}

String loadParams_timeServersettings(AutoConnectAux &aux, PageArgument &args)
{
  (void)(args);
  File param = SPIFFS.open("/time_settings.json", "r");
  if (param)
  {
    if (aux.loadElement(param))
    {
      getParams_timeServersettings(aux);
      Serial.println(F("/time_settings.json loaded"));
    }
    else
      Serial.println(F("/time_settings.json  failed to load"));
    param.close();
  }
  else
  {
    Serial.println(F("/time_settings.json open failed"));
  }

  AutoConnectText &echo = aux["date_time"].as<AutoConnectText>();
  echo.value = timestr;

  return String("");
}

String saveParams_timeServersettings(AutoConnectAux &aux, PageArgument &args)
{
  // The 'where()' function returns the AutoConnectAux that caused
  // the transition to this page.
  AutoConnectAux &setting = *portal.aux(portal.where());
  Serial.println(portal.where());
  getParams_timeServersettings(setting);

  //AutoConnectInput& mqttserver = setting["mqttserver"].as<AutoConnectInput>();
  // The entered value is owned by AutoConnectAux of /mqtt_setting.
  // To retrieve the elements of /mqtt_setting, it is necessary to get
  // the AutoConnectAux object of /mqtt_setting.

  File param = SPIFFS.open("/time_settings.json", "w");
  setting.saveElement(param, {"ntpserver"});
  param.close();

  // Echo back saved parameters to AutoConnectAux page.
  AutoConnectText &echo = aux["parameters"].as<AutoConnectText>();
  echo.value = "ntpserver: " + ntpserver + "<br>";

  const long gmtOffset_sec = 3600 * 5.5;
  const int daylightOffset_sec = 0;

  configTime(gmtOffset_sec, daylightOffset_sec, ntpserver.c_str());
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    echo.value += "Failed to obtain time";
  }
  else
  {
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

    //"%04d-%02d-%02d %02d:%02d:%02d"
    echo.value = "Server time: <br>";
    char time_S[23];
    snprintf(time_S, 22, "%04d-%02d-%02d %02d:%02d:%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    echo.value += time_S;
    rtc.adjust(DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
  }

  return String("");
}

void handleRoot()
{

  //web page header
  String webpage_header =
      "<!DOCTYPE html>"
      "<html lang= \"en \">"
      "<head>"
      "  <title>Home - Muon Detector </title>"
      "  <meta charset= \"utf-8 \">"
      "  <meta name= \"viewport\" content= \"width=device-width, initial-scale=1.0\">"
      "  <link rel= \"stylesheet \" href= \"https://maxcdn.bootstrapcdn.com/bootstrap/3.4.0/css/bootstrap.min.css \">"
      "  <script src= \"https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js \"></script>"
      "  <script src= \"https://maxcdn.bootstrapcdn.com/bootstrap/3.4.0/js/bootstrap.min.js \"></script>"
      "  <script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>"
      "     <script type= \"text/javascript \">"
      "     setTimeout(\"location.reload()\", 3000);"
      "    </script>"

      "</head>"
      "<body>"
      ""
      "<div class= \"container \">"
      "  <h2><center>Muon Detector</center></h2>"
      "<div class= \"panel panel-default \">"
      "<div class= \"panel-body \">";

  String notification = "";

  if (clock_status == 2)
  {
    notification += webpage_button("GPS", "btn-success");
  }
  else if (clock_status == 1)
  {
    notification += webpage_button("RTC", "btn-info");
  }

  if (sd_status == 1)
  {
    notification += webpage_button("SD CARD", "btn-success");
  }
  else
  {
    notification += webpage_button("NO SD", "btn-danger");
  }

  if (wifi_status == 1)
  {
    notification += webpage_button("WIFI", "btn-success");
  }
  else
  {
    notification += webpage_button("NO WIFI", "btn-danger");
  }

  if (uploadOn == 1)
  {
    if (cloud_status == 1)
    {
      notification += webpage_button("MQTT", "btn-success");
    }
    else
    {
      notification += webpage_button("NO SERVER CONNECTION", "btn-warning");
    }
  }
  else
  {
    notification += webpage_button("UPLOAD DISABLED", "btn-warning");
  }

  if (sync_status == 2)
  {
    notification += webpage_button("SYNC OK", "btn-success");
  }
  else if (sync_status == 1)
  {
    notification += webpage_button("SYNCING", "btn-primary");
  }

  //HOME WEB PAGE. in this page count rate and status of the detector will be printed
  String content =
      "  </div>"
      "</div>"
      ""
      "<div class= \"panel panel-primary \">"
      "      <div class= \"panel-heading \"><center>Device information</center></div>"
      "      <div class= \"panel-body \">"
      "      Device ID : <span class= \"label label-primary \">@D1</span> <br>"
      "      Device Location : <span class= \"label label-primary \">@D2</span>"
      "      <br> <br>"
      "      Device time: <span class= \"label label-primary \">@D3</span>"
      "      "
      "      </div> </div>  "
      "      "
      "      "
      "       <div class= \"panel panel-info \">"
      "      <div class= \"panel-heading \"><center>Count</center></div>"
      "      <div class= \"panel-body \">"
      "     	<h3> Count per Minute: <span class= \"label label-info \">@C1</span> </h3>"
      "        <h4> Avarage count per minute: <span class= \"label label-info \">@C2</span> </h4>"
      "        <h4> Total count: <span class= \"label label-info \">@C3</span> </h4>"
      "<div id=\"barchart\"></div>"
      "      </div></div>"
      "      "
      "      <div class= \"panel panel-success \">"
      "      <div class= \"panel-heading \"><center>Last event</center></div>"
      "      <div class= \"panel-body \">"
      "      <h3> Avg Voltage (mV): <span class= \"label label-success \">@L1</span> </h3>"
      "      <h4> Voltages (mV): <span class= \"label label-success \">@L2</span> </h4>"
      "      "
      "      </div>"
      "    </div>"
      "    "
      "     <div class= \"panel panel-warning \">"
      "      <div class= \"panel-heading \"><center>Debug output</center></div>"
      "      <div class= \"panel-body \">"
      "      file_path:<BR><span >@X1</span> <BR><BR> "
      "      upload_path:<BR><span >@X2</span> <BR><BR> "
      "      last_upload_file_line:<span >@X3</span> <BR> "
      "      last_upload_file_position:<span >@X4</span> <BR> <BR>"
      "      GPS:<span >@X5</span> <BR> "
      "      "
      "      </div>"
      "    </div>"
      "      "
      "      "
      "</div>"
      
      "<p></p><p style= \"padding-top:15px;text-align:center \">" AUTOCONNECT_LINK(COG_24) "</p>"
      "</div><p><center><font color=\"blue\">Muon Detector<br> University of Colombo, Sri Lanka</center></font</p></div><br>[Sasanka-2019]"
      "</body>"
      "</html>";

  //REPAPCE PLACE HOLDERS with values 
  content.replace("@D1", String(DetectorID));
  content.replace("@D2", DetectorLocation);
  content.replace("@D3", timestr);


  float avg_cpm_d1=(float)count_D1/(float)minute_count;
  float avg_cpm_d2=(float)count_D2/(float)minute_count;
  float avg_cpm_coincident=(float)count_coincident/(float)minute_count;

  content.replace("@C1", String(cpm_coincident) + " / " + String(cpm_D1) + " / " +String(cpm_D2) );
  content.replace("@C2", String(avg_cpm_coincident) + " / " + String(avg_cpm_d1) + " / " +String(avg_cpm_d2) );
  content.replace("@C3", String(count_coincident) + " / " + String(count_D1) + " / " +String(count_D2));

  content.replace("@L1", String(avg_reading_mv));
  content.replace("@L2", String(raw_readings_ADC_mv[0]) + " , " + String(raw_readings_ADC_mv[1]) + " , " + String(raw_readings_ADC_mv[2]));
  content.replace("@X1", file_path);
  content.replace("@X2", upload_path);
  content.replace("@X3", String(last_upload_file_line));
  content.replace("@X4", String(last_upload_file_position));

   char gpsStr[100];
   sniprintf(gpsStr,100,"%s,%s,%s,  SAT:%d  FIX:%d <BR> AGE:%ds  %d", String(latitude,6), String(longitude,6), (String)altitude, satellites, fix_mode, gps_age/1000,cn3);
  Serial.print(gpsStr);
  content.replace("@X5", (String)gpsStr);

  // Serial.printf("time u1 : %d ms \n", millis() - ttt);
  WiFiWebServer &webServer = portal.host();
  webServer.send(200, "text/html", webpage_header + notification + content);

  // Serial.printf("time u2 : %d ms \n", millis() - ttt);
}

String webpage_button(String name, String style)
{

  return "<button type= \"button \" class= \"btn " + style + " \">" + name + "</button>";
}