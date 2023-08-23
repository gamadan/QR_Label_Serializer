#include "RawData_CharCallbacks.h"

RawData_CharCallbacks::RawData_CharCallbacks(HardwareSerial* serial) {
  uart = serial;
}
RawData_CharCallbacks::~RawData_CharCallbacks() {

}
void RawData_CharCallbacks::onRead(BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param) {
  
}
void RawData_CharCallbacks::onWrite(BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param) {
  std::string rxValue = pCharacteristic->getValue();
  if (rxValue.length() > 0) {
    Serial.print("rxValue:");Serial.println(rxValue.c_str());
    uart->begin(115200, SERIAL_8N1, 16, 17);
    delay(5);
    uart->print(rxValue.c_str());
    uart->flush();
    uart->end();
    delay(10);
  }  
}
void RawData_CharCallbacks::onNotify(BLECharacteristic* pCharacteristic) {
  
}
void RawData_CharCallbacks::onStatus(BLECharacteristic* pCharacteristic, Status s, uint32_t code) {
  
}
