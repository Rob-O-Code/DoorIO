#include "bluetooth.h"

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "BLE2902.h"
#include "BLEHIDDevice.h"

#include "HIDTypes.h"
#include <driver/adc.h>
#include "sdkconfig.h"

#if defined(CONFIG_ARDUHAL_ESP_LOG)
  #include "esp32-hal-log.h"
  #define LOG_TAG ""
#else
  #include "esp_log.h"
  static const char* LOG_TAG = "BLEDevice";
#endif

static const uint8_t _hidReportDescriptor[] = {
  0x85, 0x01,
  0x05, 0xff,
  0x09, 0x01,
  0xa1, 0x01,
  0x25, 0x01,
  0x75, 0x01,
  0x05, 0x09,
  0x19, 0x01,
  0x29, 0x03,
  0x95, 0x03,
  0x81, 0x02,
  0xc0,
};

BleHandshake::BleHandshake(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel) 
    : hid(0)
    , deviceName(std::string(deviceName).substr(0, 15))
    , deviceManufacturer(std::string(deviceManufacturer).substr(0,15))
    , batteryLevel(batteryLevel) {}

BLEServer* BleHandshake::begin(void)
{
  BLEDevice::init(deviceName);
  BLEServer* pServer = BLEDevice::createServer();
  pServer->setCallbacks(this);

  hid = new BLEHIDDevice(pServer);

  hid->manufacturer()->setValue(deviceManufacturer);

  hid->pnp(0x02, vid, pid, version);
  hid->hidInfo(0x00, 0x01);

  BLESecurity* pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND);

  hid->reportMap((uint8_t*)_hidReportDescriptor, sizeof(_hidReportDescriptor));
  hid->startServices();

  onStarted(pServer);

  advertising = pServer->getAdvertising();
  advertising->setAppearance(GENERIC_HID);
  advertising->addServiceUUID(hid->hidService()->getUUID());
  advertising->setScanResponse(false);
  advertising->start();
  hid->setBatteryLevel(batteryLevel);

  ESP_LOGD(LOG_TAG, "Advertising started!");

  return pServer;
}

void BleHandshake::end(void)
{
}

bool BleHandshake::isConnected(void) {
  return this->connected;
}

void BleHandshake::setBatteryLevel(uint8_t level) {
  this->batteryLevel = level;
  if (hid != 0)
    this->hid->setBatteryLevel(this->batteryLevel);
}

//must be called before begin in order to set the name
void BleHandshake::setName(std::string deviceName) {
  this->deviceName = deviceName;
}

/**
 * @brief Sets the waiting time (in milliseconds) between multiple keystrokes in NimBLE mode.
 * 
 * @param ms Time in milliseconds
 */
void BleHandshake::setDelay(uint32_t ms) {
  this->_delay_ms = ms;
}

void BleHandshake::set_vendor_id(uint16_t vid) { 
  this->vid = vid; 
}

void BleHandshake::set_product_id(uint16_t pid) { 
  this->pid = pid; 
}

void BleHandshake::set_version(uint16_t version) { 
  this->version = version; 
}

uint8_t USBPutChar(uint8_t c);

void BleHandshake::onConnect(BLEServer* pServer) {
  this->connected = true;
}

void BleHandshake::onDisconnect(BLEServer* pServer) {
  this->connected = false;
  advertising->start();
}

void BleHandshake::onWrite(BLECharacteristic* me) {
  uint8_t* value = (uint8_t*)(me->getValue().c_str());
  (void)value;
  ESP_LOGI(LOG_TAG, "special keys: %d", *value);
}

void BleHandshake::delay_ms(uint64_t ms) {
  uint64_t m = esp_timer_get_time();
  if(ms){
    uint64_t e = (m + (ms * 1000));
    if(m > e){ //overflow
        while(esp_timer_get_time() > e) { }
    }
    while(esp_timer_get_time() < e) {}
  }
}
