#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define UNIT_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHANNEL_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a9"

TickType_t lastReceivedTick = xTaskGetTickCount();
bool bDeviceConnected, bOldDeviceConnected = false;

/*
 * 
BLECharacteristic* unitCharacteristic, channelCharacteristic;
std::string rxUnitMessage = "";
std::string rxChannelMessage = "";

 */

void ble_task(void * parameters) {  
  
  BLEDevice::init("QR Serializer");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks(&bDeviceConnected));
  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  unitCharacteristic = pService->createCharacteristic(
                                         UNIT_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  channelCharacteristic = pService->createCharacteristic(
                                         CHANNEL_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );


  unitCharacteristic->setCallbacks(new Unit_CharCallbacks(&lastReceivedTick, &rxUnitMessage));
  channelCharacteristic->setCallbacks(new Channel_CharCallbacks(&lastReceivedTick, &rxChannelMessage));
  
  pService->start();
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
  
  TickType_t currentTickCount = xTaskGetTickCount();
  for( ;; ) {
    vTaskDelay(50 / portTICK_PERIOD_MS ); 
    currentTickCount = xTaskGetTickCount();
    if (bDeviceConnected) {
    delay(10); // bluetooth stack will go into congestion, if too many packets are sent
    }
    // disconnecting
    if (!bDeviceConnected && bOldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        BLEDevice::startAdvertising();//pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        bOldDeviceConnected = bDeviceConnected;
    }
    
    // connecting
    if (bDeviceConnected && !bOldDeviceConnected) {
      Serial.println("Device connecting.");
      bOldDeviceConnected = bDeviceConnected;
    }
  }
}
