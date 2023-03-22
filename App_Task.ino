

void app_task(void * parameters) {  
  Serial.println("Checking for channel values");
  if(EEPROM.read(50) == 255) {
    Serial.println("No Channel values found");
    EEPROM.write(50, 97);
    EEPROM.write(51, 97);
    EEPROM.write(52, 97);
    EEPROM.write(53, 97);
    EEPROM.commit();
  }
  
  channel_char_index[0] = EEPROM.read(50);
  channel_char_index[1] = EEPROM.read(51);
  channel_char_index[2] = EEPROM.read(52);
  channel_char_index[3] = EEPROM.read(53);

  Serial.print("channel_char_index[0] = ");Serial.println(channel_char_index[0]);
  Serial.print("channel_char_index[1] = ");Serial.println(channel_char_index[1]);
  Serial.print("channel_char_index[2] = ");Serial.println(channel_char_index[2]);
  Serial.print("channel_char_index[3] = ");Serial.println(channel_char_index[3]);
  
  
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
