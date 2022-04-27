#include "bluetooth.h"

void MyBluetooth::init(Registry* registry) {
	_registry = registry;
	BLEDevice::init("");
	pBLEScan = BLEDevice::getScan(); //create new scan
	MyAdvertisedDeviceCallbacks* mAC = new MyAdvertisedDeviceCallbacks();
	mAC->_registry = registry;
	pBLEScan->setAdvertisedDeviceCallbacks(mAC);
	pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
	pBLEScan->setInterval(100);
	pBLEScan->setWindow(99);  // less or equal setInterval value
}

void MyBluetooth::clearRegistry() {
	_registry->clearInfo();
}

void MyBluetooth::scan() {
	BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
	//Serial.print("Devices found: ");
	//Serial.println(foundDevices.getCount());
	//Serial.println("Scan done!");
	pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
	//delay(2000);
}
