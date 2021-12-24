#include <PubSubClient.h>
#include <WiFi.h>
#include <FS.h>
#include <SD.h>
#include "Server_upload.h"

/*
  NOTE:
    File waiting to upload named as <filename>.x.csv
    files that are fully uploaded to the sever will remame to <filename>.c.csv from this code
*/

///////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
/////////////////////////////////////////////////upload\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
///////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//


//call this function to upload data
void upload_file()
{
  upload_path[0] = 0;
  //find next or pending file to be uploaded
  if (select_uplaod_file(upload_path, false) == 1)
  {
    //read one line from the file and upload

    file_read_line_and_upload();
    //Serial.print("xxxx ");
    //Serial.println(upload_path);
  }
  else
  {
    // if "select_uplaod_file(upload_path, false)" returns false, that means all files are uploaded expect current file
    // run  "select_uplaod_file(upload_path, true);" to select current file as the uplaod_path

    //Serial.println("U :> all okey other than current file");
    select_uplaod_file(upload_path, true);
    file_read_line_and_upload();
    
    //Serial.print("YYYY ");
    //Serial.println(upload_path);
  }
}


//this fucntion will upload "data" string to the server via MQTT
boolean upload_data(String data)
{
  Serial.print("U :> file read line : ");
  Serial.println(data);
  //covert string to a char array
  char chr_data[50];
  data.toCharArray(chr_data, 49);

  //chceck if wifi connected
  if (WiFi.status() == WL_CONNECTED)
  {
    wifi_status = 1;
    //check if MQTT server is Not conncetd, then try reconnect
    if (!client.connected())
    {
      reconnect(2);
      cloud_status = 0;
    }

    //check if MQTT server is conncetd, 
    if (client.connected())
    {
      cloud_status = 1;
      //upload data
      client.loop();
      Serial.print("U :> Publishing data: ");
      if (client.publish("sasa1", chr_data))
      {
        //upload successfull, return True
        Serial.print("U :> done \t");
        return true;
      }
      else
      {
        //upload failed 
        Serial.println("U :> Publish failed");
        cloud_status = 0;
      }
    }
  }
  else
  { 
    //no wfi connection 
    Serial.println("U :> WIFI connection lost");
    cloud_status = 0;
    wifi_status = 0;
  }

  return false;
}
///////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////SD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//This function will read the "upload file" and upload one line to the server
int file_read_line_and_upload()
{
  //Serial.printf("Reading file: %s\n", path);

  //try to open the "upload file"
  File fileUpload = SD.open(upload_path);
  if (!fileUpload)
  {
    Serial.println("U :> Failed to open file for reading");
    sd_status = 0;
    return 0;
  }

  String line_buffer;

  Serial.print("U :> Read from file: ");
  Serial.print(upload_path);
  Serial.print("\t");

  //seek to last last_upload_file_position and read from there
  fileUpload.seek(last_upload_file_position);
  //after seeking, if more data avalable, Then we have data that havent uploaded
  if (fileUpload.available())
  {
    line_buffer = fileUpload.readStringUntil('\n');       //read one line
    int tmp_upload_file_position = fileUpload.position(); //temporary set the new postion 
    fileUpload.close();
    if (line_buffer.length() > 5) //if we have more than 5 charators havent uploaded, then possibaly new data
    {
      //upload this line with "upload_data" function
      int upload_complete = upload_data(line_buffer);
      
      //function will return true if successfully uploaded to the server
      if (upload_complete == true)
      {
        sync_status = 1;  //we are uploading data, set sync satus
        last_upload_file_position = tmp_upload_file_position; //update last upload position
        //and get the last uploaded line number of the file
        //line number is the section before first ',' 
        last_upload_file_line = line_buffer.substring(0, line_buffer.indexOf(',')).toInt(); 
        //Serial.printf("Writing file: %s\n", UPLOAD_STATE_FILE);

        //write last upload infomation to the SD card. (this will write UPLOAD_STATUS_FILE = /last_upload_state.dat" file)
        //in case of a power failure this will retrain in the SD card and
        //can resume from the last postion
        File file3 = SD.open(UPLOAD_STATUS_FILE, FILE_WRITE);
        if (file3)
        {
          /*
            example data in the UPLOAD_STATUS_FILE file:
            
            Last upload file : DATA-2020-06-23/DATA-2020-06-23_01:12:00.x.csv
            Last upload file postion : 230
            Last upload file Line : 30
            ---DO NOT MODIFIY---"
          */

          //write these infomation to UPLOAD_STATUS_FILE
          file3.print(F("Last upload file :"));
          file3.println(upload_path);
          file3.print(F("Last upload file postion :"));
          file3.println(last_upload_file_position);
          file3.print(F("Last upload file Line :"));
          file3.println(last_upload_file_line);
          file3.println(F("DO NOT MODIFIY"));
          file3.println(F("---DO NOT MODIFIY---"));
          file3.close();
          sd_status = 1;
        }
        else
        {
          Serial.println("U :> failed to open UPLOAD_STATUS_FILE");
          sd_status = 0;
        }
      }
      else
      {
        sync_status = 0;
        upload_failed_count++;
      }

      return 0;
    }
    fileUpload.close();
  }

  //This will indentify if the file has uploaded all the data to the server
  //first need to check this is not the current file
  //if this is current file, new data may append to this file
  if ((strcmp(upload_path, file_path) != 0)) 
  { 
    //if some other file
    Serial.print("U :> all done for file :> ");
    Serial.println(upload_path);
    fileUpload.close();
    char new_fileName[50];
    String Str_upload_path = String(upload_path);
    //if all data are uploaded, change the file name to .c.csv form .x.csv
    Str_upload_path.replace(".x", ".c");
    Str_upload_path.toCharArray(new_fileName, 49);
  
    Serial.printf("U :> Renaming file %s to %s\n", upload_path, new_fileName);
    if (SD.rename(upload_path, new_fileName))
    {
      Serial.println("U :> File renamed");
      Serial.printf("U :> Deleting file: %s\n", UPLOAD_STATUS_FILE);
      //remove UPLOAD_STATUS_FILE file.
      //in the next loop, this will replace by the next file 
      if (SD.remove(UPLOAD_STATUS_FILE))
      {
        //Serial.println("File deleted");
      }
      else
      {
        Serial.println("U :> Delete failed");
      }
      last_upload_file_position = 0;
      last_upload_file_line = 0;
      upload_path[0] = 0;
    }
    else
    {
      Serial.println("U :> Rename failed");
    }
  }
  else
  {
    //if all are uploaded and, last upalod file matches with the current file,
    //all data up to this point are uploaded
    Serial.println("U :> All uploaded and uptodate");
    sync_status = 2;
  }

  return 0;
}

//call this function in the setup section to get the last uploaded file, postion and line
void load_last_upload_state()
{
  //select the next upload file, (false = except current file)
  select_uplaod_file(upload_path, false);
  Serial.printf("U :> Reading file: %s\n", UPLOAD_STATUS_FILE);

  //open UPLOAD_STATUS_FILE file and retraive last uploaded postion and line
  //example data in the UPLOAD_STATUS_FILE file-
  /*    
    Last upload file : DATA-2020-06-23/DATA-2020-06-23_01:12:00.x.csv
    Last upload file postion : 230
    Last upload file Line : 30
    ---DO NOT MODIFIY---"
  */

  File file_state = SD.open(UPLOAD_STATUS_FILE);
  if (!file_state)
  {
    Serial.println("U :> Failed to open file for reading");
    last_upload_file_position = 0;
    last_upload_file_line = 0;
    return;
  }
  String line_buffer2;
  if (file_state.available())
  {
    line_buffer2 = file_state.readStringUntil('\n'); //read first line to get upload_path
    line_buffer2 = line_buffer2.substring(line_buffer2.indexOf(':') + 1); //find postion until ":"
    if (line_buffer2.indexOf('\r') > 0)   //find the end of the string
      line_buffer2 = line_buffer2.substring(0, line_buffer2.indexOf('\r')); //take every thing in between
    char temp_arr[50];
    line_buffer2.toCharArray(temp_arr, 49); //covert this to a char array
    if (strcmp(temp_arr, upload_path) == 0) //compare with current last upload
    {
      Serial.println("U :> Last upload matched");
    }
    else
    {
      Serial.println("U :> ERROR: Last upload DONT matched.");
      Serial.print("U :> on file :>");
      Serial.print(temp_arr);
      Serial.print("  next upload :>");
      Serial.println(upload_path);
      return;
    }
  }
  line_buffer2 = "";
  if (file_state.available())
  {
    line_buffer2 = file_state.readStringUntil('\n');  //read next line to get last_upload_file_position
    line_buffer2 = line_buffer2.substring(line_buffer2.indexOf(':') + 1);
    last_upload_file_position = line_buffer2.toInt();
  }

  line_buffer2 = "";
  if (file_state.available())
  {
    line_buffer2 = file_state.readStringUntil('\n'); //read next line to get last_upload_file_line
    line_buffer2 = line_buffer2.substring(line_buffer2.indexOf(':') + 1);
    last_upload_file_line = line_buffer2.toInt();
  }
  //debug prints
  Serial.print(F("U :> Last upload file :"));
  Serial.println(upload_path);
  Serial.print(F("U :> Last upload file postion :"));
  Serial.println(last_upload_file_position);
  Serial.print(F("U :> Last upload file Line :"));
  Serial.println(last_upload_file_line);

  file_state.close();
}

//this function will select the next file to upload into the server
//filepath_select = search folder
//true = return current file (file that currently writing data into)
//false = next folder except current file
char select_uplaod_file(char *filepath_select, boolean current_file)
{

  File root;
  root = SD.open("/");

  //this while loop will run thru call the subfolder and files in the given path.
  //can go maximum of two levels deep in the folder stucture
  while (true)
  {

    File entry = root.openNextFile();
    if (!entry)
    {
      // no more files
      break;
    }

    if (entry.isDirectory())
    {

      //Serial.printf("dir found  : %s \n", entry.name());

      if (strncmp(entry.name(), "/DATA", 5) == 0)
      {

        while (true)
        {

          File entry2 = entry.openNextFile();
          if (!entry2)
          {
            // no more files in this directoty
            break;
          }

          if (entry2.isDirectory())
          {
          }
          else
          {
            // if (strncmp(entry2.name(), "/DATA", 5) == 0)

            String pathname2 = String(entry2.name());
            if (pathname2.startsWith("/DATA"))
            {
              //find whether end with .x.csv 
              //if not file is alredy upload file (c.csv)
              if (pathname2.charAt(pathname2.indexOf('.') + 1) == 'x')
              {
                if (current_file == true)
                {
                  strncpy(filepath_select, entry2.name(), 49);
                  //Serial.println(entry2.name());
                  entry.close();
                  entry2.close();
                  return 1;
                }
                else
                {

                  if (strcmp(entry2.name(), file_path) != 0)
                  {
                    strncpy(filepath_select, entry2.name(), 49);
                    //Serial.println(entry2.name());
                    entry.close();
                    entry2.close();
                    return 1;
                  }
                }
              }
            }
          }
          entry2.close();
        }
      }
    }
    entry.close();
  }

  return 0;
}

///////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
/////////////////////////////////////////////////MQTT\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
///////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//

//connect/reconnect to mqtt server
//retry = number of retrys before give up
void reconnect(int retry)
{
  // Loop until we're reconnected
  int cnt = 0;

  //while connection establish 
  while (!client.connected())
  {
    cloud_status = 0;
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect

    if (client.connect(mqttId.c_str(), mqttUsername.c_str(), mqttPassword.c_str())) //connnect with user name and password
    { //function will return true, if connected
      Serial.println("MQTT connected");
      cloud_status = 1;
    }
    else
    {
      //some debug prints
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.print(" MQTT will try again in 1 seconds");
      Serial.print(". retry ");
      Serial.print(cnt);
      Serial.print(" of ");
      Serial.println(retry);
      delay(1000);
      cnt++;
      if (cnt > retry) //if exeeds number of retrys
      {
        Serial.println("MQTT gave up");
        break;
      }
    }
  }
}
