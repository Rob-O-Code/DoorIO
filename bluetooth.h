#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

class MyBluetooth {
  public:
    MyBluetooth() {};
    void init();
    void testScan();
  private:
    static const int scanTime = 5;
    BLEScan* pBLEScan;
};

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    static std::string NatePhones = "94:db:56:02:61:6b";
    static BLEAddress NateAddress = BLEAddress(NatePhones);
    //Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    if (advertisedDevice.getAddress().equals(NateAddress)) {
      Serial.printf("Nate RSSI: %d\n", advertisedDevice.getRSSI());
    }
  }
};

#endif
