#pragma once

#include <Arduino.h>
#include <BLEAddress.h>

#define MAX_USERS 5
#define MAX_DEVICES 10
#define MIN_RSSI -256

class Device {
    public:
        Device(BLEAddress bleAddress, std::string name, int rssiThresh);

        std::string getName();
        int getRSSIThresh();
        BLEAddress getAddress();
        bool getDetected() {return _detected;}
        bool getPresent() {return _present;}
        int getRecordedRSSI() {return _recordedRSSI;}

        void setName(std::string name);
        void setRSSIThresh(int rssiThresh);
        void recordRSSI(int rssi);
        void clearInfo();

    private:
        BLEAddress _bleAddress = BLEAddress((uint8_t*)"\0\0\0\0\0\0");
        std::string _name;
        int _rssiThresh = 0;

        bool _detected = false;
        bool _present = false;
        int _recordedRSSI = MIN_RSSI;
};

class User {
    public:
        User(std::string name);

        std::string getName();
        Device** getDevices();
        bool getPresent();

        int addDevice(Device* device);
    private:
        std::string _name;
        Device* _devices[MAX_DEVICES];
};

class Registry {
    public:
        Registry();
        int addDevice(Device* device);

        Device* getDevice(BLEAddress address);

        void clearInfo();

    private:
        Device* devices[MAX_USERS * MAX_DEVICES];
};