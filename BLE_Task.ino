#define SERVICE_UUID                    "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define UNIT_CHARACTERISTIC_UUID        "beb54830-36e1-4688-b7f5-ea07361b26a8"
#define CHANNEL_CHARACTERISTIC_UUID     "beb54831-36e1-4688-b7f5-ea07361b26a8"
#define ADDUNIT_CHARACTERISTIC_UUID     "beb54842-36e1-4688-b7f5-ea07361b26a8"
#define REMOVEUNIT_CHARACTERISTIC_UUID  "beb54843-36e1-4688-b7f5-ea07361b26a8"
#define RAWDATA_CHARACTERISTIC_UUID  "beb54844-36e1-4688-b7f5-ea07361b26a8"

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
  addUnitCharacteristic = pService->createCharacteristic(
                                         ADDUNIT_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  removeUnitCharacteristic = pService->createCharacteristic(
                                         REMOVEUNIT_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  rawDataCharacteristic = pService->createCharacteristic(
                                         RAWDATA_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );


  unitCharacteristic->setCallbacks(new Unit_CharCallbacks(&lastReceivedTick, &rxUnitMessage));
  channelCharacteristic->setCallbacks(new Channel_CharCallbacks(&lastReceivedTick, &rxChannelMessage));
  addUnitCharacteristic->setCallbacks(new AddUnit_CharCallbacks(&rxAddUnitMessage));
  removeUnitCharacteristic->setCallbacks(new RemoveUnit_CharCallbacks(&rxRemoveUnitMessage));
  removeUnitCharacteristic->setCallbacks(new RawData_CharCallbacks(&uart));

  rawDataCharacteristic->addDescriptor(new BLE2902());
  BLEDescriptor rawDataDescriptor("2901"); // Create descriptor
  rawDataDescriptor.setValue("Send Raw Print Data"); // Set descriptor value
  unitCharacteristic->addDescriptor(&rawDataDescriptor); // Add descriptor to characteristic

  unitCharacteristic->addDescriptor(new BLE2902());
  BLEDescriptor unit_Descriptor("2901"); // Create descriptor
  unit_Descriptor.setValue("Print Unit"); // Set descriptor value
  unitCharacteristic->addDescriptor(&unit_Descriptor); // Add descriptor to characteristic

  channelCharacteristic->addDescriptor(new BLE2902());
  BLEDescriptor channel_Descriptor("2901"); // Create descriptor
  channel_Descriptor.setValue("Print Channel"); // Set descriptor value
  channelCharacteristic->addDescriptor(&channel_Descriptor); // Add descriptor to characteristic

  

  addUnitCharacteristic->addDescriptor(new BLE2902());
  BLEDescriptor addUnit_Descriptor("2901"); // Create descriptor
  addUnit_Descriptor.setValue("Add unit"); // Set descriptor value
  addUnitCharacteristic->addDescriptor(&addUnit_Descriptor); // Add descriptor to characteristic

  removeUnitCharacteristic->addDescriptor(new BLE2902());
  BLEDescriptor removeUnit_Descriptor("2901"); // Create descriptor
  removeUnit_Descriptor.setValue("Remove unit"); // Set descriptor value
  removeUnitCharacteristic->addDescriptor(&removeUnit_Descriptor); // Add descriptor to characteristic



  delay(500);
  
  pService->start();
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
  
  //TickType_t currentTickCount = xTaskGetTickCount();
  for( ;; ) {
    vTaskDelay(50 / portTICK_PERIOD_MS ); 
    //currentTickCount = xTaskGetTickCount();
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
