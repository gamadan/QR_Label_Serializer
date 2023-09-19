#ifndef QR_LABEL_SERIALIZER__SERVERCALLBACKS_H_
#define QR_LABEL_SERIALIZER__SERVERCALLBACKS_H_

#include <atomic>
#include <BLEServer.h>

class QR_Label_Serializer_ServerCallbacks: public BLEServerCallbacks {
  public:
    QR_Label_Serializer_ServerCallbacks(bool* bC);
    ~QR_Label_Serializer_ServerCallbacks();
    void onConnect(BLEServer* pServer);
    void onDisconnect(BLEServer* pServer);

  private:
    bool* bConnected;
};

#endif		// QR_LABEL_SERIALIZER__SERVERCALLBACKS_H_
