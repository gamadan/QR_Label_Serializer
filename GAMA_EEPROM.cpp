/*
 * GAMA_EEPROM.cpp
 *
 * Created: 5/1/2021 11:49:09 AM
 *  Author: dpneb
 */ 

#include "GAMA_EEPROM.h"

GAMA_EEPROM::GAMA_EEPROM(int s) {
  size = s;  
}
void GAMA_EEPROM::begin() {
  EEPROM.begin(size);
}

String GAMA_EEPROM::initChannelValues() {
  
  if(!hasChannelValues()) {
    saveChannelValues(CHANNEL_DEFAULT_VALUES);
  }
  return loadChannelValues();
}

void GAMA_EEPROM::saveChannelValues(String v) {
  EEPROM.write(CHANNEL_START_ADDRESS+0, v.charAt(0));
  EEPROM.write(CHANNEL_START_ADDRESS+1, v.charAt(1));
  EEPROM.write(CHANNEL_START_ADDRESS+2, v.charAt(2));
  EEPROM.write(CHANNEL_START_ADDRESS+3, v.charAt(3));
  EEPROM.commit();
}

String GAMA_EEPROM::loadChannelValues() {
  String output = "";
  for(int i = 0; i < CHANNEL_LENGTH; i++) {
    output.concat((char)EEPROM.read(CHANNEL_START_ADDRESS + i));
  }
  //Serial.print("Loaded Channel Value: ");//Serial.println(output);
  return output;
}

bool GAMA_EEPROM::hasChannelValues() {
  for(uint8_t i = 0; i < CHANNEL_LENGTH; i++) {
    
    if(EEPROM.read(CHANNEL_START_ADDRESS + i) != 0xFF) return true;
  }
  return false;
}


String GAMA_EEPROM::initPassword() {
  if(hasEmptyPassword()) {
    savePassword(PASSWORD_DEFAULT_VALUES);
  }
  return loadPassword();
}

void GAMA_EEPROM::savePassword(String pw) {
  int password_length = pw.length();
  char* password = (char*)malloc(sizeof(char) * (password_length + 1));
  pw.toCharArray(password, password_length + 1);
  //Serial.print("Saving password length: ");//Serial.print(password_length);//Serial.print(", to address: ");//Serial.println(PASSWORD_LENGTH_ADDRESS);
  EEPROM.write(PASSWORD_LENGTH_ADDRESS, password_length);
  for(int i = 0; i < password_length; i++) {
    //Serial.print("Saving password: ");//Serial.print(password[i]);//Serial.print(", to address: ");//Serial.println(PASSWORD_ADDRESS_START + i);
    EEPROM.write(PASSWORD_ADDRESS_START + i, password[i]);
  }
  EEPROM.commit();
  for(int i = password_length; i < PASSWORD_MAX_CHARACTERS; i++) {
    EEPROM.write(PASSWORD_ADDRESS_START + i, 0xFF);
  }
  EEPROM.commit();
  GAMA_EEPROM::savePasswordLength(password_length);
  free(password);
}




String GAMA_EEPROM::loadPassword() {
  String output = "";
  int length = EEPROM.read(PASSWORD_LENGTH_ADDRESS);
  for(int i = 0; i < length; i++) {
    output.concat((char)EEPROM.read(PASSWORD_ADDRESS_START + i));
  }
  return output;
}





bool GAMA_EEPROM::hasEmptyPassword() {
  for(int i = 0; i < PASSWORD_MAX_CHARACTERS; i++) {
    if(EEPROM.read(i) != 255) {
      return false;
    }
  }
  return true;
}

void GAMA_EEPROM::savePasswordLength(uint8_t l) {
  EEPROM.write(PASSWORD_LENGTH_ADDRESS, l);
  EEPROM.commit();
}




uint8_t GAMA_EEPROM::getDataPin() {
  return EEPROM.read(DATA_PIN_ADDRESS);
}
uint8_t GAMA_EEPROM::getSelectorPin() {
  return EEPROM.read(SELECTOR_PIN_ADDRESS);  
}

uint8_t GAMA_EEPROM::getRSSIInputPin() {
  return EEPROM.read(RSSI_INPUT_PIN_ADDRESS);  
}


void GAMA_EEPROM::setDataPin(uint8_t p) {
  EEPROM.write(DATA_PIN_ADDRESS, p);
  EEPROM.commit();
}
void GAMA_EEPROM::setSelectorPin(uint8_t p) {
  EEPROM.write(SELECTOR_PIN_ADDRESS, p);
  EEPROM.commit();
}
void GAMA_EEPROM::setRSSIInputPin(uint8_t p) {
  EEPROM.write(RSSI_INPUT_PIN_ADDRESS, p);
  EEPROM.commit();
}
