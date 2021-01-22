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
	int  flapPos;
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
	currentState.flapPos = 0;
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
	delay(100);
	digitalWrite(GEAR_SAFE0, 1);
	delay(100);
	digitalWrite(GEAR_SAFE1, 1);
	delay(100);
	digitalWrite(GEAR_SAFE2, 1);
	delay(100);
	digitalWrite(FLAP_TRANSIT, 1);
	delay(100);
	digitalWrite(FLAP_SET_APP, 1);
	delay(100);
	digitalWrite(FLAP_SET_LAND, 1);
	delay(100);
	digitalWrite(GEAR_TRANSIT, 0);
	delay(100);
	digitalWrite(GEAR_SAFE0, 0);
	delay(100);
	digitalWrite(GEAR_SAFE1, 0);
	delay(100);
	digitalWrite(GEAR_SAFE2, 0);
	delay(100);
	digitalWrite(FLAP_TRANSIT, 0);
	delay(100);
	digitalWrite(FLAP_SET_APP, 0);
	delay(100);
	digitalWrite(FLAP_SET_LAND, 0);
	delay(100);
}

void interpretFlapState(void) {
	if (currentState.flap0) {
		currentState.flapPos = 0;
	} else if (currentState.flap1) {
		currentState.flapPos = 1;
	} if (currentState.flap2) {
		currentState.flapPos = 2;
	}
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

	currentState.flap0 = !digitalRead(FLAP0);
	currentState.flap1 = !digitalRead(FLAP1);
	currentState.flap2 = !digitalRead(FLAP2);
	interpretFlapState();
	currentState.gearSafe0 = digitalRead(LGEAR);
	currentState.gearSafe1 = digitalRead(LGEAR);
	currentState.gearSafe2 = digitalRead(LGEAR);

	testIndicators();
}

void noConnectionActions(void) {
	fishForPlugin();
	testIndicators();
}

void setControl(char* device, char* value) {
	// act on message from XPlane by setting our systems state
	unsigned int v = atoi(value);
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
	interpretFlapState();
}

int bouncer = 0;
void markStart(int delayTime) {
	bouncer = delayTime;
}

void checkSwitches(void) {
	if (bouncer-- <= 0) {
		bouncer = 0;
		bool v = false;
		v = !digitalRead(LGEAR);
		if (v != currentState.lGear) {
			markStart(10);
			currentState.lGear = v;
		}
		currentState.flap0 = false;
		currentState.flap1 = false;
		currentState.flap2 = false;
		v = !digitalRead(FLAP0);
		if (v != currentState.flap0) {
			if (v) {
				markStart(50);
				currentState.flap0 = v;
			}
		}
		v = !digitalRead(FLAP1);
		if (v != currentState.flap1) {
			if (v) {
				markStart(50);
				currentState.flap1 = v;
			}
		}
		v = !digitalRead(FLAP2);
		if (v != currentState.flap2) {
			if (v) {
				markStart(50);
				currentState.flap2 = v;
			}
		}
	}
	interpretFlapState();
}

void sendChanges() {
	if (currentState.lGear != prevState.lGear) {
		sendDataTypeBool("LGEAR", currentState.lGear);
		prevState.lGear = currentState.lGear;
	}
	if (currentState.flapPos != prevState.flapPos) {
		if (currentState.flapPos == 0) {
			if (prevState.flapPos == 1) {
				sendDataTypeBool("FLAP_UP", 1);
			} else if (prevState.flapPos == 2) {
				sendDataTypeBool("FLAP_UP", 1);
				sendDataTypeBool("FLAP_UP", 1);
			}
		} else if (currentState.flapPos == 1) {
			if (prevState.flapPos == 0) {
				sendDataTypeBool("FLAP_DOWN", 1);
			} else if (prevState.flapPos == 2) {
				sendDataTypeBool("FLAP_UP", 1);
			}
		} else if (currentState.flapPos == 2) {
			if (prevState.flapPos == 1) {
				sendDataTypeBool("FLAP_DOWN", 1);
			} else if (prevState.flapPos == 0) {
				sendDataTypeBool("FLAP_DOWN", 1);
				sendDataTypeBool("FLAP_DOWN", 1);
			}
		}
		prevState.flapPos = currentState.flapPos;
	}
}

void boxMainLoop(void) {
	checkSwitches();
	sendChanges();
	showAnnunciators();
}
