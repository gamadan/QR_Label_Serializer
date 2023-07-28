#include "RemoveUnit_CharCallbacks.h"

RemoveUnit_CharCallbacks::RemoveUnit_CharCallbacks(std::string* rxMessage) {
  rx_message = rxMessage;
}
RemoveUnit_CharCallbacks::~RemoveUnit_CharCallbacks() {

}
void RemoveUnit_CharCallbacks::onRead(BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param) {
  
}
void RemoveUnit_CharCallbacks::onWrite(BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param) {
  std::string rxValue = pCharacteristic->getValue();
  if (rxValue.length() > 0) {
    *rx_message = rxValue;
  }  
}
void RemoveUnit_CharCallbacks::onNotify(BLECharacteristic* pCharacteristic) {
  
}
void RemoveUnit_CharCallbacks::onStatus(BLECharacteristic* pCharacteristic, Status s, uint32_t code) {
  
}
