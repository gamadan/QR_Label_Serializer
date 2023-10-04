

void app_task(void * parameters) {  
  Serial.println("Checking for channel values");
  if(EEPROM.read(QR_BLE_CHANNEL_ADDRESS_0) == 255 || EEPROM.read(QR_BLE_CHANNEL_ADDRESS_1) == 255 || EEPROM.read(QR_BLE_CHANNEL_ADDRESS_2) == 255 || EEPROM.read(QR_BLE_CHANNEL_ADDRESS_3) == 255) {
    Serial.println("No Channel values found");
    EEPROM.write(QR_BLE_CHANNEL_ADDRESS_0, 0);
    EEPROM.write(QR_BLE_CHANNEL_ADDRESS_1, 0);
    EEPROM.write(QR_BLE_CHANNEL_ADDRESS_2, 0);
    EEPROM.write(QR_BLE_CHANNEL_ADDRESS_3, 0);
    EEPROM.commit();
  }


  EEPROM.write(QR_BLE_CHANNEL_ADDRESS_0, 1);
  EEPROM.commit();
  channel_char_index[0] = EEPROM.read(QR_BLE_CHANNEL_ADDRESS_0);
  channel_char_index[1] = EEPROM.read(QR_BLE_CHANNEL_ADDRESS_1);
  channel_char_index[2] = EEPROM.read(QR_BLE_CHANNEL_ADDRESS_2);
  channel_char_index[3] = EEPROM.read(QR_BLE_CHANNEL_ADDRESS_3);

  Serial.print("channel_char_index[0] = ");Serial.println(channel_char_index[0]);
  Serial.print("channel_char_index[1] = ");Serial.println(channel_char_index[1]);
  Serial.print("channel_char_index[2] = ");Serial.println(channel_char_index[2]);
  Serial.print("channel_char_index[3] = ");Serial.println(channel_char_index[3]);
  
  Serial.println("1");
  tft.init();
  tft.setRotation(1);
  x = 0;
  y = 0;
  Serial.println("2");
  unitIndex = 0;
  Serial.println("3");

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
  Serial.printf("unitIndex = %d\r\n", unitIndex);
  Serial.printf("units.size() = %d\r\n", units.size());
  Serial.printf("units.at(unitIndex) = %s\r\n", units.at(unitIndex).c_str());
  drawUnitLabel(units.at(unitIndex));
  drawChannelLabel(getChannelString());
  drawPrintButton();
  
  // Set "cursor" at top left corner of display (0,0) and select font 2
  // (cursor will move to next line automatically during printing with 'tft.println'
  //  or stay on the line is there is room for the text with tft.print)
  //tft.setCursor(0, 0, 2);


  
  for( ;; ) {
    vTaskDelay(50 / portTICK_PERIOD_MS ); 
    if(!rxUnitMessage.empty() && !rxChannelMessage.empty()) {
      Serial.println("BLE Printing:");
      Serial.print("Unit:");Serial.println(rxUnitMessage.c_str());
      Serial.print("Channel:");Serial.println(rxChannelMessage.c_str());
      printQrLabel(rxUnitMessage, rxChannelMessage);
      rxUnitMessage = "";
      rxChannelMessage = "";
    }
      //std::string rxAddUnitMessage = "";
      //std::string rxRemoveUnitMessage = "";

    if(!rxAddUnitMessage.empty()) {
      
      std::string name = rxAddUnitMessage;
      if(doesUnitExistInList(name)) {
        Serial.println("Unit already exists!");
      } else {
        units.push_back(name);
        saveUnitListFile();
        Serial.println("Restarting ESP32");
        delay(2000);
        esp_restart();
      }
    } else if(!rxRemoveUnitMessage.empty()) {
      std::string name = rxRemoveUnitMessage;
      if(!doesUnitExistInList(name)) {
        Serial.println("Unit does not exist!");
      } else {
        for(int i = 0; i < units.size(); i++) {
          if(units.at(i).compare(name) == 0) {
            units.erase(units.begin()+i);
          }
        }        
        saveUnitListFile();
        Serial.println("Restarting ESP32");
        delay(2000);
        esp_restart();
      }
    }
  
    
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
}
