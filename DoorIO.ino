///////////////////////////////////////////////////////
// Robbie Oleynick, Nate Reppucci, Cameron Pelletier //
///////////////////////////////////////////////////////

#include "bluetooth.h"

BleHandshake bleHandshake;

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>


BLEServer* pServer;

void setup() {
  Serial.begin(115200);
  Serial.println("Beginning...");
  pServer = bleHandshake.begin();
  Serial.println("Done!");

  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.printf("Connected count: %d\n", pServer->getConnectedCount());
  
  std::map<uint16_t, conn_status_t> myMap = pServer->getPeerDevices(false);
  for (const auto& n : myMap) {
    Serial.print("["); Serial.print(n.first); Serial.print("] = "); Serial.print(n.second.mtu); Serial.println(";");
  }
  Serial.println("clinet = true");
  myMap = pServer->getPeerDevices(true);
  for (const auto& n : myMap) {
    Serial.print("["); Serial.print(n.first); Serial.print("] = "); Serial.print(n.second.mtu); Serial.println(";");
  }
  
  delay(500);
}
