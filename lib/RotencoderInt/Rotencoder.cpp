#include "Rotencoder.h"

volatile int v0 = 0;
volatile int v1 = 0;
volatile unsigned int enc0A = 0;
volatile unsigned int enc0B = 0;
volatile unsigned int old0  = 0;
volatile unsigned int enc1A = 0;
volatile unsigned int enc1B = 0;
volatile unsigned int old1  = 0;
volatile unsigned long lastIntTime = 0;
#define BOUNCE_DELAY 0

int getEncValue(int encNumber) {
	float factor = 2.0;
	if (encNumber == 0) {
		return v0/factor;
	}
	return v1/factor;
}

void calculateChange(unsigned int encNumber) {
		unsigned int v = enc0A + (enc0B * 2);
		unsigned int old = old0;
		if (encNumber == 1) {
			v = enc1A + (enc1B * 2);
			old = old1;
		}
		// cycle should be 3 -> 1 -> 0 -> 2 -> 3 -> 1 -> 0 -> 2 -> etc
		int delta = 0;
		switch(old) {
			case 0:
				if (v == 2) {
					delta = -1;
				} else if (v  == 1) {
					delta = 1;
				}
				break;
			case 1:
				if (v == 0) {
					delta = -1;
				} else if (v  == 3) {
					delta = 1;
				}
				break;
			case 2:
				if (v == 3) {
					delta = -1;
				} else if (v  == 0) {
					delta = 1;
				}
				break;
			case 3:
				if (v == 1) {
					delta = -1;
				} else if (v  == 2) {
					delta = 1;
				}
				break;
			default:
				break;
		}
		if (delta != 0) {
			if (encNumber == 0) {
				v0 += delta;
				old0 = v;
			} else {
				v1 += delta;
				old1 = v;
			}
		}
}

void enc0AR() {
	cli();
	unsigned long t = millis();
	if (t - lastIntTime > BOUNCE_DELAY) {
		enc0A = digitalRead(20);
		calculateChange(0);
	}
	lastIntTime = t;
	sei();
}

void enc0BR() {
	cli();
	unsigned long t = millis();
	if (t - lastIntTime > BOUNCE_DELAY) {
		enc0B = digitalRead(21);
		calculateChange(0);
	}
	lastIntTime = t;
	sei();
}

void enc1AR() {
	cli();
	unsigned long t = millis();
	if (t - lastIntTime > BOUNCE_DELAY) {
		enc1A = digitalRead(18);
		calculateChange(1);
	}
	lastIntTime = t;
	sei();
}

void enc1BR() {
	cli();
	unsigned long t = millis();
	if (t - lastIntTime > BOUNCE_DELAY) {
		enc1B = digitalRead(19);
		calculateChange(1);
	}
	lastIntTime = t;
	sei();
}

void setRotPins() {
	pinMode(18, INPUT_PULLUP); // turn on internal pull-up resistor for each input
	pinMode(19, INPUT_PULLUP);
	attachInterrupt(4, enc1AR, CHANGE); // pin 19
	attachInterrupt(5, enc1BR, CHANGE); // pin 18

	pinMode(20, INPUT_PULLUP); // turn on internal pull-up resistor for each input
	pinMode(21, INPUT_PULLUP);
	attachInterrupt(2, enc0AR, CHANGE); // pin 21
	attachInterrupt(3, enc0BR, CHANGE); // pin 20
}

