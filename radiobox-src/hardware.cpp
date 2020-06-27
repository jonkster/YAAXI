#include <Arduino.h>
#include <XC3714.h>
#include "Rotencoder.h"
#include "hardware.h"

bool diagnosticMode = false;
int oldV = 0;
struct tBoxState boxState;
bool stateChanged = false;

int activeDisplay;
int standbyDisplay;

struct tBoxState readCurrentBoxState();

int mode = COM1_M;

void showRadioMode();

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
		MAX7219shownum(activeDisplay, v1 * 1000, 0);
		MAX7219shownum(standbyDisplay, v0 * 1000, 0);
	}
	if (switchStateChanged()) {
		MAX7219showText(activeDisplay, "All ");
		MAX7219showText(standbyDisplay, "Off");
		if (getSwitchState(SW00)) {
			MAX7219showText(activeDisplay, "button 0");
			MAX7219showText(standbyDisplay, "On");
		}
		if (getSwitchState(SW01)) {
			MAX7219showText(activeDisplay, "button 1");
			MAX7219showText(standbyDisplay, "On");
		}
		if (getSwitchState(SW02)) {
			MAX7219showText(activeDisplay, "button 2");
			MAX7219showText(standbyDisplay, "On");
		}
	}
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

void incMode() {
	switch(mode) {
		case COM1_M: mode = COM2_M;
			     break;
		case COM2_M: mode = NAV1_M;
			     break;
		case NAV1_M: mode = NAV2_M;
			     break;
		case NAV2_M: mode = ADF1_M;
			     break;
		case ADF1_M: mode = COM1_M;
			     break;
		default: mode = COM1_M;
	}
	showRadioMode();
}


void initHardware() {
        // pins for switches
        pinMode (SW00, INPUT_PULLUP);
        pinMode (SW01, INPUT_PULLUP);
        pinMode (SW02, INPUT_PULLUP);
        /*pinMode (SW03, INPUT_PULLUP);
        pinMode (SW04, INPUT_PULLUP);
        pinMode (SW05, INPUT_PULLUP);
        pinMode (SW06, INPUT_PULLUP);
        pinMode (SW07, INPUT_PULLUP);
        pinMode (SW08, INPUT_PULLUP);
        pinMode (SW09, INPUT_PULLUP);
        pinMode (SW10, INPUT_PULLUP);
        pinMode (SW11, INPUT_PULLUP);*/

        // pins for LEDS
        //pinMode (LED0, OUTPUT);
        //pinMode (LED1, OUTPUT);

	// setup rotary encoders
	setRotPins();

	// setup 7 seg led displays
	activeDisplay = MAX7219addDisplay(43, 45, 47);
        standbyDisplay = MAX7219addDisplay(42, 44, 46);
        MAX7219brightness(activeDisplay, 0);
        MAX7219brightness(standbyDisplay, 0 );

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
	currentState.sw03 = false;
	currentState.sw04 = false;
	currentState.sw05 = false;
	currentState.sw06 = false;
	currentState.sw07 = false;
	currentState.sw08 = false;
	currentState.sw09 = false;
	currentState.sw10 = false;
	currentState.sw11 = false;
	currentState.knob00 = getEncoderValue(0);
	currentState.knob01 = getEncoderValue(1);
	return currentState;
}

int getRadioMode() {
	return mode;
}

void showRadioMode() {
	char unit = '1';
	char type = 'C';
	switch(mode) {
		case COM1_M: unit = '1';
			     type = 'C';
			     break;
		case COM2_M:  unit = '2';
			     type = 'C';
			     break;
		case NAV1_M:  unit = '1';
			     type = 'N';
			     break;
		case NAV2_M:  unit = '2';
			     type = 'N';
			     break;
		case ADF1_M:  unit = ' ';
			     type = 'A';
			     break;
		default: 
			     break;

	}
	MAX7219showSymAtDigit(activeDisplay, 7, type);
	MAX7219showSymAtDigit(standbyDisplay, 7, unit);
}

void setControl(char* device, char* value) {
	int v = atoi(value);
	if (strcmp("LED0", device) == 0) setLedState(LED0, v);
	else if (strcmp("LED1", device) == 0) setLedState(LED1, v);
	else {
		long vl = atof(value) * 10;
		if (strcmp("DIGITS0", device) == 0) {
			if (mode == COM1_M) {
				MAX7219blankAll(activeDisplay);
				MAX7219shownum(activeDisplay, vl, 3);
			}
		} else if (strcmp("DIGITS1", device) == 0) {
			if (mode == COM1_M) {
				MAX7219blankAll(standbyDisplay);
				MAX7219shownum(standbyDisplay, vl, 3);
			}
		} else if (strcmp("DIGITS2", device) == 0) {
			if (mode == COM2_M) {
				MAX7219blankAll(activeDisplay);
				MAX7219shownum(activeDisplay, vl, 3);
			}
		} else if (strcmp("DIGITS3", device) == 0) {
			if (mode == COM2_M) {
				MAX7219blankAll(standbyDisplay);
				MAX7219shownum(standbyDisplay, vl, 3);
			}
		} else if (strcmp("DIGITS4", device) == 0) {
			if (mode == NAV1_M) {
				MAX7219blankAll(activeDisplay);
				MAX7219shownum(activeDisplay, vl, 3);
			}
		} else if (strcmp("DIGITS5", device) == 0) {
			if (mode == NAV1_M) {
				MAX7219blankAll(standbyDisplay);
				MAX7219shownum(standbyDisplay, vl, 3);
			}
		} else if (strcmp("DIGITS6", device) == 0) {
			if (mode == NAV2_M) {
				MAX7219blankAll(activeDisplay);
				MAX7219shownum(activeDisplay, vl, 3);
			}
		} else if (strcmp("DIGITS7", device) == 0) {
			if (mode == NAV2_M) {
				MAX7219blankAll(standbyDisplay);
				MAX7219shownum(standbyDisplay, vl, 3);
			}
		} else if (strcmp("DIGITS8", device) == 0) {
			if (mode == ADF1_M) {
				MAX7219blankAll(activeDisplay);
				MAX7219shownum(activeDisplay, vl, 3);
			}
		} else if (strcmp("DIGITS9", device) == 0) {
			if (mode == ADF1_M) {
				MAX7219blankAll(standbyDisplay);
				MAX7219shownum(standbyDisplay, vl, 3);
			}
		}
		else Serial.println("could not find device!");
		showRadioMode();
	}
}

void setDiagnosticMode(bool mode) {
	diagnosticMode = mode;
}

void setLedState(int ledPin, bool state) {
	//digitalWrite(ledPin, state);
	char lp[10];
	snprintf(lp, sizeof(lp)-1, "pin %i", ledPin);
	MAX7219showText(activeDisplay, lp);
	if (state) {
		MAX7219showText(standbyDisplay, "On");
	} else {
		MAX7219showText(standbyDisplay, "Off");
	}
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
	MAX7219blankAll(activeDisplay);
	delay(delayms);
	MAX7219showText(activeDisplay, " Error");
	delay(delayms);
	MAX7219blankAll(activeDisplay);
}

void showNoConnection(const int delayms) {
	MAX7219blankAll(activeDisplay);
	MAX7219showText(activeDisplay, "no plane");
	MAX7219showText(standbyDisplay, "");
	delay(delayms);
	MAX7219showText(activeDisplay, "");
	MAX7219showText(standbyDisplay, "no plane");
}

void showOKLeds(const int delayms) {
	MAX7219blankAll(activeDisplay);
	delay(delayms);
	MAX7219showText(activeDisplay, "no Error");
	delay(delayms);
	MAX7219blankAll(activeDisplay);
}


void testLeds(const int delayms) {
	MAX7219showText(activeDisplay, "       ");
	delay(delayms);
	MAX7219showText(activeDisplay, "testing");
	delay(delayms);
	MAX7219showText(activeDisplay, "       ");
}

