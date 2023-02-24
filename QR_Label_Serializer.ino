/*  
 Test the tft.print() viz embedded tft.write() function

 This sketch used font 2, 4, 7

 Make sure all the display driver and pin connections are correct by
 editing the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################
 */
#include <HardwareSerial.h>
#include "FS.h"
#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>
#include "Def.h"
#include "GAMA_EEPROM.h"


#define EEPROM_SIZE        512
GAMA_EEPROM eeprom = GAMA_EEPROM(EEPROM_SIZE);

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


char channel_char_index[] = {0, 0, 0, 0};

char channel[4];

void setup(void) {
  Serial.begin(115200);

  eeprom.begin();

  Serial.println("Checking for channel values");
  if(!eeprom.hasChannelValues()) {
    Serial.println("No Channel values found");
    String values = "";
    values.concat(0);
    eeprom.saveChannelValues(values);
  }
  
  String values = eeprom.loadChannelValues();

  Serial.println(values.charAt(0));
  Serial.println(values.charAt(1));
  Serial.println(values.charAt(2));
  Serial.println(values.charAt(3));
  
  channel_char_index[0] = values.charAt(0);
  channel_char_index[1] = values.charAt(1);
  channel_char_index[2] = values.charAt(2);
  channel_char_index[3] = values.charAt(3);
  
  
  
  tft.init();
  tft.setRotation(1);
  x = 0;
  y = 0;
  unitIndex = 0;

  populatePossibleCharacters();
  channel[0] = possibleChannelCharacters[channel_char_index[0]];
  channel[1] = possibleChannelCharacters[channel_char_index[1]];
  channel[2] = possibleChannelCharacters[channel_char_index[2]];
  channel[3] = possibleChannelCharacters[channel_char_index[3]];

  // call screen calibration
  touch_calibrate();



  // Fill screen with grey so we can see the effect of printing with and without 
  // a background colour defined
  tft.fillScreen(TFT_GREY);
  
  tft.setTextDatum(MC_DATUM);
  
  drawBackButton();
  drawForwardButton();
  drawUnitLabel(units[unitIndex]);
  drawChannelLabel(getChannelString());
  drawPrintButton();
  
  // Set "cursor" at top left corner of display (0,0) and select font 2
  // (cursor will move to next line automatically during printing with 'tft.println'
  //  or stay on the line is there is room for the text with tft.print)
  //tft.setCursor(0, 0, 2);

}

void loop() {
  tft.getTouch(&x, &y);



  if(checkIfButtonPressed(x, y, PRINT_BUTTON_X, PRINT_BUTTON_Y, PRINT_BUTTON_W, PRINT_BUTTON_H)) {
    print();    
    delay(BUTTON_DELAY);  
  }
  if(checkIfButtonPressed(x, y, BACK_BUTTON_X, BACK_BUTTON_Y, BACK_BUTTON_W, BACK_BUTTON_H)) {
    back();
    delay(BUTTON_DELAY); 
  }
  if(checkIfButtonPressed(x, y, FORWARD_BUTTON_X, FORWARD_BUTTON_Y, FORWARD_BUTTON_W, FORWARD_BUTTON_H)) {
    forward();
    delay(BUTTON_DELAY); 
  }
  
  

  x = 0;
  y = 0;
  delay(10);  
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
  uart.print(units[unitIndex].c_str());
  uart.println("^FS");
  uart.println("^FO10,100^FDChannel^FS");
  uart.print("^FO32,150^FD");
  uart.print(chan.c_str());
  uart.println("^FS");
  uart.println("^FO200,50");
  uart.println("^BQN,2,4");
  uart.print("^FDQA,");
  uart.print(units[unitIndex].c_str());
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
  std::string values = "";
  for(uint8_t i = 0; i < 4; i++) {
    values.append(1, channel_char_index[i]);
  }
  eeprom.saveChannelValues(values.c_str());
  drawChannelLabel(chan);
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
      channel_char_index[1] = channel_char_index[13] + 1;
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
    unitIndex = NUMBER_OF_UNITS - 1;
  } else {
    unitIndex--;
  }  
  drawUnitLabel(units[unitIndex]);
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

void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  if (!SPIFFS.begin()) {
    Serial.println("Formatting file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

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
