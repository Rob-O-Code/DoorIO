#pragma once

#include "user.h"
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"

#define RA8875_INT     5
#define RA8875_CS      33
#define RA8875_RESET   32

#define DEFAULT_BRIGHTNESS 50
#define FADE_AMOUNT 5

enum Screen {
    SCREEN_BTWAIT,
    SCREEN_REPORT,
    SCREEN_ALERT,
    SCREEN_DEBUG,
    SCREEN_NONE,
    SCREEN_WELCOME
};

enum BacklightFade {
    BACKLIGHT_IMMEDIATE,
    BACKLIGHT_FADE
};

class MyDisplay {
    public:
        MyDisplay() {};
        int init();
        int update();

        void setScreen(Screen screen);
        void setUser(unsigned int i, User* user);

        void setBacklight(uint8_t b, BacklightFade fade=BACKLIGHT_FADE);
        void fadeBacklight();

        bool isMissing();

    private:
        bool _init;
        int16_t currentBacklight = DEFAULT_BRIGHTNESS, targetBacklight = DEFAULT_BRIGHTNESS;
        Adafruit_RA8875 _tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);

        User* userArr[MAX_USERS];
        Screen curScreen = SCREEN_NONE;

        void WEL_draw();

        void BTW_draw();

        void RPT_draw();
        void RPT_drawBackground();
        void RPT_drawHeader(std::string message);
        void RPT_drawUserDevice(uint16_t x0, uint16_t y0, Device* device, User* user);

        void ALT_draw();
        void ALT_drawUserDevice(uint16_t x0, uint16_t y0, Device* device, User* user);

        void DBG_draw();
        void DBG_drawBackground();
        void DBG_drawUserDevice(uint16_t x0, uint16_t y0, Device* device, std::string userName);

};