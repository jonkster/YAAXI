#include <Arduino.h>
#include "switches.h"
#include "Communication.h"
#include "Xplane.h"
#include "config.h"

#define MAX_TASKS 10
#define LAST_ADJUST_SECS 1
unsigned long timeNow[MAX_TASKS];

void setup(){
	Serial.begin(115200);
	initSwitches();
	setupEthernet();
	addGetXPRegs();
	addSetXPRegs();
	for (int i = 0; i < MAX_TASKS; i++) {
		timeNow[i] = millis();
	}
	testLeds(100);
}

void actOnBoxChanges() {
	pushBoxToXP();
}

void lookForXPChanges(const int delayRequestingXPData) {
	unsigned long t = millis();
	if (t > (timeNow[1] + delayRequestingXPData)) {
		queueNextXPRequest();
		timeNow[1] = t;
	}
}

bool firstTime = true;
void loop(){

	checkSwitches();

	if (! knowsXPAddr()) {
		unsigned long t = millis();
		if (t > (timeNow[0] + 500)) {
			showWaiting();
			timeNow[0] = t;
		}
		bool boxChanged = checkAnySwitchesChanged();
		if (actOnRotations()) {
			boxChanged = true;
		}
		lookForXPChanges(500);
		updateComms();
	} else {
		if (firstTime) {
			firstTime = false;
			lightsAll(false);
		}

		bool boxChanged = checkAnySwitchesChanged();

		if (actOnRotations()) {
			boxChanged = true;
		}
		if (boxChanged) {
			actOnBoxChanges();
			unloadRxQueue();
		} else {
			actOnXPResponses();
		}

		lookForXPChanges(500);
		updateComms();
	}
}
