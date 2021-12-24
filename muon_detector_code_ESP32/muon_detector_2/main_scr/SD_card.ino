#include <Arduino.h>
#include "Serial_print.h"
#include "hardwareConfig.h"
#include <SPI.h>
#include <SD.h>
#include "display_TFT.h"
#include "display_OLED.h"

//This function will append string to the end of the file
//  fs::FS &fs => SD card object
//  *path => path of the file including file name and extension
//  *message => string to append on to the file
char appendFile(fs::FS &fs, const char *path, const char *message)
{
  //Serial.printf("Appending to file: %s\n", path);
  char status = 0;
  File file = fs.open(path, FILE_APPEND);
  if (file)   //make sure file oped successfully 
  {
    if (file.print(message))
    {   
        //if append id successful
        //Serial.println("Message appended");
        status = 1;
        file.close(); //close if done
        sd_status=1;
        return 1;
    }
    else
    {   
        //if cant append, may be something wrong with the SD card
        Serial.println("can not append");
        file.close();
        sd_status=0;  //set status to SD card error
        return 0;
    }
   
  }
    //if cant opne the file
    Serial.println("can open file");
    sd_status=0;
    return 0;
}
 
//This function will create a new file and add text to new file
//  fs::FS &fs => SD card object
//  *path => path of the file including (new) file name and extension
//  *message => string to add on to the file after creation
char makeFile(fs::FS &fs, const char *path, const char *message)
{

  //Serial.printf("Appending to file: %s\n", path);
  char status = 0;
  File file = fs.open(path, FILE_APPEND);
  
  //check whether already there is a file with same name. If present, append the message to that file
  if (file)
  { 
    if (file.print(message))
    {
      Serial.println("file alreday exixting. Data added to file");
      status = 1;
    }
    else
    {
      status = 0;
    }
    file.close();
 }

  //exit from the function if successfull
  if (status==1){
    sd_status=1;
    return 1;
  }
  
  //can`t open file. So now file with that name
  //lets create file
  //but first make sure a folder matching to the path alreday in the SD card
  if (!fs.exists(path))
  { 
    //if not create a folder
    Serial.print("Path does not exist: ");
    Serial.println(path);
    String str_path = String(path);
    int folder_name_index = str_path.indexOf('/', 1);

    //code will go inside to the parent folder check wether it is present
    if (folder_name_index != -1)
    { //inside a folder
      char folder_name[50];
      str_path.substring(0, folder_name_index).toCharArray(folder_name, 50);
      if (!fs.exists(folder_name))
      {
        //if not creat a folder
        Serial.print("Creating Dir: ");
        Serial.println(folder_name);
        if (fs.mkdir(folder_name))
        {
          Serial.println("Dir created");
        }
        else
        {
          Serial.println("mkdir failed");
          sd_status=0;
          return 0;
        }
      }
    }

    //folder structure is now ok
    //create a new file
    Serial.print("Creating file: ");
    Serial.println(path);
    File file_new = fs.open(path, FILE_WRITE);
    if (!file_new)
    {
      Serial.println("Failed to Create file");
      sd_status=0;
      return 0;
    }
    else
    {
      Serial.println("File Created");
    }

    file_new.close();
  }
  else
  {
    Serial.println("File exisits but failed to open file for appending. ");
    sd_status=0;
    return 0;
  }

    //add the message to the newly created file
    File file2 = fs.open(path, FILE_APPEND);
    if (file2)
    {
      char status = 0;
      if (file2.print(message))
      {
        Serial.println("Data added to file");
        status = 1;
      }
      else
      {
        Serial.println("cant add data");
        status = 0;
      }
      file2.close();
      sd_status=status;
      return status;
    }
    else
    {
      Serial.println("Cannot open file");
    }

}
 


//initialize the SD card
char init_SDcard()
{

  char buffer[100];
  serial_print_stars(60);
  Serial.println(F("Initializing SD card..."));

  if (!SD.begin(SD_CS))
    {
      sd_status=0;
      Serial.println(F("###ERROR### SD initialization failed!"));

      display_printError("No SD Card",10000);
      return(0);
    }

   

    Serial.println(F("initialization done."));
    sd_status=1;
    display_icons_OLED();
    display_icons_TFT();
    
    //determine SD card type
    uint8_t cardType = SD.cardType();

    Serial.print(F("SD Card Type: "));

    if (cardType == CARD_NONE)
    {
        Serial.println(F("No SD card attached"));
    }
    else if (cardType == CARD_MMC)
    {
        Serial.println(F("MMC"));
    }
    else if (cardType == CARD_SD)
    {
        Serial.println(F("SDSC"));
    }
    else if (cardType == CARD_SDHC)
    {
        Serial.println(F("SDHC"));
    }
    else
    {
        Serial.println(F("UNKNOWN"));
    }
     delay(10);

    //print sd card size
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    snprintf(buffer,100,"SD Card Size: %lluMB\n", cardSize );
    Serial.print(buffer);

    //print used space
    snprintf(buffer,100,"Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.print(buffer);
    snprintf(buffer,100,"Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024) );
    Serial.print(buffer);
    delay(10);

    //list all the files inside the SD card
    //list_files();

    delay(10);
    serial_print_stars(60);

    return(1);
}


//list all the files inside the SD card 
void list_files()
{
    File root;
    root = SD.open("/");
    Serial.println(F("___________File structure___________"));
    printDirectory(root, 0);
    serial_print_dashes(30);
}

//this fucntion will list files inside a folder
void printDirectory(File dir, int numTabs)
{

    while (true)
    {
        File entry = dir.openNextFile();
        if (!entry)
        {
            // no more files
            break;
        }
        for (uint8_t i = 0; i < numTabs; i++)
        {
            //Serial.print('\t');
        }

        if (entry.isDirectory())
        {
            //Serial.print(entry.name());
            Serial.println(" ");
            //move inside to the folder and list files inside that folder
            //this is a recusive function, hence this will print all flies in all available folders
            printDirectory(entry, numTabs + 1);
        }
        else
        {
           
            Serial.print(entry.name());
            // files have sizes, directories do not
            Serial.print("\t\t");
            unsigned long s = entry.size();
            Serial_print_file_size(s);
        }
        entry.close();
    }
}


void Serial_print_file_size(unsigned long s)
{
  char buffer[50];
    if (s >= 1000000)
    {

        snprintf( buffer,50,"%f",s / 1000.00 / 1000.00 );
        Serial.print(buffer);
        Serial.print(" ");
         snprintf( buffer,50,"%.3f",((float)(s % (1000000))) / 1000.00);
        Serial.print(buffer);
    }
    else
    {   
        snprintf( buffer,50,"%.3f",(float)s / 1000.00);
        Serial.print(buffer);
    }

    Serial.println(" KB");
}



