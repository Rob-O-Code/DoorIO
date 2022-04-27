#pragma once

#include <Arduino.h>

#define PIR_PIN 14
#define PHOTO_PIN 4
#define BUZZ_PIN 25

class MyIO {
  public:
    MyIO() {};
    void init();
    bool isPIR() {return digitalRead(PIR_PIN);}

    void setBuzz(bool state);
  
};