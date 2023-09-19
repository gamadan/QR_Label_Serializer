#include "QR_Label_Serializer_ServerCallbacks.h"


/*
 * 
 * 
 * BLE Remote Server Callbacks
 * 
 * 
 * 
 */
QR_Label_Serializer_ServerCallbacks::QR_Label_Serializer_ServerCallbacks(bool* bC) {
  bConnected = bC;
}
QR_Label_Serializer_ServerCallbacks::~QR_Label_Serializer_ServerCallbacks() {
  
}
void QR_Label_Serializer_ServerCallbacks::onConnect(BLEServer* pServer) {
  *bConnected = true;
};

void QR_Label_Serializer_ServerCallbacks::onDisconnect(BLEServer* pServer) {
 *bConnected = false;
};
