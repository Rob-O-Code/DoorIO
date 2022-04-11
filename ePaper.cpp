#include "ePaper.h"

int ePaper::init() {
  if (DEBUG) Serial.println("---Begin ePaper Display initialization...---");

  isInit = true;
  if (DEBUG) Serial.println("---ePaper Display initialized!---");
  return 0;
}

int ePaper::display() {
  if (DEBUG) Serial.println("---Begin ePaper display---");

  if (DEBUG) Serial.println("---ePaper display complete!---");
  return 0;
}
