#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include "BLEHIDDevice.h"
#include "BLECharacteristic.h"

#define BLE_KEYBOARD_VERSION "0.0.5"
#define BLE_KEYBOARD_VERSION_MAJOR 0
#define BLE_KEYBOARD_VERSION_MINOR 0
#define BLE_KEYBOARD_VERSION_REVISION 5

class BleHandshake : public BLEServerCallbacks, public BLECharacteristicCallbacks
{
private:
  BLEHIDDevice* hid;
  BLECharacteristic* inputKeyboard;
  BLECharacteristic* outputKeyboard;
  BLECharacteristic* inputMediaKeys;
  BLEAdvertising*    advertising;
  std::string        deviceName;
  std::string        deviceManufacturer;
  uint8_t            batteryLevel;
  bool               connected = false;
  uint32_t           _delay_ms = 7;
  void delay_ms(uint64_t ms);

  uint16_t vid       = 0x05ac;
  uint16_t pid       = 0x820a;
  uint16_t version   = 0x0210;

public:
  BleHandshake(std::string deviceName = "ESP32 BleHandshake", std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 69);
  BLEServer* begin(void);
  void end(void);
  bool isConnected(void);
  void setBatteryLevel(uint8_t level);
  void setName(std::string deviceName);  
  void setDelay(uint32_t ms);

  void set_vendor_id(uint16_t vid);
  void set_product_id(uint16_t pid);
  void set_version(uint16_t version);
protected:
  virtual void onStarted(BLEServer *pServer) { };
  virtual void onConnect(BLEServer* pServer) override;
  virtual void onDisconnect(BLEServer* pServer) override;
  virtual void onWrite(BLECharacteristic* me) override;

};

#endif // CONFIG_BT_ENABLED

#endif
