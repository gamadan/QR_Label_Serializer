#include "Channel_CharCallbacks.h"

Channel_CharCallbacks::Channel_CharCallbacks(TickType_t* lastReceivedTick, std::string* rxMessage) {
  lastReceviedTickCount = lastReceivedTick;
  rx_message = rxMessage;
}
Channel_CharCallbacks::~Channel_CharCallbacks() {

}
void Channel_CharCallbacks::onRead(BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param) {
  
}
void Channel_CharCallbacks::onWrite(BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param) {
  std::string rxValue = pCharacteristic->getValue();
  if (rxValue.length() > 0) {
    *rx_message = rxValue;
  }  
}
void Channel_CharCallbacks::onNotify(BLECharacteristic* pCharacteristic) {
  
}
void Channel_CharCallbacks::onStatus(BLECharacteristic* pCharacteristic, Status s, uint32_t code) {
  
}
