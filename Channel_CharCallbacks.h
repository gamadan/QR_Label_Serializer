#ifndef CHANNEL_CHARCALLBACKS_H_
#define CHANNEL_CHARCALLBACKS_H_

#include "Arduino.h"
#include <freertos/stream_buffer.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

class Channel_CharCallbacks: public BLECharacteristicCallbacks {
  public:
    Channel_CharCallbacks(TickType_t* lastReceivedTick, std::string* rxMessage);
    ~Channel_CharCallbacks();
    //void onWrite(BLECharacteristic *pCharacteristic);
	
	/**
	 * @brief Callback function to support a read request.
	 * @param [in] pCharacteristic The characteristic that is the source of the event.
	 * @param [in] param The BLE GATTS param. Use param->read.
	 */
	void onRead(BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param);

	/**
	 * @brief Callback function to support a write request.
	 * @param [in] pCharacteristic The characteristic that is the source of the event.
	 * @param [in] param The BLE GATTS param. Use param->write.
	 */
	void onWrite(BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param);

	/**
	 * @brief Callback function to support a Notify request.
	 * @param [in] pCharacteristic The characteristic that is the source of the event.
	 */
	void onNotify(BLECharacteristic* pCharacteristic);

	/**
	 * @brief Callback function to support a Notify/Indicate Status report.
	 * @param [in] pCharacteristic The characteristic that is the source of the event.
	 * @param [in] s Status of the notification/indication
	 * @param [in] code Additional code of underlying errors
	 */
	void onStatus(BLECharacteristic* pCharacteristic, Status s, uint32_t code);
	
  private:
    std::string *rx_message;
    TickType_t *lastReceviedTickCount;
};

#endif
