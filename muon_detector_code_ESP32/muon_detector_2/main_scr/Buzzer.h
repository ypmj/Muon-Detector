#ifndef _BUZZER_H_
#define _BUZZER_H_

//function prototype 
void init_buzzer();
void buzzer(int freq, int volume);
void buzzer_short_beep();
void buzzer_long_beep();
void buzzer_beep(int frequency, int duration);

//import variables from the main_scr
extern int buzzer_volume1; //out of 255   //Buzzer volume (when detect a event)
extern int buzzer_volume2;  //out of 255   //Buzzer volume (General beeps, warnings, ect)

#endif