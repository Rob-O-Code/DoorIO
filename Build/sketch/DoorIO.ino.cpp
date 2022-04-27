#include <Arduino.h>
#line 1 "c:\\Users\\rpole\\Documents\\DoorIO\\DoorIO.ino"
///////////////////////////////////////////////////////
// Robbie Oleynick, Nate Reppucci, Cameron Pelletier //
///////////////////////////////////////////////////////

#include "bluetooth.h"
#include "display.h"
#include "IO.h"
//#include "internet.h"

//MyInternet myInternet;
MyBluetooth myBluetooth;
MyDisplay myDisplay;
MyIO myIO;
Registry registry;

User *robbie, *nate;

#line 18 "c:\\Users\\rpole\\Documents\\DoorIO\\DoorIO.ino"
void setup();
#line 54 "c:\\Users\\rpole\\Documents\\DoorIO\\DoorIO.ino"
void loop();
#line 18 "c:\\Users\\rpole\\Documents\\DoorIO\\DoorIO.ino"
void setup() {
	Serial.begin(115200);

	/* users and devices example */
	

	robbie = new User("Robbie");
	Device* flip52 = new Device(BLEAddress("b8:f6:53:92:a9:a3"), "Flip 5.2", -58);
	robbie->addDevice(flip52);
	registry.addDevice(flip52);
	Device* nateHeadphones = new Device(BLEAddress("94:db:56:02:61:6b"), "Nate's Headphones", -58);
	robbie->addDevice(nateHeadphones);
	registry.addDevice(nateHeadphones);
	nate = new User("Nate");
	Device* flip51 = new Device(BLEAddress("5c:fb:7c:cc:df:e8"), "Flip 5.1", -58);
	nate->addDevice(flip51);
	registry.addDevice(flip51);
	/* */

	myIO.init();
	if (myDisplay.init()) {
		Serial.println("Error initializing display");
		while (1);
	};
	myBluetooth.init(&registry);

	//myInternet.init();
	/* display example */
	myDisplay.update();
	delay(500UL);
	myDisplay.setUser(0, robbie);
	myDisplay.setUser(1, nate);
	myDisplay.setScreen(Debug);
	/* */
}

void loop() {
	Serial.println("loop!");
	delay(1000UL);
	/* test code */
	while (!myIO.isPIR());
		registry.clearInfo();
		myDisplay.update();
		for (unsigned int i = 50; i <= 255; i++) {
		myDisplay.setBacklight((uint8_t) i);
		delay(2UL);
	}
	myBluetooth.testScan();
	myIO.setBuzz(HIGH);
	myDisplay.update();
	delay(100UL);
	myIO.setBuzz(LOW);
	delay(3000UL);
	for (unsigned int i = 255; i >= 50; i--) {
		myDisplay.setBacklight((uint8_t) i);
		delay(2UL);
	}
	delay(2000UL);
}

