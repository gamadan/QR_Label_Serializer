#include "Unit_CharCallbacks.h"

Unit_CharCallbacks::Unit_CharCallbacks(TickType_t* lastReceivedTick, std::string* rxMessage) {
  lastReceviedTickCount = lastReceivedTick;
  rx_message = rxMessage;
}
Unit_CharCallbacks::~Unit_CharCallbacks() {

}
void Unit_CharCallbacks::onRead(BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param) {
	
}
void Unit_CharCallbacks::onWrite(BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param) {
	std::string rxValue = pCharacteristic->getValue();
	if (rxValue.length() > 0) {
		*rx_message = rxValue;
	}  
}
void Unit_CharCallbacks::onNotify(BLECharacteristic* pCharacteristic) {
	
}
void Unit_CharCallbacks::onStatus(BLECharacteristic* pCharacteristic, Status s, uint32_t code) {
	
}
