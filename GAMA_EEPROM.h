/*
 * GAMA_EEPROM.h
 *
 * Created: 7/1/2022
 *  Author: Daniel Nebert
 *
 */ 
#ifndef ARDUINO_H_
#define ARDUINO_H_
#include "Arduino.h"
#endif

#ifndef EEPROM_H_
#define EEPROM_H_
#include <EEPROM.h>
#endif

#ifndef GAMA_DEF_EEPROM_H_
#define GAMA_DEF_EEPROM_H_
#include "GAMA_Def_EEPROM.h"
#endif

class GAMA_EEPROM {
  public:
    GAMA_EEPROM(int s);
    void begin();
    String initChannelValues();
    
    void saveChannelValues(String v);
    String loadChannelValues();
    bool hasChannelValues();

    String initPassword();
    
    void savePassword(String pw);
    String loadPassword();
    bool hasEmptyPassword();
    void savePasswordLength(uint8_t l);

    uint8_t getDataPin();
    uint8_t getSelectorPin();
    uint8_t getRSSIInputPin();

    void setDataPin(uint8_t p);
    void setSelectorPin(uint8_t p);
    void setRSSIInputPin(uint8_t p);
  private:
    int size;
  
  protected:
    // none yet
};
