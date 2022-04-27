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
        case SCREEN_WELCOME:
            WEL_draw();
            break;
        case SCREEN_BTWAIT:
            BTW_draw();
            break;
        case SCREEN_REPORT:
            RPT_draw();
            break;
        case SCREEN_ALERT:
            ALT_draw();
            break;
        case SCREEN_DEBUG:
            DBG_draw();
            break;
        case SCREEN_NONE:
        default:
            _tft.fillScreen(RA8875_BLACK);
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

// TODO: Move somewhere else
bool MyDisplay::isMissing() {
    for (unsigned int i = 0; i < MAX_USERS; i++) {
        User* user = userArr[i];
        if (user == nullptr) continue; // no user
        if (user->getPresent() == false) continue; // not present
        for (unsigned int j = 0; j < MAX_DEVICES; j++) {
            if (user->getDevices()[j] == nullptr) continue; // no device
            if (user->getDevices()[j]->getPresent() == false) { // device not present
                return true;
            }
        }
    }
    return false;
}

void MyDisplay::WEL_draw() {
    _tft.fillScreen(RA8875_CYAN);
    _tft.textMode();
    _tft.textSetCursor(10, 10);
    _tft.textEnlarge(3);
    _tft.textColor(RA8875_BLUE, RA8875_GREEN);
    _tft.textWrite("Hello, world!");
    _tft.graphicsMode();
}

void MyDisplay::BTW_draw() {
    _tft.fillScreen(RA8875_BLUE);
    _tft.textMode();
    _tft.textColor(RA8875_WHITE, RA8875_BLUE);
    _tft.textEnlarge(1);
    _tft.textSetCursor(20, 180);
    _tft.textWrite("Scanning for Bluetooth devices...");
    _tft.graphicsMode();
}

void MyDisplay::RPT_draw() {
    RPT_drawBackground();

    unsigned int users = 0;
    std::string message = "Hello, ";
    for (unsigned int i = 0; i < MAX_USERS; i++) {
        if (userArr[i] == nullptr) continue;
        if (userArr[i]->getPresent()) {
            if (users != 0) message += " & " + userArr[i]->getName();
            else message += userArr[i]->getName();
            users++;
        }
    }
    if (users == 0) message += "there";
    message += "!";
    RPT_drawHeader(message);

    int y = 8 + 64;
    for (unsigned int i = 0; i < MAX_USERS; i++) {
        if (userArr[i] == nullptr) continue;
        User* user = userArr[i];
        Device** devices = user->getDevices();
        for (unsigned int j = 0; j < MAX_DEVICES; j++) {
            if (devices[j] == nullptr) continue;
            RPT_drawUserDevice(8, y, devices[j], user);
            y += 32;
        }
    }
}

#define RPT_GREY 0b0010000100000100
void MyDisplay::RPT_drawBackground() {
    _tft.fillScreen(RPT_GREY);
    _tft.drawFastHLine(4, 64+3, _tft.width() - 8, RA8875_WHITE);
    _tft.drawFastHLine(4, 64+4, _tft.width() - 8, RA8875_WHITE);
    _tft.drawFastHLine(4, 64+5, _tft.width() - 8, RA8875_WHITE);
}

void MyDisplay::RPT_drawHeader(std::string message) {
    _tft.textMode();
    _tft.textEnlarge(2);
    _tft.textSetCursor(8, 8);
    _tft.textTransparent(RA8875_WHITE);
    _tft.textWrite(message.c_str());
    _tft.graphicsMode();
}

void MyDisplay::RPT_drawUserDevice(uint16_t x0, uint16_t y0, Device* device, User* user) {
    _tft.textMode();
    _tft.textSetCursor(x0, y0);
    if (device->getRecordedRSSI() > device->getRSSIThresh()) {
        _tft.textTransparent(0b1100011111111000);
    } else if (user->getPresent()) {
        _tft.textColor(RA8875_RED, RA8875_WHITE); //0b1111111000011000
    } else {
        _tft.textTransparent(RA8875_WHITE);
    }
    _tft.textEnlarge(1);
    _tft.textWrite(user->getName().c_str());
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
}

void MyDisplay::ALT_draw() {
    Serial.println("START ALERT SCREEN");
    _tft.fillScreen(RA8875_RED);
    uint16_t y = 160;
    for (unsigned int i = 0; i < MAX_USERS; i++) {
        User* user = userArr[i];
        if (user == nullptr) continue;
        if (user->getPresent() == false) continue;
        for (unsigned int j = 0; j < MAX_DEVICES; j++) {
            Device* device = user->getDevices()[j];
            if (device == nullptr) continue;
            if (device->getPresent() == false) {
                ALT_drawUserDevice(8, y, device, user);
                y += 32;
            }
        }
    }
    Serial.println("FINISH ALERT SCREEN");
}

void MyDisplay::ALT_drawUserDevice(uint16_t x0, uint16_t y0, Device* device, User* user) {
    _tft.textMode();
    _tft.textTransparent(RA8875_WHITE);
    _tft.textSetCursor(x0, y0);
    _tft.textEnlarge(1);
    std::string message = "> " + user->getName() + " forgot " + device->getName();
    _tft.textWrite(message.c_str());
    _tft.graphicsMode();
}

void MyDisplay::DBG_draw() {
    DBG_drawBackground();
    int y = 8;
    for (unsigned int i = 0; i < MAX_USERS; i++) {
        if (userArr[i] == nullptr) continue;
        User* user = userArr[i];
        Device** devices = user->getDevices();
        for (unsigned int j = 0; j < MAX_DEVICES; j++) {
            if (devices[j] == nullptr) continue;
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
}