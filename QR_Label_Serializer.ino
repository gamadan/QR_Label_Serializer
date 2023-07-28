/*
 * how to get TFT_eSPI (by BODMER) working:
 * 
 * The library
 */


/*  
 Test the tft.print() viz embedded tft.write() function

 This sketch used font 2, 4, 7

 Make sure all the display driver and pin connections are correct by
 editing the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################
 */

 /* 
 * v1.0.1 - Added 'RF12V-1PRT-2PRD-BT'
 */
#include <iostream>
#include <vector>
#include <HardwareSerial.h>
#include "FS.h"
#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>
#include "Def.h"
#include <EEPROM.h>

#include "ServerCallbacks.h"
#include "Unit_CharCallbacks.h"
#include "Channel_CharCallbacks.h"
#include "AddUnit_CharCallbacks.h"
#include "RemoveUnit_CharCallbacks.h"

BLECharacteristic *unitCharacteristic;
BLECharacteristic *channelCharacteristic;

BLECharacteristic *addUnitCharacteristic;
BLECharacteristic *removeUnitCharacteristic;

void app_task(void * parameters);
void ble_task(void * parameters);

TaskHandle_t app_handle;
TaskHandle_t ble_handle;

std::string rxUnitMessage = "";
std::string rxChannelMessage = "";
std::string rxAddUnitMessage = "";
std::string rxRemoveUnitMessage = "";


/*
 * 
 * v2.0.0 - Added BLE characteristics to print from BLE dev app
 * 
 * v2.0.1 - Added task for application logic
 * 
 */


 
#define VERSION "2.0.1"

#define EEPROM_SIZE        512

// This is the file name used to store the touch coordinate
// calibration data. Change the name to start a new calibration.
#define CALIBRATION_FILE "/TouchCalData3"

#define BUTTON_DELAY    25

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
bool REPEAT_CAL = false;

#define TFT_GREY 0x5AEB // New colour

#define MAX_CHAR_IN_SET   36

TFT_eSPI tft = TFT_eSPI();  // Invoke library
uint16_t x, y;

uint8_t unitIndex;

char possibleChannelCharacters[36];

HardwareSerial uart(2);  //if using UART2


uint8_t channel_char_index[] = {0, 0, 0, 0};

char channel[4];

std::string unitListPath = "/unitList.txt";


void startAppTask() {
  xTaskCreatePinnedToCore(app_task,
                          "Application Logic Task",
                          1024*4,
                          NULL,
                          6,
                          &app_handle,
                          1);
}

void startBLETask() {
  xTaskCreatePinnedToCore(ble_task,
                          "BLE Task",
                          1024*5,
                          NULL,
                          7,
                          &ble_handle,
                          1);
}


bool doesUnitExistInList(std::string unitName) {
  for(int i = 0; i < units.size(); i++) {
    if(units.at(i).compare(unitName) == 0) return true;
  }
  return false;
}


void deleteFile(const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(SPIFFS.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}

void writeFile(const char * path, const char * message){
    Serial.printf("Writing file: %s ", path);

    File file = SPIFFS.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}
std::string readFile(const char * path){
    std::string output = "";
    Serial.printf("Reading file: %s\r\n", path);
    
    File file = SPIFFS.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return output;
    }
    
  
    Serial.print("File size:");Serial.println(file.size());
  
    Serial.println("Read from file:");
  
    while(file.available()){
        output += file.read();
    }
    file.close();
    //Serial.print("Read: ");Serial.print(path);Serial.println(", contents: ");
    //Serial.println(output.c_str());
    return output;
}

void saveUnitListFile() {
  std::string list = "";
  for(int i = 0; i < units.size(); i++) {
    list.append(units.at(i));
    list.append("|");  
  }
  list.pop_back();
  writeFile(unitListPath.c_str(), list.c_str());
}
bool loadUnitListFile() {
  std::string unitList = readFile(unitListPath.c_str());
  if(unitList.size() > 0) {
    units.clear();
  } else {
    return false;
  }
  std::size_t found = unitList.find("|");
  while(found != std::string::npos) {
    std::string unit = unitList.substr(0, found);
    Serial.print("unit: ");Serial.println(unit.c_str());
    units.push_back(unit); 
    unitList = unitList.substr(found + 1);
    found = unitList.find("|");
  }
    Serial.print("unit: ");Serial.println(unitList.c_str());
  units.push_back(unitList);
  return true;
}
void setup(void) {
  Serial.begin(115200);

  Serial.println("GAMA Inc. QR Label Serializer for Zebra ZT230 thermal printer");
  Serial.print("Version: ");Serial.println(VERSION);

  EEPROM.begin(EEPROM_SIZE);
  
  init_SPIFFS();

  Serial.println("Reading unit list.");

  loadUnitListFile();
  //Serial.println(unitList.c_str());
  
  startAppTask();
  startBLETask();
}

void loop() {
  vTaskDelete(NULL); 
}

void programChannel()
{
  uart.begin(115200, SERIAL_8N1, 16, 17);
  delay(5);
  String output = "[CHAN";
  output.concat(channel[0]);
  output.concat(channel[1]);
  output.concat(channel[2]);
  output.concat(channel[3]);
  output.concat("]");
  uart.print(output);
  uart.flush();
  uart.end();
  delay(10);  
}


std::string getChannelString() {
  std::string unitChannel = "";
  for(uint8_t i = 0; i < 4; i++) {
    unitChannel.append(1, channel[i]);
  }
  return unitChannel;
}

void printQrLabel(std::string chan)
{
  
  uart.begin(9600, SERIAL_8N1, 16, 17);
  delay(5);
  
  uart.println("^XA");
  uart.println("^CI0,157,48");
  uart.println("^CF0,35");
  uart.print("^FO5,25^FD");
  uart.print(units.at(unitIndex).c_str());
  uart.println("^FS");
  uart.println("^FO10,100^FDChannel^FS");
  uart.print("^FO32,150^FD");
  uart.print(chan.c_str());
  uart.println("^FS");
  uart.println("^FO200,50");
  uart.println("^BQN,2,4");
  uart.print("^FDQA,");
  uart.print(units.at(unitIndex).c_str());
  uart.print(",");
  uart.print(chan.c_str());
  uart.println("^FS");
  uart.println("^XZ");
  uart.flush();
  uart.end();
  delay(10);  
}

void printQrLabel(std::string unit, std::string chan)
{
  
  uart.begin(9600, SERIAL_8N1, 16, 17);
  delay(5);
  
  uart.println("^XA");
  uart.println("^CI0,157,48");
  uart.println("^CF0,35");
  uart.print("^FO5,25^FD");
  uart.print(unit.c_str());
  uart.println("^FS");
  uart.println("^FO10,100^FDChannel^FS");
  uart.print("^FO32,150^FD");
  uart.print(chan.c_str());
  uart.println("^FS");
  uart.println("^FO200,50");
  uart.println("^BQN,2,4");
  uart.print("^FDQA,");
  uart.print(unit.c_str());
  uart.print(",");
  uart.print(chan.c_str());
  uart.println("^FS");
  uart.println("^XZ");
  uart.flush();
  uart.end();
  delay(10);  
}







void print() {
  Serial.println("Printing");
  std::string chan = getChannelString();
  Serial.println(chan.c_str());
  // Send channel to EEPROM
  programChannel();

  // Send unit and channel to label maker
  printQrLabel(chan);


  incrementChannel();
  chan = getChannelString();
  drawChannelLabel(chan);

  EEPROM.write(50, channel_char_index[0]);
  EEPROM.write(51, channel_char_index[1]);
  EEPROM.write(52, channel_char_index[2]);
  EEPROM.write(53, channel_char_index[3]);
  EEPROM.commit();
  
}

void incrementChannel() {
  char carry = 0;
  if(channel_char_index[3] == MAX_CHAR_IN_SET-1) {
    carry = 1;
    channel_char_index[3] = 0;
  } else {
    channel_char_index[3] = channel_char_index[3] + 1;
  }    

  if(carry) {
    carry = 0;
    if(channel_char_index[2] == MAX_CHAR_IN_SET-1) {
      carry = 1;
      channel_char_index[2] = 0;
    } else {
      channel_char_index[2] = channel_char_index[2] + 1;
    }
  }
  

  if(carry) {
    carry = 0;
    if(channel_char_index[1] == MAX_CHAR_IN_SET-1) {
      carry = 1;
      channel_char_index[1] = 0;
    } else {
      channel_char_index[1] = channel_char_index[1] + 1;
    }
  }
  

  if(carry) {
    carry = 0;
    if(channel_char_index[0] == MAX_CHAR_IN_SET-1) {
      carry = 1;
      channel_char_index[0] = 0;
    } else {
      channel_char_index[0] = channel_char_index[0] + 1;
    }
  }
  

  channel[0] = possibleChannelCharacters[channel_char_index[0]];
  channel[1] = possibleChannelCharacters[channel_char_index[1]];
  channel[2] = possibleChannelCharacters[channel_char_index[2]];
  channel[3] = possibleChannelCharacters[channel_char_index[3]];

}

void back() {
  if(unitIndex == 0) {
    unitIndex = units.size() - 1;
  } else {
    unitIndex--;
  }  
  drawUnitLabel(units.at(unitIndex));
}


void forward() {
  if(unitIndex == NUMBER_OF_UNITS - 1) {
    unitIndex = 0;
  } else {
    unitIndex++;
  }  
  drawUnitLabel(units[unitIndex]);
}



bool checkIfButtonPressed(uint16_t touch_X, uint16_t touch_Y, uint16_t button_X, uint16_t button_Y, uint16_t button_W, uint16_t button_H) {
  if(touch_X >= button_X &&
     touch_X <= (button_X + button_W) &&
     touch_Y >= button_Y &&
     touch_Y <= (button_Y + button_H)) return true;
  return false;
}




void drawUnitLabel(std::string label) {
  tft.fillRect(10, 70, 300, 50, TFT_LIGHTGREY);
  if(label.length() > 19) {
    tft.setTextSize(1);
  } else {
    tft.setTextSize(2);
  }
  
  tft.setTextFont(2);
  tft.setTextColor(TFT_BLACK);
  tft.setCursor( 20, 80, 2);
  tft.setTextDatum(MC_DATUM);
  tft.println(label.c_str());
}
void drawBackButton() {
  tft.fillRect(BACK_BUTTON_X, BACK_BUTTON_Y, BACK_BUTTON_W, BACK_BUTTON_H, TFT_WHITE);
  tft.setTextSize(4);
  tft.setTextDatum(MC_DATUM);
  tft.setCursor( 23, 0, 2);
  tft.setTextColor(TFT_BLACK);
  tft.setTextFont(2);
  tft.println("<");
}


void drawForwardButton() {
  tft.fillRect(FORWARD_BUTTON_X, FORWARD_BUTTON_Y, FORWARD_BUTTON_W, FORWARD_BUTTON_H, TFT_WHITE);
  tft.setTextSize(4);
  tft.setTextDatum(MC_DATUM);
  tft.setCursor( 277, 0, 2);
  tft.setTextColor(TFT_BLACK);
  tft.setTextFont(2);
  tft.println(">");
}

void drawChannelLabel(std::string chan) {
  tft.fillRect(70, 10, 180, 60, TFT_GREY);
  tft.setTextSize(4);
  tft.setTextColor(TFT_YELLOW);
  tft.setCursor( 110, 0, 2);
  tft.println(chan.c_str());
}


void drawPrintButton() {
  tft.fillRect(PRINT_BUTTON_X, PRINT_BUTTON_Y, PRINT_BUTTON_W, PRINT_BUTTON_H, TFT_WHITE);
  tft.setTextSize(4);
  tft.setTextDatum(MC_DATUM);
  tft.setCursor( 90, 145, 2);
  tft.setTextColor(TFT_BLACK);
  tft.setTextFont(2);
  tft.println("PRINT");
}






void populatePossibleCharacters() {
  char possible = 97;
  for(int i = 0; i < 26; i++) {
    possibleChannelCharacters[i] = possible++;
  }
  possible = 48;
  for(int i = 0; i < 10; i++) {
    possibleChannelCharacters[i + 26] = possible++;
  }
}
void init_SPIFFS() {
  // check file system exists
  if (!SPIFFS.begin()) {
    Serial.println("Formatting file system");
    SPIFFS.format();
    SPIFFS.begin();
  }
}
void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft.setTouch(calData);
  } else {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}
