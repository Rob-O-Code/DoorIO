#include "IO.h"

int MyIO::init() {
	pinMode(PIR_PIN, INPUT);
	pinMode(PHOTO_PIN, INPUT);
	pinMode(BUZZ_PIN, OUTPUT);
	digitalWrite(BUZZ_PIN, LOW);

	if (!msa.begin()) {
		Serial.println("Failed to find MSA311 chip");
		return -1;
	}

	// Photoresistor initialization
	for (uint8_t i = 0; i < PHOTO_NUMREADINGS; i++) {
		photoCircular[i] = analogRead(PHOTO_PIN);
	}

	// Accelerometer initialization
	msa.setDataRate(MSA301_DATARATE_15_63_HZ);
	msa.setRange(MSA301_RANGE_2_G);
	msa.setResolution(MSA301_RESOLUTION_14);
	return 0;
}

void MyIO::setBuzz(bool state) {
	digitalWrite(BUZZ_PIN, state);
}

uint16_t MyIO::getPhotoSmooth() {
	photoCircular[photoCircularIndex] = 0;
	for (uint8_t i = 0; i < 10; i++) {
		photoCircular[photoCircularIndex] += analogRead(PHOTO_PIN) / 8;
	}
	if (++photoCircularIndex >= PHOTO_NUMREADINGS) photoCircularIndex = 0;

	uint32_t sum = 0;
	for (uint8_t i = 0; i < PHOTO_NUMREADINGS; i++) {
		sum += photoCircular[i];
	}
	sum /= PHOTO_NUMREADINGS;
	Serial.print("LOL: "); Serial.print(sum); Serial.print("   ");
	return (uint16_t) sum;
}

#define BACKLIGHT_MIN 10L
#define BACKLIGHT_MAX 255L
uint8_t MyIO::mapPhotoToBacklight(uint16_t val) {
	return (uint8_t) constrain(map(val, 0L, 5000L, BACKLIGHT_MIN, BACKLIGHT_MAX), BACKLIGHT_MIN, BACKLIGHT_MAX);
}

unsigned long MyIO::lastOpen() {
	//if (!init) return ULONG_MAX;
	msa.read();
	accelCircular[accelCircularIndex] = msa.z - accelPrevious;
	accelPrevious = msa.z;

	if (++accelCircularIndex >= ACCEL_NUMREADINGS) accelCircularIndex = 0;

	uint8_t count = 0;
	for (uint8_t i = 0; i < ACCEL_NUMREADINGS; i++) {
		if (abs(accelCircular[i]) >= ACCEL_THRESHOLD) count++;
	}

	if (count >= ACCEL_TRIGGERCOUNT) {
		timeLastOpen = millis();
		for (uint8_t i = 0; i < ACCEL_NUMREADINGS; i++) accelCircular[i] = 0;
		return 0;
	}
	return millis() - timeLastOpen;
}

/*
  static bool prevSense = digitalRead(PIR_PIN);
  bool sense = digitalRead(PIR_PIN);
  if (prevSense != sense) {
    Serial.print("Now ");
    Serial.println(sense ? "HIGH" : "LOW");
    prevSense = sense;
    if (sense) {
      digitalWrite(BUZZ_PIN, HIGH);
      timer = 20;
    }
  }
  int value = analogRead(PHOTO_PIN);
  //Serial.printf("%d\n", value);

  if (timer > 0) {
    timer--;
  } else {
    digitalWrite(BUZZ_PIN, LOW);
  }
  delay(10);*/