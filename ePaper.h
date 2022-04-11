#ifndef EPAPER_H_
#define EPAPER_H_

#include <Arduino.h>

#define DEBUG 1

class ePaper {
  public:
    int init(void);
  private:
    bool isInit;
};

#endif
