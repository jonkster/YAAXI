#include <Arduino.h>
#include <XC3714.h>
#include "Rotencoder.h"
#include "hardware.h"

bool diagnosticMode = false;
int oldV = 0;
struct tBoxState boxState;
bool stateChanged = false;

struct tBoxState readCurrentBoxState();
//
// Use software SPI 
TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_SDI, TFT_CLK, TFT_LED);

// hardware SPI (faster - on Mega: 52-SCK, 50-MISO, 51-MOSI)
//TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED);

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
	if (getSwitchState(AP_ON) && getSwitchState(AP_FD)) {
		showErrorLeds(100);
		setDiagnosticMode(true);
		tft.clear();
		return true;
	} 
	if (getSwitchState(AP_ON) && getSwitchState(AP_HDG)) {
		showOKLeds(100);
		setDiagnosticMode(false);
		tft.clear();
		return true;
	} 
	return false;
}

long readDigits() {
	int digit = DIGIT_SELECT_4;
	long v = 0;
	for (int i = 5; i > 0; i--) {
		v *= 10;
		digitalWrite(digit, 0);
		int d = 8 * digitalRead(BCD1);
		d += 4*digitalRead(BCD2);
		d += 2*digitalRead(BCD4);
		d += digitalRead(BCD8);
		digitalWrite(digit--, 1);
		v += (15 - d);
	}
	return v;
}

int readSwitch(int snum) {
	return ! digitalRead(snum);
}

void diagnosticDisplay() {
	tft.drawText(16, 16, "diagnostic mode");
	long v = readDigits();
	char st[32];
	sprintf(st, "alt = %6li   ", v);
	tft.drawText(24, 34, st);
	int s = readSwitch(AP_ON);
	if (s) {
		tft.drawText(26, 56, "AP Toggle");
	} else {
		s = readSwitch(AP_FD);
		if (s) {
			tft.drawText(26, 56, "FD Toggle");
		} else {
			s = readSwitch(AP_HDG);
			if (s) {
				tft.drawText(26, 56, "HDG Toggle");
			} else {
				s = readSwitch(AP_NAV);
				if (s) {
					tft.drawText(26, 56, "NAV Toggle");
				} else {
					s = readSwitch(AP_APR);
					if (s) {
						tft.drawText(26, 56, "APR Toggle");
					} else {
						s = readSwitch(AP_VS);
						if (s) {
							tft.drawText(26, 56, "VS Toggle");
						} else {
							s = readSwitch(AP_ALT);
							if (s) {
								tft.drawText(26, 56, "ALT Toggle");
							} else {
								s = readSwitch(AP_UP);
								if (s) {
									tft.drawText(26, 56, "UP");
								} else {
									s = readSwitch(AP_DN);
									if (s) {
										tft.drawText(26, 56, "DOWN");
									} else {
										tft.drawText(26, 56, "                      ");
									}
								}
							}
						}
					}
				}
			}
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

void initHardware() {
	tft.begin();
	tft.setOrientation(1);
	tft.setFont(Terminal6x8);
	tft.drawRectangle(0, 0, tft.maxX() - 1, tft.maxY() - 1, COLOR_GREEN);
	tft.setOrientation(1);
  	//tft.setFont(Terminal6x8);
  	tft.setFont(Terminal12x16);
  	tft.setFont(Terminal11x16);
  	tft.setGFXFont(&FreeMono12pt7b );

	pinMode (BCD1, INPUT_PULLUP);
	pinMode (BCD2, INPUT_PULLUP);
	pinMode (BCD4, INPUT_PULLUP);
	pinMode (BCD8, INPUT_PULLUP);

	pinMode (DIGIT_SELECT_0, OUTPUT);
	pinMode (DIGIT_SELECT_1, OUTPUT);
	pinMode (DIGIT_SELECT_2, OUTPUT);
	pinMode (DIGIT_SELECT_3, OUTPUT);
	pinMode (DIGIT_SELECT_4, OUTPUT);

	digitalWrite(DIGIT_SELECT_0, 1);
	digitalWrite(DIGIT_SELECT_1, 1);
	digitalWrite(DIGIT_SELECT_2, 1);
	digitalWrite(DIGIT_SELECT_3, 1);
	digitalWrite(DIGIT_SELECT_4, 1);

	pinMode (AP_ON, INPUT_PULLUP);
	pinMode (AP_FD, INPUT_PULLUP);
	pinMode (AP_HDG, INPUT_PULLUP);
	pinMode (AP_NAV, INPUT_PULLUP);
	pinMode (AP_APR, INPUT_PULLUP);
	pinMode (AP_VS, INPUT_PULLUP);
	pinMode (AP_ALT, INPUT_PULLUP);
	pinMode (AP_DN, INPUT_PULLUP);
	pinMode (AP_UP, INPUT_PULLUP);

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
	currentState.sw00 = getSwitchState(AP_ON);
	currentState.sw01 = getSwitchState(AP_FD);
	currentState.sw02 = getSwitchState(AP_HDG);
	currentState.sw03 = getSwitchState(AP_NAV);
	currentState.sw04 = getSwitchState(AP_APR);
	currentState.sw05 = getSwitchState(AP_VS);
	currentState.sw06 = getSwitchState(AP_ALT);
	currentState.sw07 = getSwitchState(AP_UP);
	currentState.sw08 = getSwitchState(AP_DN);
	currentState.knob00 = getEncoderValue(0);
	currentState.knob01 = getEncoderValue(1);
	return currentState;
}

void setControl(char* device, char* value) {
	tft.drawText(8, 16, device);
	tft.drawText(20, 16, value);
}

void setDiagnosticMode(bool mode) {
	diagnosticMode = mode;
}

void setLedState(int ledPin, bool state) {
	digitalWrite(ledPin, state);
}

void setLedIDXState(int ledIdx, bool state) {
	tft.drawText(8, 16, "LED");
	if (state) {
		tft.drawText(20, 16, "ON");
	} else {
		tft.drawText(20, 16, "OFF");
	}
}

// turns on/off the left or right bank of leds
void setLedGroup(unsigned int redOrGreen, bool state) {
	if (redOrGreen == RED_LED_BANK) {
		tft.drawText(8, 16, "RED Mode        ");
	} else {
		tft.drawText(8, 16, "GREEN Mode      ");
	}
}

void showErrorLeds(const int delayms) {
	tft.drawText(8, 16, "No Plane Found");
	delay(delayms);
	tft.clear();
	delay(delayms);
	tft.drawText(16, 32, "is X Plane Running?");
	delay(delayms);
	tft.clear();
	delay(delayms);
	tft.drawText(16, 48, "is Network Down?");
	delay(delayms);
	tft.clear();
}

void showOKLeds(const int delayms) {
	tft.drawText(8, 16, "GREEN Mode    ");
	delay(delayms);
	tft.clear();
	delay(delayms);
	tft.drawText(8, 16, "GREEN Mode    ");
	delay(delayms);
	tft.clear();
}


void testLeds(const int delayms) {
	tft.drawText(8, 16, "TEST Mode      ");
	delay(delayms);
	tft.clear();
	delay(delayms);
	tft.drawText(8, 16, "TEST Mode      ");
	delay(delayms);
	tft.clear();
}

