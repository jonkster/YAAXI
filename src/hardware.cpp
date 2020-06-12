#include <Arduino.h>
#include <XC3714.h>
#include "Rotencoder.h"
#include "hardware.h"

bool diagnosticMode = false;
int oldV = 0;
struct tBoxState boxState;
bool stateChanged = false;

struct tBoxState readCurrentBoxState();

bool knobStateChanged() {
	bool changed = false;
	struct tBoxState currentState = readCurrentBoxState();
	if (currentState.knob0 != boxState.knob0) changed = true;
	if (currentState.knob1 != boxState.knob1) changed = true;
	if (changed) {
		boxState = currentState;
	}
	return changed;
}
bool switchStateChanged() {
	bool changed = false;
	struct tBoxState currentState = readCurrentBoxState();
	if (currentState.sw0 != boxState.sw0) changed = true;
	if (currentState.sw1 != boxState.sw1) changed = true;
	if (currentState.sw2 != boxState.sw2) changed = true;
	if (currentState.sw3 != boxState.sw3) changed = true;
	if (currentState.sw4 != boxState.sw4) changed = true;
	if (currentState.sw5 != boxState.sw5) changed = true;
	if (currentState.sw6 != boxState.sw6) changed = true;
	if (currentState.sw7 != boxState.sw7) changed = true;
	if (changed) {
		boxState = currentState;
	}
	return changed;
}

bool boxStateChanged() {
	return (switchStateChanged() || knobStateChanged());
}

bool checkModeChange() {
	if (getSwitchState(PUSH2) && getSwitchState(PUSH3)) {
		showErrorLeds(100);
		setDiagnosticMode(true);
		return true;
	} 
	if (getSwitchState(PUSH0) && getSwitchState(PUSH1)) {
		showOKLeds(100);
		setDiagnosticMode(false);
		return true;
	} 
	return false;
}


void diagnosticDisplay() {
	if (knobStateChanged()) {
		int v0 = getEncoderValue(0);
		int v1 = getEncoderValue(1);
		setLedGroup(RED_LED_BANK, LED_OFF);
		setLedGroup(GREEN_LED_BANK, LED_OFF);
		setLedState(LED0, v0 % 2);
		setLedState(LED1, v0 % 3);
		setLedState(LED2, v0 % 4);
		setLedState(LED3, v1 % 2);
		setLedState(LED4, v1 % 3);
		setLedState(LED5, v1 % 4);
		delay(1000);
	} else if (getSwitchState(TWIST0)) {
		setLedGroup(RED_LED_BANK, LED_ON);
		setLedGroup(GREEN_LED_BANK, LED_OFF);
	} else if (getSwitchState(TWIST1)) {
		setLedGroup(RED_LED_BANK, LED_OFF);
		setLedGroup(GREEN_LED_BANK, LED_ON);
	} else {
		setLedState(LED0, getSwitchState(TOGGLE0));
		setLedState(LED1, getSwitchState(TOGGLE1));
		setLedState(LED2, getSwitchState(PUSH0));
		setLedState(LED3, getSwitchState(PUSH1));
		setLedState(LED4, getSwitchState(PUSH2));
		setLedState(LED5, getSwitchState(PUSH3));
	}
}

void displayBoxState(struct tBoxState state) {
	Serial.print("sw0: "); Serial.println(state.sw0);
	Serial.print("sw1: "); Serial.println(state.sw1);
	Serial.print("sw2: "); Serial.println(state.sw2);
	Serial.print("sw3: "); Serial.println(state.sw3);
	Serial.print("sw4: "); Serial.println(state.sw4);
	Serial.print("sw5: "); Serial.println(state.sw5);
	Serial.print("sw6: "); Serial.println(state.sw6);
	Serial.print("sw7: "); Serial.println(state.sw7);
	Serial.print("knob0: "); Serial.println(state.knob0);
	Serial.print("knob1: "); Serial.println(state.knob1);
	Serial.println("-------------------------------");
}

struct tBoxState getBoxState() {
	return boxState;
}

bool getDiagnosticMode() {
	return diagnosticMode;
}

int getEncoderValue(int encNumber) {
	return getEncValue(encNumber);
}

bool getLedState(int ledPin) {
	return digitalRead(ledPin);
}

bool getSwitchState(unsigned int switchPin) {
	unsigned int newValue = digitalRead(switchPin);
	// toggle switches have reverse logic
	if ((switchPin == TOGGLE0) || (switchPin == TOGGLE1)) {
		newValue = ! newValue;
	}
	if (newValue == 0) {
		return true;
	}
	return false;
}

void initHardware() {
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
	setRotPins();

	// display leds indicating system ready
	showErrorLeds(100);
	delay(200);
	showOKLeds(100);
	delay(200);

	// read current state of box
	boxState = readCurrentBoxState();
}


struct tBoxState readCurrentBoxState() {
	struct tBoxState currentState;
	currentState.sw0 = getSwitchState(TOGGLE0);
	currentState.sw1 = getSwitchState(TOGGLE1);
	currentState.sw2 = getSwitchState(PUSH0);
	currentState.sw3 = getSwitchState(PUSH1);
	currentState.sw4 = getSwitchState(PUSH2);
	currentState.sw5 = getSwitchState(PUSH3);
	currentState.sw6 = getSwitchState(TWIST0);
	currentState.sw7 = getSwitchState(TWIST1);
	currentState.knob0 = getEncoderValue(0);
	currentState.knob1 = getEncoderValue(1);
	return currentState;
}

void setControl(char* device, char* value) {
	int v = atoi(value);
	if (strcmp("LED0", device) == 0) setLedState(LED0, v);
	else if (strcmp("LED1", device) == 0) setLedState(LED1, v);
	else if (strcmp("LED2", device) == 0) setLedState(LED2, v);
	else if (strcmp("LED3", device) == 0) setLedState(LED3, v);
	else if (strcmp("LED4", device) == 0) setLedState(LED4, v);
	else if (strcmp("LED5", device) == 0) setLedState(LED5, v);
	else {
		Serial.println("could not find device!");
	}
}

void setDiagnosticMode(bool mode) {
	diagnosticMode = mode;
}

void setLedState(int ledPin, bool state) {
	digitalWrite(ledPin, state);
}

void setLedIDXState(int ledIdx, bool state) {
	switch(ledIdx) {
		case 0: setLedState(LED0, state); break;
		case 1: setLedState(LED1, state); break;
		case 2: setLedState(LED2, state); break;
		case 3: setLedState(LED3, state); break;
		case 4: setLedState(LED4, state); break;
		case 5: setLedState(LED5, state); break;
	}	
}

// turns on/off the left or right bank of leds
void setLedGroup(unsigned int redOrGreen, bool state) {
	if (redOrGreen == RED_LED_BANK) {
		setLedState((unsigned int) LED0, state);
		setLedState((unsigned int) LED2, state);
		setLedState((unsigned int) LED4, state);
	} else {
		setLedState((unsigned int) LED1, state);
		setLedState((unsigned int) LED3, state);
		setLedState((unsigned int) LED5, state);
	}
}

void showErrorLeds(const int delayms) {
	setLedGroup(RED_LED_BANK, LED_ON);
	setLedGroup(GREEN_LED_BANK, LED_OFF);
	delay(delayms);
	setLedGroup(RED_LED_BANK, LED_OFF);
	delay(delayms);
	setLedGroup(RED_LED_BANK, LED_ON);
	delay(delayms);
	setLedGroup(RED_LED_BANK, LED_OFF);
}

void showOKLeds(const int delayms) {
	setLedGroup(RED_LED_BANK, LED_OFF);
	setLedGroup(GREEN_LED_BANK, LED_ON);
	delay(delayms);
	setLedGroup(GREEN_LED_BANK, LED_OFF);
	delay(delayms);
	setLedGroup(GREEN_LED_BANK, LED_ON);
	delay(delayms);
	setLedGroup(GREEN_LED_BANK, LED_OFF);
}


void testLeds(const int delayms) {
	unsigned int ledsInOrder[] = { LED0, LED1, LED2, LED3, LED4, LED5 };
	unsigned int ledCount =  sizeof(ledsInOrder)/sizeof(ledsInOrder[0]);

	for (unsigned int i = 0; i < ledCount; i++) {
		setLedIDXState(i, LED_ON);
		delay(delayms);
		setLedIDXState(i, LED_OFF);
		delay(delayms);
	}
	delay(delayms);
	for (int i = ledCount - 1; i >= 0; i--) {
		setLedIDXState(i, LED_ON);
		delay(delayms);
		setLedIDXState(i, LED_OFF);
		delay(delayms);
	}
	delay(delayms);
}

