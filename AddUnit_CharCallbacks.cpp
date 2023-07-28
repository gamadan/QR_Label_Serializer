#include "AddUnit_CharCallbacks.h"

AddUnit_CharCallbacks::AddUnit_CharCallbacks(std::string* rxMessage) {
  rx_message = rxMessage;
}
AddUnit_CharCallbacks::~AddUnit_CharCallbacks() {

}
void AddUnit_CharCallbacks::onRead(BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param) {
  
}
void AddUnit_CharCallbacks::onWrite(BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param) {
  std::string rxValue = pCharacteristic->getValue();
  if (rxValue.length() > 0) {
    Serial.print("rxValue:");Serial.println(rxValue.c_str());
    *rx_message = rxValue;
  }  
}
void AddUnit_CharCallbacks::onNotify(BLECharacteristic* pCharacteristic) {
  
}
void AddUnit_CharCallbacks::onStatus(BLECharacteristic* pCharacteristic, Status s, uint32_t code) {
  
}
