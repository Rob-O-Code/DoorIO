#pragma once

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "user.h"

class MyBluetooth {
	public:
		MyBluetooth() {};
		void init(Registry* registry);
		void clearRegistry();
		void scan();
		Registry* getRegistry() {return _registry;}

	private:
		static const int scanTime = 3;
		BLEScan* pBLEScan;
		Registry* _registry;
};

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
	friend class MyBluetooth;
	Registry* _registry;

	void onResult(BLEAdvertisedDevice advertisedDevice) {
		//Serial.printf("Advertised Device: %s ", advertisedDevice.toString().c_str());
		Device* device = _registry->getDevice(advertisedDevice.getAddress());
		if (device != nullptr) {
			device->recordRSSI(advertisedDevice.getRSSI());
			Serial.println("MATCH!");
		}
		//Serial.println("");
	}
};
