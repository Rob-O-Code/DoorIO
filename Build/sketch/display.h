#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#include "user.h"

#define RA8875_INT     5
#define RA8875_CS      33
#define RA8875_RESET   32

enum Screen {
    Debug,
    None
};

class MyDisplay {
    public:
        MyDisplay() {};
        int init();
        int update();

        void setScreen(Screen screen);
        void setUser(unsigned int i, User* user);

        void setBacklight(uint8_t b);

    private:
        bool _init;
        Adafruit_RA8875 _tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);

        User* userArr[MAX_USERS];
        Screen curScreen = None;

        void DBG_draw();
        void DBG_drawBackground();
        void DBG_drawUserDevice(uint16_t x0, uint16_t y0, Device* device, std::string userName);
};