#include <Arduino.h>
#include "communication.h"
#include "box.h"

// PIN ASSIGNMENTS
#define LED0 LED_BUILTIN
#define SWITCH0  12

struct tBoxState {
	bool switch0;
	bool led0;
};
struct tBoxState currentState;
struct tBoxState prevState;


void setStartState(void) {
	currentState.led0 = 0;
	currentState.switch0 = digitalRead(SWITCH0);
	digitalWrite(LED0, currentState.led0);

}

void boxSetup() {
	pinMode(LED0, OUTPUT);
	digitalWrite(LED0, 1);
	pinMode(SWITCH0, INPUT_PULLUP);
	delay(200);
	digitalWrite(LED0, 0);

}

void noConnectionActions(void) {
	delay(500);
	fishForPlugin();
}

void setControl(char* device, char* value) {
	// act on message from XPlane by setting our systems state
	unsigned long v = atol(value);
	if (strcmp("LED0", device) == 0) {
		currentState.led0 = v;	
	} else {
		if (DEBUG_MODE) {
			Serial.println("could not find device!");
		}
	}
}


bool sendAnyChanges() {
	// send any changes we know about to XPlane
	bool changes = false;
	if (currentState.switch0 != prevState.switch0) {
		sendDataTypeLong("SWITCH0", currentState.switch0);   
		changes = true;
	}
	return changes;
}

void clearChanges(void) {
	prevState = currentState;
}

void boxMainLoop(void) {
	currentState.switch0 = digitalRead(SWITCH0);
	digitalWrite(LED0, currentState.led0);
}
