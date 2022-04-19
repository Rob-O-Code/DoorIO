#ifndef IO_H_
#define IO_H_

#include <Arduino.h>

#define PIR_PIN 14
#define PHOTO_PIN 4
#define BUZZ_PIN 25

class MyIO {
  public:
    MyIO() {};
    void init();
  
};

#endif
