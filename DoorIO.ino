///////////////////////////////////////////////////////
// Robbie Oleynick, Nate Reppucci, Cameron Pelletier //
///////////////////////////////////////////////////////

#include "bluetooth.h"
#include "display.h"
#include "IO.h"

#define _TASK_SLEEP_ON_IDLE_RUN
#define _TASK_PRIORITY
#define _TASK_WDT_IDS
#define _TASK_TIMECRITICAL
#include "TaskScheduler.h"

Scheduler tasks, hp_tasks;
void pollSensors();
Task T_pollSensors(TASK_MILLISECOND * 50, TASK_FOREVER, &pollSensors, &hp_tasks, false);
void pollBluetooth();
Task T_pollBluetooth(TASK_SECOND * 10, TASK_FOREVER, &pollBluetooth, &tasks, false);
void backlightDim();
Task T_backlightDim(TASK_MILLISECOND * 50, TASK_FOREVER, &backlightDim, &hp_tasks, false);

//MyInternet myInternet;
MyBluetooth myBluetooth;
MyDisplay myDisplay;
MyIO myIO;
Registry registry;


User *robbie, *nate, *cam;


void setup() {
	Serial.begin(115200);

	/* users and devices example */
	
	robbie = new User("Robbie");
	nate = new User("Nate");
	cam = new User("Cam");
	Device* flip52 = new Device(BLEAddress("b8:f6:53:92:a9:a3"), "Flip 5.2", -55);
	robbie->addDevice(flip52);
	nate->addDevice(flip52);
	registry.addDevice(flip52);
	Device* nateHeadphones = new Device(BLEAddress("94:db:56:02:61:6b"), "Nate's Headpho.", -55);
	nate->addDevice(nateHeadphones);
	registry.addDevice(nateHeadphones);
	Device* keyboard = new Device(BLEAddress("28:18:78:17:24:3b"), "BT Keyboard", -55);
	cam->addDevice(keyboard);
	registry.addDevice(keyboard);
	Device* flip51 = new Device(BLEAddress("5c:fb:7c:cc:df:e8"), "Flip 5.1", -55);
	robbie->addDevice(flip51);
	registry.addDevice(flip51);

	if (myIO.init()) {
		Serial.println("Error initializing IO");
		while (1);
	}
	if (myDisplay.init()) {
		Serial.println("Error initializing display");
		while (1);
	};
	myBluetooth.init(&registry);


	tasks.setHighPriorityScheduler(&hp_tasks);
	T_backlightDim.restart();
	T_pollSensors.restart();

	myDisplay.setScreen(SCREEN_WELCOME);
	myDisplay.update();
	delay(500UL);

	myDisplay.setUser(0, robbie);
	myDisplay.setUser(1, nate);
	myDisplay.setUser(2, cam);
}

void loop() {
	tasks.execute();
	/* test code 
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
	*/
}

bool isStandby = false;
void pollSensors() {
	//Serial.println("--- TASK BEGIN: pollSensors() ---");

	// PIR Sensor: Is there someone at the door?
	if (myIO.lastPIR() > 5000UL) {
		Serial.println("No one here..."); 
		standby();
		delay(500UL);
		return;
	}
	isStandby = false;

	// Someone is here, so ensure that Bluetooth is polling
	if (!T_pollBluetooth.isEnabled()) {
		newSession();
		T_pollBluetooth.restart();
	}

	// Backlight: How bright should the display be?
	uint8_t b = myIO.mapPhotoToBacklight(myIO.getPhotoSmooth());
	myDisplay.setBacklight(b);

	// Accelerometer: Did someone open the door?
	if (myIO.lastOpen() < 50UL) {
		//Serial.println("Trigger");
		egress();
	}

	//Serial.println("--- TASK COMPLETE: pollSensors() ---");
}

void newSession() {
	myBluetooth.clearRegistry();
}

void pollBluetooth() {
	Serial.println("--- TASK BEGIN: pollBluetooth() ---");
	myDisplay.setScreen(SCREEN_BTWAIT);
	myDisplay.update();
	delay(500UL);
	myBluetooth.scan();
	myDisplay.setScreen(SCREEN_REPORT);
	myDisplay.update();
	Serial.println("--- TASK COMPLETE: pollBluetooth() ---");
}

void backlightDim() {
	myDisplay.fadeBacklight();
}

void standby() {
	myIO.getPhotoSmooth();
	if (isStandby) return;
	T_pollBluetooth.abort();
	myDisplay.setBacklight(0, BACKLIGHT_IMMEDIATE);
	myDisplay.setScreen(SCREEN_NONE);
	myDisplay.update();
	isStandby = true;
}

void egress() {
	T_pollBluetooth.abort();
	myIO.setBuzz(HIGH);
	delay(50UL);
	myIO.setBuzz(LOW);
	
	delay(200UL);

	if (myDisplay.isMissing()) {
		myDisplay.setScreen(SCREEN_ALERT);
		myDisplay.update();
		for (uint8_t i = 0; i < 3; i++) {
			myIO.setBuzz(HIGH);
			delay(300UL);
			myIO.setBuzz(LOW);
			delay(50UL);
		}
		delay(2000UL);
	} else Serial.println("All good!");

	isStandby = false;
	standby();
	delay(5000UL);
}