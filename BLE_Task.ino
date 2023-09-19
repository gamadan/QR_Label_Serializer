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

/*
 * 
BLECharacteristic* unitCharacteristic, channelCharacteristic;
std::string rxUnitMessage = "";
std::string rxChannelMessage = "";

 */

void initUnitCharacteristic() {
  unitCharacteristic = printLabelService->createCharacteristic(
                                         UNIT_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
                                       
  unitCharacteristic->setCallbacks(new Unit_CharCallbacks(&lastReceivedTick, &rxUnitMessage));

  
  
}


void initChannelCharacteristic() {
  channelCharacteristic = printLabelService->createCharacteristic(
                                         CHANNEL_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
                                       
  channelCharacteristic->setCallbacks(new Channel_CharCallbacks(&lastReceivedTick, &rxChannelMessage));

}

void initAddUnitCharacteristic() {
  addUnitCharacteristic = addRemoveService->createCharacteristic(
                                         ADDUNIT_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  addUnitCharacteristic->setCallbacks(new AddUnit_CharCallbacks(&rxAddUnitMessage));


}

void initRemoveUnitCharacteristic() {
  removeUnitCharacteristic = addRemoveService->createCharacteristic(
                                         REMOVEUNIT_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  removeUnitCharacteristic->setCallbacks(new RemoveUnit_CharCallbacks(&rxRemoveUnitMessage));

}

void initRawDataCharacteristic() {
  rawDataCharacteristic = rawDataService->createCharacteristic(
                                         RAWDATA_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  rawDataCharacteristic->setCallbacks(new RawData_CharCallbacks(&uart));

  
}

void ble_task(void * parameters) {  
  
  BLEDevice::init("QR Serializer");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new QR_Label_Serializer_ServerCallbacks(&bDeviceConnected));
  printLabelService = pServer->createService(PRINT_LABEL_SERVICE_UUID);
  addRemoveService = pServer->createService(ADD_REMOVE_SERVICE_UUID);
  rawDataService = pServer->createService(RAWDATA_CHARACTERISTIC_UUID);



  initUnitCharacteristic();
  initChannelCharacteristic();
  initAddUnitCharacteristic();
  initRemoveUnitCharacteristic();
  initRawDataCharacteristic();

  
/*
 * For some reason, this doesn't work correctly. 
 * 
 * 
  BLEDescriptor unit_Descriptor("2901"); // Create descriptor
  BLEDescriptor channel_Descriptor("2901"); // Create descriptor
  BLEDescriptor addUnit_Descriptor("2901"); // Create descriptor
  BLEDescriptor removeUnit_Descriptor("2901"); // Create descriptor
  BLEDescriptor rawDataDescriptor("2901"); // Create descriptor
  

  unitCharacteristic->addDescriptor(new BLE2902());
  unit_Descriptor.setValue("Print Unit"); // Set descriptor value
  unitCharacteristic->addDescriptor(&unit_Descriptor); // Add descriptor to characteristic

  channelCharacteristic->addDescriptor(new BLE2902());
  channel_Descriptor.setValue("Print Channel"); // Set descriptor value

  addUnitCharacteristic->addDescriptor(new BLE2902());  
  addUnit_Descriptor.setValue("Add unit"); // Set descriptor value
  addUnitCharacteristic->addDescriptor(&addUnit_Descriptor); // Add descriptor to characteristic
  
  removeUnitCharacteristic->addDescriptor(new BLE2902());  
  removeUnit_Descriptor.setValue("Remove unit"); // Set descriptor value
  removeUnitCharacteristic->addDescriptor(&removeUnit_Descriptor); // Add descriptor to characteristic

  rawDataCharacteristic->addDescriptor(new BLE2902());  
  rawDataDescriptor.setValue("Raw data"); // Set descriptor value
  unitCharacteristic->addDescriptor(&rawDataDescriptor); // Add descriptor to characteristic

  Serial.println("unit_Descriptor discriptor:");
  Serial.println(unit_Descriptor.toString().c_str());
  Serial.println("");

  Serial.println("asdfasdf discriptor:");
  Serial.println(channel_Descriptor.toString().c_str());
  Serial.println("");

  Serial.println("addUnit_Descriptor discriptor:");
  Serial.println(addUnit_Descriptor.toString().c_str());
  Serial.println("");

  Serial.println("removeUnit_Descriptor discriptor:");
  Serial.println(removeUnit_Descriptor.toString().c_str());
  Serial.println("");

  Serial.println("rawDataDescriptor discriptor:");
  Serial.println(rawDataDescriptor.toString().c_str());
  Serial.println("");
*/

  

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
