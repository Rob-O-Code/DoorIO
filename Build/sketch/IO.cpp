#include "IO.h"

void MyIO::init() {
    pinMode(PIR_PIN, INPUT);
    pinMode(PHOTO_PIN, INPUT);
    pinMode(BUZZ_PIN, OUTPUT);
    digitalWrite(BUZZ_PIN, LOW);
}

void MyIO::setBuzz(bool state) {
  digitalWrite(BUZZ_PIN, state);
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