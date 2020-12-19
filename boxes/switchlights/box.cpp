#include <Arduino.h>
//#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
//#include "encoderlib.h"
#include "communication.h"
#include "box.h"

// PIN ASSIGNMENTS
//
// DISPLAY devices
#define GEAR_TRANSIT 49
#define GEAR_SAFE0 47
#define GEAR_SAFE1 45
#define GEAR_SAFE2 43
#define FLAP_TRANSIT 41
#define FLAP_SET_APP 39
#define FLAP_SET_LAND 37


// CONTROL devices
#define EARTH0   15
#define FLAP0   16
#define FLAP1   17
#define FLAP2   18
#define LGEAR   19
#define EARTH1   20

struct tBoxState {
	bool lGear;
	bool flap0;
	bool flap1;
	bool flap2;
	bool gearTransit;
	bool gearSafe0;
	bool gearSafe1;
	bool gearSafe2;
	bool flapTransit;
	bool flapApp;
	bool flapLand;
};
struct tBoxState currentState;
struct tBoxState prevState;

void setStartState(void) {
	currentState.lGear = false;
	currentState.flap0 = false;
	currentState.flap1 = false;
	currentState.flap2 = false;
	currentState.gearTransit = false;
	currentState.gearSafe0 = false;
	currentState.gearSafe1 = false;
	currentState.gearSafe2 = false;
	currentState.flapTransit = false;
	currentState.flapApp = false;
	currentState.flapLand = false;
	prevState = currentState;
}

void showAnnunciators() {
	digitalWrite(GEAR_TRANSIT, currentState.gearTransit);
	digitalWrite(GEAR_SAFE0, currentState.gearSafe0);
	digitalWrite(GEAR_SAFE1, currentState.gearSafe1);
	digitalWrite(GEAR_SAFE2, currentState.gearSafe2);
	digitalWrite(FLAP_TRANSIT, currentState.flapTransit);
	digitalWrite(FLAP_SET_APP, currentState.flapApp);
	digitalWrite(FLAP_SET_LAND, currentState.flapLand);
}

void testIndicators() {
	digitalWrite(GEAR_TRANSIT, 1);
	delay(400);
	digitalWrite(GEAR_SAFE0, 1);
	delay(400);
	digitalWrite(GEAR_SAFE1, 1);
	delay(400);
	digitalWrite(GEAR_SAFE2, 1);
	delay(400);
	digitalWrite(FLAP_TRANSIT, 1);
	delay(400);
	digitalWrite(FLAP_SET_APP, 1);
	delay(400);
	digitalWrite(FLAP_SET_LAND, 1);
	delay(400);
	digitalWrite(GEAR_TRANSIT, 0);
	delay(400);
	digitalWrite(GEAR_SAFE0, 0);
	delay(400);
	digitalWrite(GEAR_SAFE1, 0);
	delay(400);
	digitalWrite(GEAR_SAFE2, 0);
	delay(400);
	digitalWrite(FLAP_TRANSIT, 0);
	delay(400);
	digitalWrite(FLAP_SET_APP, 0);
	delay(400);
	digitalWrite(FLAP_SET_LAND, 0);
	delay(400);
}

void boxSetup() {
	pinMode(LGEAR, INPUT_PULLUP);
	pinMode(FLAP0, INPUT_PULLUP);
	pinMode(FLAP1, INPUT_PULLUP);
	pinMode(FLAP2, INPUT_PULLUP);
	pinMode(EARTH0, OUTPUT);
	pinMode(EARTH1, OUTPUT);
	digitalWrite(EARTH0, 0);
	digitalWrite(EARTH1, 0);

	pinMode(GEAR_TRANSIT, OUTPUT);
	pinMode(GEAR_SAFE0, OUTPUT);
	pinMode(GEAR_SAFE1, OUTPUT);
	pinMode(GEAR_SAFE2, OUTPUT);
	pinMode(FLAP_TRANSIT, OUTPUT);
	pinMode(FLAP_SET_APP, OUTPUT);
	pinMode(FLAP_SET_LAND, OUTPUT);

	digitalWrite(GEAR_TRANSIT, 0);
	digitalWrite(GEAR_SAFE0, 0);
	digitalWrite(GEAR_SAFE1, 0);
	digitalWrite(GEAR_SAFE2, 0);
	digitalWrite(FLAP_TRANSIT, 0);
	digitalWrite(FLAP_SET_APP, 0);
	digitalWrite(FLAP_SET_LAND, 0);

	testIndicators();
}

void noConnectionActions(void) {
	fishForPlugin();
}

void setControl(char* device, char* value) {
	// act on message from XPlane by setting our systems state
	unsigned int v = atoi(value);
	Serial.print("MSG '");
	Serial.print(device);
	Serial.print("' = ");
	Serial.println(value);
	if (strcmp("GEAR_TRANSIT", device) == 0) {
		currentState.gearTransit = (v == 1);
	} 
	if (strcmp("GEAR_SAFE0", device) == 0) {
		currentState.gearSafe0 = (v == 1);
	} 
	if (strcmp("GEAR_SAFE1", device) == 0) {
		currentState.gearSafe1 = (v == 1);
	} 
	if (strcmp("GEAR_SAFE2", device) == 0) {
		currentState.gearSafe2 = (v == 1);
	} 
	if (strcmp("FLAP_TRANSIT", device) == 0) {
		currentState.flapTransit = (v == 1);
	} 
	if (strcmp("FLAP_APP", device) == 0) {
		currentState.flapApp = (v == 1);
	} 
	if (strcmp("FLAP_LAND", device) == 0 ) {
		currentState.flapLand = (v == 1);
	}
}

int bouncer = 0;
void markStart(int delayTime) {
	bouncer = delayTime;
}

void checkSwitches(void) {
	if (bouncer-- <= 0) {
		bouncer = 0;
		bool v = false;
		v = digitalRead(LGEAR);
		if (v != currentState.lGear) {
			markStart(90);
			currentState.lGear = v;
		}
		v = digitalRead(FLAP0);
		if (v != currentState.flap0) {
			markStart(50);
			currentState.flap0 = v;
		}
		v = digitalRead(FLAP1);
		if (v != currentState.flap1) {
			markStart(50);
			currentState.flap1 = v;
		}
		v = digitalRead(FLAP2);
		if (v != currentState.flap2) {
			markStart(50);
			currentState.flap2 = v;
		}
	}
}

void sendChanges() {
	if (currentState.lGear != prevState.lGear) {
		sendDataTypeBool("LGEAR", currentState.lGear);
		prevState.lGear = currentState.lGear;
	}
	if (currentState.flap0 != prevState.flap0) {
		if (!currentState.flap0)
			sendDataTypeFloat("FLAP_UP", (double)0.0);
		prevState.flap0 = currentState.flap0;
	}
	if (currentState.flap1 != prevState.flap1) {
		if (!currentState.flap1) {
			if (!prevState.flap0) {
				sendDataTypeFloat("FLAP_DOWN", (double)0.5);
			} else {
				sendDataTypeFloat("FLAP_UP", (double)0.5);
			}
		}
		prevState.flap1 = currentState.flap1;
	}
	if (currentState.flap2 != prevState.flap2) {
		if (!currentState.flap2)
			sendDataTypeFloat("FLAP_DOWN", (double)1.0);
		prevState.flap2 = currentState.flap2;
	}
}

void boxMainLoop(void) {
	checkSwitches();
	sendChanges();
	showAnnunciators();
}
