#include <Arduino.h>
#include "Buzzer.h"
#include "hardwareConfig.h"

/*
    THIS IS FOR PASSIVE BUZZER.   
*/

//initialize buzzer (initialize PWM)
void init_buzzer()
{
  ledcSetup(0, 2000, 8); //channel,frequancy,resolution
  ledcAttachPin(BUZZER, 0);  //pin,channel (we will be using PWM channel 0 for the buzzer)
}

//produce a beep sound with given frequacy and volume
//freq = buzzer sound frequancy
//volume = volume
// NOTE: This is a Non-blocking code.
void buzzer(int freq, int volume)
{
  ledcWriteTone(0, freq); //channel,frequancy
  ledcWrite(0, volume);   //channel,duty
}

//short beep sound at 4 khz for 250 ms
//volumn can be set by the portal
// NOTE: This is a blocking code. takes 500 ms to execute
void buzzer_short_beep()
{
  buzzer(4000, buzzer_volume1);
  delay(250);
  buzzer(4000, 0);
  delay(250);
}

//Long beep sound at 4 khsz for 660 ms
// NOTE: This is a blocking code. takes 1200 ms to execute
void buzzer_long_beep()
{
  buzzer(4000, buzzer_volume1);
  delay(600);
  buzzer(4000, 0);
  delay(600);
}

//custom duration beep sound
// NOTE: This is a blocking code. takes duration*2 to execute
void buzzer_beep(int frequency, int duration)
{
  buzzer(frequency, buzzer_volume1);
  delay(duration);
  buzzer(frequency, 0);
  delay(duration);
}