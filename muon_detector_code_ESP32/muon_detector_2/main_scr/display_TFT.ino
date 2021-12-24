#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "iconset_16x16.h"
#include <display_TFT.h>
#include <Buzzer.h>
#include "hardwareConfig.h"
#include "display_TFT.h"

//initialize display
void init_display_TFT()
{
  tft.initR(INITR_BLACKTAB);             //type of the display
  pinMode(DISPLAY5100_BACKLIGHT, OUTPUT);//backlight pin as output
  display_backlight_TFT(HIGH);           //blink backlight
  delay(500);
  display_backlight_TFT(LOW);
  delay(500);
  display_backlight_TFT(HIGH);
  tft.fillScreen(ST77XX_BLACK);         //clear screen
}


//print count and other infomations
void update_display_TFT(){
    tft.setTextWrap(false);
  
    update_icons_TFT();

    //DUAL mode
    tft.drawRoundRect(0, 20, 127, 55, 3, ST77XX_BLUE);
    tft.setCursor(0, 23);
    tft.setTextColor(ST77XX_GREEN,ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setCursor(1,tft.getCursorY());
    tft.print(" Coincendent cnt:");
    tft.println(count_coincident);
    tft.setTextColor(ST77XX_WHITE,ST77XX_BLACK);
    tft.println();
    tft.setTextSize(2);
    tft.setCursor(1,tft.getCursorY());
    tft.println("  AVG CPH");
    tft.setCursor(1,tft.getCursorY());
    tft.print("   ");
    float avg_c=(float)count_coincident/(float)minute_count*60;
    tft.print(avg_c);
    if (avg_c>10) tft.println();
    else tft.println(" ");
    tft.setTextSize(1);

    tft.drawRoundRect(0, 76, 127, 50, 3, ST77XX_BLUE);
    tft.drawFastVLine(63, 76, 50, ST77XX_BLUE);
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_CYAN,ST77XX_BLACK);
    tft.setCursor(1, 81);
    tft.print("   DET 1");
    tft.setCursor(64, 81);
    tft.print("   DET 2");
    tft.setTextColor(ST77XX_ORANGE,ST77XX_BLACK);

    tft.setCursor(1, 91);
    tft.setCursor(1, tft.getCursorY());
    tft.print(" Tot:");
    tft.print(count_D1);
    tft.setCursor(64, tft.getCursorY());
    tft.print(" Tot:");
    tft.println(count_D2);
    tft.setCursor(1, tft.getCursorY());
    tft.print("  CPM:");
    tft.print(cpm_D1); if(cpm_D1<10) tft.print(" ");
    tft.setCursor(64, tft.getCursorY());
    tft.print("  CPM:");
    tft.print(cpm_D2);if(cpm_D2<10) tft.print(" ");
    tft.println();
    tft.setCursor(1, tft.getCursorY());

    tft.print(" AVG:");
    float avg_d1=(float)count_D1/(float)minute_count;
    tft.print(avg_d1,2);if(avg_d1<10) tft.print(" ");
    tft.setCursor(64, tft.getCursorY());
    tft.print(" AVG:");
    float avg_d2=(float)count_D2/(float)minute_count;
    tft.print(avg_d2,2);if(avg_d2<10) tft.print(" ");
    tft.println();
    tft.setCursor(1, tft.getCursorY());
    tft.print("  ");
    int pos1 = count_D1 % 10;
    tft.print(reading_mv_D1[pos1],0);
    tft.print("mv"); if(reading_mv_D1[pos1]<1000) tft.print(" ");
    tft.setCursor(64, tft.getCursorY());
    tft.print("  ");
    int pos2 = count_D2 % 10;
    tft.print(reading_mv_D2[pos2],0);
    tft.print("mv"); if(reading_mv_D2[pos2]<1000) tft.print(" ");
    tft.println();
  
    

  if(gpsTimeOn==true){
    tft.setTextSize(1);
    tft.setCursor(0, 129);
    tft.setTextColor(ST77XX_MAGENTA,ST77XX_BLACK);
    tft.println(DetectorLocation);
    tft.print("IP: ");
    tft.println(ip);
    tft.setTextColor(0x8410,ST77XX_BLACK);
    tft.print("GPS: ");
    tft.print(latitude,4);
    tft.print(",");
    tft.println(longitude,4);
    tft.print("ALT:");
    tft.print(altitude,1);
    tft.print(" S:");
    tft.print(satellites);
    tft.print(" ");
    tft.print(fix_mode);
    tft.print("D T:");
    tft.println(gps_age);
  }else{
    tft.setTextSize(1);
    tft.setCursor(0, 129);
    tft.setTextColor(ST77XX_MAGENTA,ST77XX_BLACK);
    tft.println();
    tft.println();
    tft.println(DetectorLocation);
    tft.print("IP: ");
    tft.println(ip);
  }

}

//display icors
void display_icons_TFT()
{

  if (clock_status == 2)
  {
    tft.drawBitmap(17 * 0, 0, icon_blank, 16, 16, ST77XX_BLACK);
    tft.drawBitmap(17 * 0, 0, icon_gps, 16, 16, ST77XX_YELLOW);
  }
  else if (clock_status == 1)
  {
    tft.drawBitmap(17 * 0, 0, icon_clock, 16, 16, ST77XX_YELLOW);
  }
  else
  {
    tft.drawBitmap(17 * 0, 0, icon_blank, 16, 16, ST77XX_BLACK);
  }

  if (sd_status == 1)
  {
    tft.drawBitmap(17 * 1, 0, icon_sdCard, 16, 16, ST77XX_YELLOW);
  }
  else
  {
    tft.drawBitmap(17 * 1, 0, icon_blank, 16, 16, ST77XX_BLACK);
  }

  if (wifi_status == 1)
  {
    tft.drawBitmap(17 * 2, 0, icon_wifi3, 16, 16, ST77XX_YELLOW);
  }
  else
  {
    tft.drawBitmap(17 * 2, 0, icon_blank, 16, 16, ST77XX_BLACK);
  }

  if (cloud_status == 1)
  {
    tft.drawBitmap(17 * 3, 0, icon_cloud, 16, 16, ST77XX_YELLOW);
  }
  else
  {
    tft.drawBitmap(17 * 3, 0, icon_blank, 16, 16, ST77XX_BLACK);
  }


  if (sync_status == 2)
  {
    tft.drawBitmap(17 * 4, 0, icon_blank, 16, 16, ST77XX_BLACK);
    tft.drawBitmap(17 * 4, 0, icon_syncDone, 16, 16, ST77XX_YELLOW);
  }
  else if (sync_status == 1)
  {
    tft.drawBitmap(17 * 4, 0, icon_blank, 16, 16, ST77XX_BLACK);
    tft.drawBitmap(17 * 4, 0, icon_sync, 16, 16, ST77XX_YELLOW);
  }
  else
  {
    if (uploadOn == true){
      tft.drawBitmap(17 * 4, 0, icon_blank, 16, 16, ST77XX_BLACK);
      tft.drawBitmap(17 * 4, 0, icon_sync_fail, 16, 16, ST77XX_YELLOW);
    } else {
      tft.drawBitmap(17 * 4, 0, icon_blank, 16, 16, ST77XX_BLACK);
    }
  }

  //tft.display();
}

//this function is faster than display icons
//this only display icons only if chnange detected 
void update_icons_TFT()
{
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_YELLOW,ST77XX_BLACK);
    tft.setCursor(97, 0);
    if(now.hour()<10) tft.print("0");
    tft.print( now.hour());
    tft.print(":");
    if(now.minute()<10) tft.print("0");
    tft.print(now.minute());
    tft.setCursor(109, 8);
    tft.print(":");
    if(now.second()<10) tft.print("0");
    tft.print( now.second());
    tft.setTextWrap(false);

  if (
      (prv_wifi_status != wifi_status) ||
      (prv_sync_status != sync_status) ||
      (prv_clock_status != clock_status) ||
      (prv_sd_status != sd_status) ||
      (prv_cloud_status != cloud_status))
  {
    display_icons_TFT();

    prv_wifi_status = wifi_status;
    prv_sync_status = sync_status;
    prv_clock_status = clock_status;
    prv_sd_status = sd_status;
    prv_cloud_status = cloud_status;
  }
}

//turn on/off backlight
void display_backlight_TFT(boolean sts)
{
  digitalWrite(DISPLAY5100_BACKLIGHT, sts);
}

//clear display by filling entire display with black
void display_clear_TFT(){
  tft.fillScreen(ST77XX_BLACK);
}
