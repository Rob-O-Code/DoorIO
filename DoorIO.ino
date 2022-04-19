///////////////////////////////////////////////////////
// Robbie Oleynick, Nate Reppucci, Cameron Pelletier //
///////////////////////////////////////////////////////

#include <Arduino.h>
#include "Bluetooth.h"
#define PIR_PIN 14
#define PHOTO_PIN 4
#define BUZZ_PIN 25

MyBluetooth myBluetooth;

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(PHOTO_PIN, INPUT);
  pinMode(BUZZ_PIN, OUTPUT);

  myBluetooth.init();
}

int timer = 0;

void loop() {
  myBluetooth.testScan();
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
}
