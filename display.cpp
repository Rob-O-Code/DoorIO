#include "display.h"

int MyDisplay::init() {
    if (_init) return 1;
    if (!_tft.begin(RA8875_800x480)) {
        Serial.println("LCD not found!");
        return false;
    }
    _tft.displayOn(true);
    _tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
    _tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
    _tft.PWM1out(DEFAULT_BRIGHTNESS);

    _tft.fillScreen(RA8875_BLACK);

    for (unsigned int i = 0; i < MAX_USERS; i++) {
        userArr[i] = nullptr;
    }

    _init = true;
    return 0;
}

int MyDisplay::update() {
    switch (curScreen) {
        case Debug:
            DBG_draw();
            break;
        default:
            _tft.fillScreen(RA8875_CYAN);
            _tft.textMode();
            _tft.textSetCursor(10, 10);
            _tft.textEnlarge(3);
            _tft.textColor(RA8875_BLUE, RA8875_GREEN);
            _tft.textWrite("Hello, world!");
            _tft.graphicsMode();
    }
    return 0;
}

void MyDisplay::setScreen(Screen screen) {
    curScreen = screen;
}

void MyDisplay::setUser(unsigned int i, User* user) {
    if (i >= MAX_USERS) return;
    userArr[i] = user;
}

void MyDisplay::setBacklight(uint8_t b, BacklightFade fade) {
    if (fade == BACKLIGHT_FADE) {
        targetBacklight = b;
        fadeBacklight();
    } else if (fade == BACKLIGHT_IMMEDIATE) {
        _tft.PWM1out(targetBacklight = b);
    }
}

void MyDisplay::fadeBacklight() {
    if (currentBacklight == targetBacklight) return;
    if (currentBacklight - FADE_AMOUNT > targetBacklight) {
        _tft.PWM1out(targetBacklight = targetBacklight - FADE_AMOUNT);
    } else if (currentBacklight + FADE_AMOUNT < targetBacklight) {
        _tft.PWM1out(targetBacklight = targetBacklight + FADE_AMOUNT);
    }
    _tft.PWM1out(currentBacklight = targetBacklight);
}

void MyDisplay::DBG_draw() {
    DBG_drawBackground();
    int y = 8;
    for (unsigned int i = 0; i < MAX_USERS; i++) {
        if (userArr[i] == nullptr) break;
        User* user = userArr[i];
        Device** devices = user->getDevices();
        for (unsigned int j = 0; j < MAX_DEVICES; j++) {
            if (devices[j] == nullptr) break;
            DBG_drawUserDevice(8, y, devices[j], user->getName());
            y += 32;
        }
    }
}

void MyDisplay::DBG_drawBackground() {
    _tft.fillScreen(RA8875_BLACK);
}

void MyDisplay::DBG_drawUserDevice(uint16_t x0, uint16_t y0, Device* device, std::string userName) {
    _tft.textMode();
    _tft.textSetCursor(x0, y0);
    if (device->getRecordedRSSI() > device->getRSSIThresh()) {
        _tft.textColor(RA8875_BLACK, RA8875_GREEN);
    } else {
        _tft.textColor(RA8875_WHITE, RA8875_BLACK);
    }
    _tft.textEnlarge(1);
    _tft.textWrite(userName.c_str());
    _tft.textSetCursor(x0 + 150, y0);
    _tft.textWrite(device->getName().c_str());
    _tft.textSetCursor(x0 + 400, y0);
    _tft.textWrite(device->getAddress().toString().c_str());
    if (device->getDetected()) {
        char str[6];
        itoa(device->getRecordedRSSI(), str, 10);
        _tft.textSetCursor(x0 + 700, y0);
        _tft.textWrite(str);
    }
    _tft.graphicsMode();
    Serial.println("Printed device!");
}