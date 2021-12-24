#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "iconset_16x16.h"
#include <display_OLED.h>
#include <Buzzer.h>
#include "display_OLED.h"
#include "hardwareConfig.h"
#include "display_TFT.h"


//initialize display
void init_display_OLED()
{
  //begin display with internal VCC 
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS))
  { // initialize the LCD display
    Serial.println(F("SSD1306 allocation failed"));
  }

  display.display();
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.display();
}

//print error msgs
void display_printError(const char *msg1)
{
  display_printError(msg1,"",0);
}

void display_printError(const char *msg1,int timeOut)
{
  display_printError(msg1,"",timeOut);
}

void display_printError(const char *msg1,const char *msg2,int timeOut)
{ 
  buzzer_beep(4000,150);
  buzzer_beep(2500,250);

  display_clear_TFT();
  tft.setCursor(0,0);
  tft.setTextSize(2);
  tft.println();
  tft.setTextColor(ST77XX_RED);
  tft.println("  ERROR ! ");
  tft.setTextColor(ST77XX_ORANGE);
  tft.println(msg1);
  tft.println(msg2);



  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(BLACK, WHITE); // Draw 'inverse' text
  display.setTextSize(2);
  display.println();
  display.println("  ERROR ! ");
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println();
  display.println(msg1);
  display.println(msg2);
  display.display();

  delay(timeOut);

}

void display_printError_OLED_ONLY(const char *msg1,const char *msg2,int timeOut)
{ 
  buzzer_beep(4000,150);
  buzzer_beep(2500,250);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(BLACK, WHITE); // Draw 'inverse' text
  display.setTextSize(2);
  display.println();
  display.println("  ERROR ! ");
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println();
  display.println(msg1);
  display.println(msg2);
  display.display();

  delay(timeOut);

}


//show counts, mV and other information on the OLED
void update_display_OLED(){

    display.fillRect(0, 16, 127, 47, BLACK);
    display.setTextColor(WHITE, BLACK);
    display.setTextSize(1);
    char count_str[10];
    snprintf(count_str, 9, "%d", count_coincident);
    display.setCursor(127 - strlen(count_str) * 6, 0);
    display.print(count_coincident);
    display.setTextColor(WHITE);
    display.setCursor(0, 16);
    display.setTextSize(2);
    display.print("  ");
    display.print(avg_reading_mv, 0);
    display.println(" mV");
    display.print("  CPH - ");
    display.println((float)count_coincident/(float)minute_count*60);
    display.setTextSize(1);
    display.print("    ");
    display.println(" ");
    display.print(" ");
    display.print(timestr);
    display.display();
}

//display icons accoding to the status
void display_icons_OLED()
{

  if (clock_status == 2)
  {
    display.drawBitmap(17 * 0, 0, icon_blank, 16, 16, 0);
    display.drawBitmap(17 * 0, 0, icon_gps, 16, 16, 1);
  }
  else if (clock_status == 1)
  {
    display.drawBitmap(17 * 0, 0, icon_clock, 16, 16, 1);
  }
  else
  {
    display.drawBitmap(17 * 0, 0, icon_blank, 16, 16, 0);
  }

  if (sd_status == 1)
  {
    display.drawBitmap(17 * 1, 0, icon_sdCard, 16, 16, 1);
  }
  else
  {
    display.drawBitmap(17 * 1, 0, icon_blank, 16, 16, 0);
  }

  if (wifi_status == 1)
  {
    display.drawBitmap(17 * 2, 0, icon_wifi3, 16, 16, 1);
  }
  else
  {
    display.drawBitmap(17 * 2, 0, icon_blank, 16, 16, 0);
  }

  if (cloud_status == 1)
  {
    display.drawBitmap(17 * 3, 0, icon_cloud, 16, 16, 1);
  }
  else
  {
    display.drawBitmap(17 * 3, 0, icon_blank, 16, 16, 0);
  }

  if (sync_status == 2)
  {
    display.drawBitmap(17 * 4, 0, icon_blank, 16, 16, 0);
    display.drawBitmap(17 * 4, 0, icon_syncDone, 16, 16, 1);
  }
  else if (sync_status == 1)
  {
    display.drawBitmap(17 * 4, 0, icon_blank, 16, 16, 0);
    display.drawBitmap(17 * 4, 0, icon_sync, 16, 16, 1);
  }
  else
  {
    display.drawBitmap(17 * 4, 0, icon_blank, 16, 16, 0);
  }

  display.display();
}

//this function is faster than display icons
//this only display icons only if chnange detected 
void update_icons_OLED()
{
  if (
      (prv_wifi_status != wifi_status) ||
      (prv_sync_status != sync_status) ||
      (prv_clock_status != clock_status) ||
      (prv_sd_status != sd_status) ||
      (prv_cloud_status != cloud_status))
  {
    display_icons_OLED();

    prv_wifi_status = wifi_status;
    prv_sync_status = sync_status;
    prv_clock_status = clock_status;
    prv_sd_status = sd_status;
    prv_cloud_status = cloud_status;
  }
}