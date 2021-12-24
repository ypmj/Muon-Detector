
#ifndef _SDCARD_H_
#define _SDCARD_H_

//function prototype 
char init_SDcard();
char makeFile(fs::FS &fs, const char *path, const char *message);
char appendFile(fs::FS &fs, const char *path, const char *message);
void readFile(fs::FS &fs, const char * path);
void updateFile(fs::FS &fs, const char * path);
char select_uplaod_file(char *filepath, boolean current_file);

//import variables from the main_scr
extern boolean sd_status;

#endif