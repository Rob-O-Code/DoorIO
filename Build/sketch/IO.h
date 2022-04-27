#pragma once

#include <Arduino.h>
#include "Adafruit_MSA301.h"

#define PIR_PIN 14
#define PHOTO_PIN 4
#define BUZZ_PIN 25

#define PHOTO_NUMREADINGS 50

#define ACCEL_NUMREADINGS 20
#define ACCEL_THRESHOLD 50
#define ACCEL_TRIGGERCOUNT 10

class MyIO {
	public:
		MyIO() {};
		int init();

		bool isPIR() {return digitalRead(PIR_PIN);}
		unsigned long lastPIR();

		void setBuzz(bool state);

		uint16_t getPhotoSmooth();
		uint8_t mapPhotoToBacklight(uint16_t val);

		unsigned long lastOpen();

	private:
		Adafruit_MSA311 msa;

		unsigned long timeLastPIR;

		uint8_t photoCircularIndex = 0;
		int16_t photoCircular[PHOTO_NUMREADINGS];
		
		uint8_t accelCircularIndex = 0;
		int16_t accelCircular[ACCEL_NUMREADINGS];
		int16_t accelPrevious;
		unsigned long timeLastOpen;
};
