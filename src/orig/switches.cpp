#include <Arduino.h>
#include <XC3714.h>
#include "Rotencoder.h"
#include "switches.h"
#include "Xplane.h"

//Leds
#define LED0    2
#define LED1    3
#define LED2    4
#define LED3    5
#define LED4    24
#define LED5    26

// Switches
#define TOGGLE0    8
#define TOGGLE1    9
#define PUSH0    A4
#define PUSH1    A5
#define PUSH2    28
#define PUSH3    30
#define TWIST0    6
#define TWIST1    7

bool lightState[]		= { false, false, false, false, false, false };
unsigned int ledLookup[]	= { LED0, LED1, LED2, LED3, LED4, LED5 };
unsigned int switchState[]	= { 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned int switchLookup[]	= { TOGGLE0, TOGGLE1, PUSH0, PUSH1, PUSH2, PUSH3, TWIST0, TWIST1 };
bool switchChanged[]		= { false, false, false, false, false, false, false, false };
int lastV[2]			= {0, 0};
int lastDirection[2]		= {0, 0};
unsigned long lastChanged[2]	= {0, 0};

// declare functions
int switchCount();
void toggleLightState(const int ledNum);


int getLastRot(const int encoderNum) {
	int last = lastDirection[encoderNum];
	lastDirection[encoderNum] = 0;
	return last;
}

bool actOnRotations() {
	bool boxChanged = false;
	int delta = 1;
	if ((millis() - lastChanged[0]) < 10) {
		delta = 5;
	}
	int counter = getCounter(0);
	int diff = (counter - lastV[0]);
	lastDirection[0] = 0;
	if (diff != 0) {
		boxChanged = true;
		if (((lastDirection[0] > 0) && (diff > 0)) || ((lastDirection[0] < 0) && (diff < 0)))  {
			delta = 0;
		}
		if (diff > 0) {
			lastDirection[0] = -delta;
		} else {
			lastDirection[0] = delta;
		}
		lastV[0] = counter;
		lastChanged[0] = millis();
	}
	delta = 1;
	if ((millis() - lastChanged[1]) < 10) {
		delta = 5;
	}
	counter = getCounter(1);
	diff = (counter - lastV[1]);
	lastDirection[1] = 0;
	if (diff != 0) {
		boxChanged = true;
		if (((lastDirection[1] > 0) && (diff > 0)) || ((lastDirection[1] < 0) && (diff < 0)))  {
			delta = 0;
		}
		if (diff > 0) {
			lastDirection[1] = delta;
		} else {
			lastDirection[1] = -delta;
		}
		lastV[1] = counter;
		lastChanged[1] = millis();
	}
	return boxChanged;
}

bool checkAnySwitchesChanged() {
	bool boxChanged = false;
	for (int i = 0; i < switchCount(); i++) {
		if (switchChanged[i]) {
			boxChanged = true;
		}
	}
	return boxChanged;
}

void checkSwitches() {
	updateRot();
	for (int i = 0; i < switchCount(); i++) {
		switchChanged[i] = false;
		int pin = switchLookup[i];
		unsigned int v = digitalRead(pin);
		if (v != switchState[i]) {
			switchState[i] = v;
			switchChanged[i] = true;
		}
	}
}

bool getSwitchChanged(const int switchIdx) {
	return switchChanged[switchIdx];
}

int getSwitchState(const int switchIdx) {
	return switchState[switchIdx];
}


void initSwitches() {
        // pins for switches
        pinMode (TOGGLE0, INPUT);
        pinMode (TOGGLE1, INPUT);
        pinMode (PUSH0, INPUT);
        pinMode (PUSH1, INPUT);
        pinMode (PUSH2, INPUT);
        pinMode (PUSH3, INPUT);
        pinMode (TWIST0, INPUT);
        pinMode (TWIST1, INPUT);

        // pins for LEDS
        pinMode (LED0, OUTPUT);
        pinMode (LED1, OUTPUT);
        pinMode (LED2, OUTPUT);
        pinMode (LED3, OUTPUT);
        pinMode (LED4, OUTPUT);
        pinMode (LED5, OUTPUT);

	// setup rotary encoders
	setRotPins(0, A0, A1);
	setRotPins(1, A2, A3);

	lastChanged[0] = millis();
	lastChanged[1] = millis();
}

int ledCount() {
	return sizeof(ledLookup)/sizeof(ledLookup[0]);
}

// set led based on state value (led index NOT name)
void lightAct(const int ledNum) {
	int pin = LED0;
	if (ledNum < ledCount()) {
		pin = ledLookup[ledNum];
	}
	digitalWrite(pin, lightState[ledNum]);
}

// returns if led lit or not, index by led number NOT pin name
bool getLed(const int ledNum) {
	int pin = LED0;
	if (ledNum < ledCount()) {
		pin = ledLookup[ledNum];
	}
	return digitalRead(pin);
}

void lightsAll(const bool onoff) {
	for (int i = 0; i < ledCount(); i++) {
		lightSet(i, onoff);
	}
}

// set led, index by led number NOT pin name
void lightSet(const int ledNum, const bool onoff) {
	if (ledNum >= ledCount()) {
		return;
	}
	if (lightState[ledNum] != onoff) {
		lightState[ledNum] = onoff;
	}
	lightAct(ledNum);
}

int switchCount() {
	return sizeof(switchLookup)/sizeof(switchLookup[0]);
}

void showWaiting() {
	for (int i = 0; i < ledCount(); i++) {
		toggleLightState(i);
	}
}

void testLeds(const int delayms) {
	for (int i = 0; i < ledCount(); i++) {
		toggleLightState(i);
		delay(delayms);
	}
	delay(2 * delayms);
	for (int i = 0; i < ledCount(); i++) {
		toggleLightState(i);
		delay(delayms);
	}
}

// toggle leds, index by led number NOT name
void toggleLightState(const int ledNum) {
	lightState[ledNum] = ! lightState[ledNum];
	lightAct(ledNum);
}
