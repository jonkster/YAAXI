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
	if (currentState.knob00 != boxState.knob00) changed = true;
	if (currentState.knob01 != boxState.knob01) changed = true;
	if (changed) {
		boxState = currentState;
	}
	return changed;
}
bool switchStateChanged() {
	bool changed = false;
	struct tBoxState currentState = readCurrentBoxState();
	if (currentState.sw00 != boxState.sw00) changed = true;
	if (currentState.sw01 != boxState.sw01) changed = true;
	if (currentState.sw02 != boxState.sw02) changed = true;
	if (currentState.sw03 != boxState.sw03) changed = true;
	if (currentState.sw04 != boxState.sw04) changed = true;
	if (currentState.sw05 != boxState.sw05) changed = true;
	if (currentState.sw06 != boxState.sw06) changed = true;
	if (currentState.sw07 != boxState.sw07) changed = true;
	if (currentState.sw08 != boxState.sw08) changed = true;
	if (currentState.sw09 != boxState.sw09) changed = true;
	if (currentState.sw10 != boxState.sw10) changed = true;
	if (currentState.sw11 != boxState.sw11) changed = true;
	if (changed) {
		boxState = currentState;
	}
	return changed;
}

bool boxStateChanged() {
	return (switchStateChanged() || knobStateChanged());
}

bool checkModeChange() {
	if (getSwitchState(SW00) && getSwitchState(SW01)) {
		showErrorLeds(100);
		setDiagnosticMode(true);
		return true;
	} 
	if (getSwitchState(SW00) && getSwitchState(SW02)) {
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
		setLedState(LED1, v1 % 2);
		delay(500);
	}
	int swArray[] = { SW00, SW01, SW02, SW03, SW04, SW05, SW06, SW07, SW08, SW09, SW10, SW11 };
	for (int i = 0; i < (int)(sizeof(swArray)/sizeof(swArray[0])); i++) {
		int ledPin = swArray[i];
		if (getSwitchState(ledPin)) {
			if (i % 2) {
				setLedGroup(RED_LED_BANK, LED_ON);
				setLedGroup(GREEN_LED_BANK, LED_OFF);
			} else {
				setLedGroup(RED_LED_BANK, LED_OFF);
				setLedGroup(GREEN_LED_BANK, LED_ON);
			}
			delay(500);
		}
	}
	setLedGroup(RED_LED_BANK, LED_OFF);
	setLedGroup(GREEN_LED_BANK, LED_OFF);
}

void displayBoxState(struct tBoxState state) {
	Serial.print("sw00: "); Serial.println(state.sw00);
	Serial.print("sw01: "); Serial.println(state.sw01);
	Serial.print("sw02: "); Serial.println(state.sw02);
	Serial.print("sw03: "); Serial.println(state.sw03);
	Serial.print("sw04: "); Serial.println(state.sw04);
	Serial.print("sw05: "); Serial.println(state.sw05);
	Serial.print("sw06: "); Serial.println(state.sw06);
	Serial.print("sw07: "); Serial.println(state.sw07);
	Serial.print("sw08: "); Serial.println(state.sw08);
	Serial.print("sw09: "); Serial.println(state.sw09);
	Serial.print("sw10: "); Serial.println(state.sw10);
	Serial.print("sw11: "); Serial.println(state.sw11);
	Serial.print("knob00: "); Serial.println(state.knob00);
	Serial.print("knob01: "); Serial.println(state.knob01);
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
	if (newValue == 0) {
		return true;
	}
	return false;
}

void initHardware() {
        // pins for switches
        pinMode (SW00, INPUT_PULLUP);
        pinMode (SW01, INPUT_PULLUP);
        pinMode (SW02, INPUT_PULLUP);
        pinMode (SW03, INPUT_PULLUP);
        pinMode (SW04, INPUT_PULLUP);
        pinMode (SW05, INPUT_PULLUP);
        pinMode (SW06, INPUT_PULLUP);
        pinMode (SW07, INPUT_PULLUP);
        pinMode (SW08, INPUT_PULLUP);
        pinMode (SW09, INPUT_PULLUP);
        pinMode (SW10, INPUT_PULLUP);
        pinMode (SW11, INPUT_PULLUP);

        // pins for LEDS
        pinMode (LED0, OUTPUT);
        pinMode (LED1, OUTPUT);

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
	currentState.sw00 = getSwitchState(SW00);
	currentState.sw01 = getSwitchState(SW01);
	currentState.sw02 = getSwitchState(SW02);
	currentState.sw03 = getSwitchState(SW03);
	currentState.sw04 = getSwitchState(SW04);
	currentState.sw05 = getSwitchState(SW05);
	currentState.sw06 = getSwitchState(SW06);
	currentState.sw07 = getSwitchState(SW07);
	currentState.sw08 = getSwitchState(SW08);
	currentState.sw09 = getSwitchState(SW09);
	currentState.sw10 = getSwitchState(SW10);
	currentState.sw11 = getSwitchState(SW11);
	currentState.knob00 = getEncoderValue(0);
	currentState.knob01 = getEncoderValue(1);
	return currentState;
}

void setControl(char* device, char* value) {
	int v = atoi(value);
	if (strcmp("LED0", device) == 0) setLedState(LED0, v);
	else if (strcmp("LED1", device) == 0) setLedState(LED1, v);
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
	}	
}

// turns on/off the left or right bank of leds
void setLedGroup(unsigned int redOrGreen, bool state) {
	if (redOrGreen == RED_LED_BANK) {
		setLedState((unsigned int) LED0, state);
	} else {
		setLedState((unsigned int) LED1, state);
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
	unsigned int ledsInOrder[] = { LED0, LED1 };
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

