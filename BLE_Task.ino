#define PRINT_LABEL_SERVICE_UUID        "2e01ae6e-5726-11ee-8c99-0242ac120002"
#define ADD_REMOVE_SERVICE_UUID         "2e01b27e-5726-11ee-8c99-0242ac120002"
#define RAWDATA_SERVICE_UUID            "2e01b3dc-5726-11ee-8c99-0242ac120002"
#define UNIT_CHARACTERISTIC_UUID        "2e01b4f4-5726-11ee-8c99-0242ac120002"
#define CHANNEL_CHARACTERISTIC_UUID     "2e01b986-5726-11ee-8c99-0242ac120002"
#define ADDUNIT_CHARACTERISTIC_UUID     "2e01baa8-5726-11ee-8c99-0242ac120002"
#define REMOVEUNIT_CHARACTERISTIC_UUID  "2e01bbac-5726-11ee-8c99-0242ac120002"
#define RAWDATA_CHARACTERISTIC_UUID     "2e01bcb0-5726-11ee-8c99-0242ac120002"



TickType_t lastReceivedTick = xTaskGetTickCount();
bool bDeviceConnected, bOldDeviceConnected = false;

void ble_task(void * parameters) {  
  
  BLEDevice::init("QR Serializer");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new QR_Label_Serializer_ServerCallbacks(&bDeviceConnected));


  /*
   * Pringing a label for a user entered Unit and user entered Channel
   */
  printLabelService = pServer->createService(PRINT_LABEL_SERVICE_UUID);
  unitCharacteristic = printLabelService->createCharacteristic(
                                         UNIT_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );                                       
  unitCharacteristic->setCallbacks(new Unit_CharCallbacks(&lastReceivedTick, &rxUnitMessage));
  unitCharacteristic->addDescriptor(new BLE2902());
  BLEDescriptor unit_Descriptor("2901"); // Create descriptor
  unit_Descriptor.setValue("Print Unit"); // Set descriptor value
  unitCharacteristic->addDescriptor(&unit_Descriptor); // Add descriptor to characteristic
  /////////// Channel follows //////////
  channelCharacteristic = printLabelService->createCharacteristic(
                                         CHANNEL_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  channelCharacteristic->setCallbacks(new Channel_CharCallbacks(&lastReceivedTick, &rxChannelMessage));                                       
  channelCharacteristic->addDescriptor(new BLE2902());
  BLEDescriptor channel_Descriptor("2901"); // Create descriptor
  channel_Descriptor.setValue("Print Channel"); // Set descriptor value
  channelCharacteristic->addDescriptor(&channel_Descriptor); // Add descriptor to characteristic







  /*
   * For adding and removing unit from the list
   */
  addRemoveService = pServer->createService(ADD_REMOVE_SERVICE_UUID);
  addUnitCharacteristic = addRemoveService->createCharacteristic(
                                         ADDUNIT_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  addUnitCharacteristic->setCallbacks(new AddUnit_CharCallbacks(&rxAddUnitMessage));

  addUnitCharacteristic->addDescriptor(new BLE2902());
  BLEDescriptor addUnit_Descriptor("2901"); // Create descriptor
  addUnit_Descriptor.setValue("Add unit"); // Set descriptor value
  addUnitCharacteristic->addDescriptor(&addUnit_Descriptor); // Add descriptor to characteristic
  ////////// Remove follows //////////
  removeUnitCharacteristic = addRemoveService->createCharacteristic(
                                         REMOVEUNIT_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  removeUnitCharacteristic->setCallbacks(new RemoveUnit_CharCallbacks(&rxRemoveUnitMessage));

  removeUnitCharacteristic->addDescriptor(new BLE2902());  
  BLEDescriptor removeUnit_Descriptor("2901"); // Create descriptor
  removeUnit_Descriptor.setValue("Remove unit"); // Set descriptor value
  removeUnitCharacteristic->addDescriptor(&removeUnit_Descriptor); // Add descriptor to characteristic 
  
  
  
  
  
  
  
  /*
   * Any data sent to this characteristic, will be passed through
   * to the label printer
   */
  rawDataService = pServer->createService(RAWDATA_CHARACTERISTIC_UUID);
  rawDataCharacteristic = rawDataService->createCharacteristic(
                                         RAWDATA_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  rawDataCharacteristic->setCallbacks(new RawData_CharCallbacks(&uart));

  rawDataCharacteristic->addDescriptor(new BLE2902());  
  BLEDescriptor rawDataDescriptor("2901"); // Create descriptor
  rawDataDescriptor.setValue("Raw data"); // Set descriptor value
  rawDataCharacteristic->addDescriptor(&rawDataDescriptor); // Add descriptor to characteristic  

  
  delay(500);
  
  printLabelService->start();
  addRemoveService->start();
  rawDataService->start();
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(PRINT_LABEL_SERVICE_UUID);
  pAdvertising->addServiceUUID(ADD_REMOVE_SERVICE_UUID);
  pAdvertising->addServiceUUID(RAWDATA_SERVICE_UUID);
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
