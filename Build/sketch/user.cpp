#include "user.h"

//////// DEVICE METHODS ////////

Device::Device(BLEAddress bleAddress, std::string name, int rssiThresh) {
    _bleAddress = bleAddress;
    _name = name;
    _rssiThresh = rssiThresh;
}

std::string Device::getName() {
    return _name;
}

int Device::getRSSIThresh() {
    return _rssiThresh;
}

BLEAddress Device::getAddress() {
    return _bleAddress;
}

void Device::setName(std::string name) {
    _name = name;
}

void Device::setRSSIThresh(int rssiThresh) {
    _rssiThresh = rssiThresh;
}

void Device::recordRSSI(int rssi) {
    _recordedRSSI = rssi;
    _detected = true;
}

void Device::clearInfo() {
    _recordedRSSI = MIN_RSSI;
    _detected = false;
}

//////// USER METHODS ////////

User::User(std::string name) {
    _name = name;
    for (unsigned int i = 0; i < MAX_DEVICES; i++) {
        _devices[i] = nullptr;
    }
}

std::string User::getName() {
    return _name;
}

Device** User::getDevices() {
    return _devices;
}

int User::addDevice(Device* device) {
    for (unsigned int i = 0; i < MAX_DEVICES; i++) {
        if (_devices[i] != nullptr) continue;
        _devices[i] = device;
        return 0;
    }
    Serial.println("ERROR (User::addDevice) Unable to add device to user");
    return -1;
}

//////// REGISTRY METHODS ////////

Registry::Registry() {
    for (unsigned int i = 0; i < MAX_USERS * MAX_DEVICES; i++) {
        devices[i] = nullptr;
    }
}

// TODO: Make better
int Registry::addDevice(Device* device) {
    for (unsigned int i = 0; i < MAX_USERS * MAX_DEVICES; i++) {
        if (devices[i] != nullptr) continue;
        devices[i] = device;
        return 0;
    }
    return -1;
}

Device* Registry::getDevice(BLEAddress address) {
    for (unsigned int i = 0; i < MAX_USERS * MAX_DEVICES; i++) {
        if (devices[i] == nullptr) continue;
        Serial.printf("(%s) ", devices[i]->getAddress().toString());
        if (address.equals(devices[i]->getAddress()))
        return devices[i];
    }
    return nullptr;
}

void Registry::clearInfo() {
    for (unsigned int i = 0; i < MAX_USERS * MAX_DEVICES; i++) {
        if (devices[i] != nullptr)
        devices[i]->clearInfo();
    }
}