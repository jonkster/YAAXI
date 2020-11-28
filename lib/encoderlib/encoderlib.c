#include "encoderlib.h"

volatile bool switchState = false;
volatile bool enc0_aState = false;
volatile bool enc0_bState = false;
volatile bool enc1_aState = false;
volatile bool enc1_bState = false;

volatile bool prevSwitchState = false;
volatile bool prevEnc0_aState = false;
volatile bool prevEnc0_bState = false;
volatile bool prevEnc1_aState = false;
volatile bool prevEnc1_bState = false;

volatile int encoder0Pos = 0;
volatile int encoder1Pos = 0;
volatile int encoder0LastPos = 0;
volatile int encoder1LastPos = 0;
volatile bool buttonPressed = 0;

volatile bool somethingHappened = false;

int calcEncoderDelta(int aNow, int aPrev, int bNow, int bPrev) {
	if ((aPrev == LOW) && (aNow == HIGH)) {
		if (bNow == LOW) {
			return -1;
		} else {
			return 1;
		}
	} /*else if ((aPrev == HIGH) && (aNow == LOW)) {
		if (bNow == HIGH) {
			return -1;
		} else {
			return 1;
		}
	} else if ((bPrev == LOW) && (bNow == HIGH)) {
		if (aNow == HIGH) {
			return -1;
		} else {
			return 1;
		}
	} */ else if ((bPrev == HIGH) && (bNow == LOW)) {
		if (aNow == LOW) {
			return -1;
		} else {
			return 1;
		}
	}
	return 0;
}

bool debugDidSomethingHappen(void) {
	bool tmp = somethingHappened;
	somethingHappened = false;
	return tmp;
}

void checkChanges(void) {
	somethingHappened = false;
	if (enc0_aState != prevEnc0_aState) {
		encoder0LastPos = encoder0Pos;
		encoder0Pos += calcEncoderDelta(enc0_aState, prevEnc0_aState, enc0_bState, prevEnc0_bState);
		prevEnc0_aState = enc0_aState;
		somethingHappened = true;
	}
	if (enc0_bState != prevEnc0_bState) {
		encoder0LastPos = encoder0Pos;
		encoder0Pos += calcEncoderDelta(enc0_aState, prevEnc0_aState, enc0_bState, prevEnc0_bState);
		prevEnc0_bState = enc0_bState;
		somethingHappened = true;
	}
	if (enc1_aState != prevEnc1_aState) {
		encoder1LastPos = encoder1Pos;
		encoder1Pos += calcEncoderDelta(enc1_aState, prevEnc1_aState, enc1_bState, prevEnc1_bState);
		prevEnc1_aState = enc1_aState;
		somethingHappened = true;
	}
	if (enc1_bState != prevEnc1_bState) {
		encoder1LastPos = encoder1Pos;
		encoder1Pos += calcEncoderDelta(enc1_aState, prevEnc1_aState, enc1_bState, prevEnc1_bState);
		prevEnc1_bState = enc1_bState;
		somethingHappened = true;
	}
	if (switchState != prevSwitchState) {
		if (! switchState) {
			buttonPressed = 1;
		}
		prevSwitchState = switchState;
		somethingHappened = true;
	}
}

bool checkSwitchPushed(void) {
	bool pressed = buttonPressed;
	resetSwitch();
	return pressed;
}

int getEncoderDir(int encNum) {
	if (encNum == 0) {
		if (encoder0LastPos > encoder0Pos) {
			encoder0LastPos = encoder0Pos;
			return -1;
		}
		else if (encoder0LastPos < encoder0Pos) {
			encoder0LastPos = encoder0Pos;
			return 1;
		}
		else
			return 0;
	} else {
		if (encoder1LastPos > encoder1Pos) {
			encoder1LastPos = encoder1Pos;
			return -1;
		}
		else if (encoder1LastPos < encoder1Pos) {
			encoder1LastPos = encoder1Pos;
			return 1;
		}
		else
			return 0;
	}
	return 0;
}

int getEncoderValue(int encNum) {
	if (encNum == 0) {
		return encoder0Pos;
	} else {
		return encoder1Pos;
	}
}

void setupEncoders(void) {
	pinMode(SW, INPUT_PULLUP);
	pinMode(ENC_GND, OUTPUT);
	digitalWrite(ENC_GND, 0);
	pinMode(ENC0_A, INPUT_PULLUP);
	pinMode(ENC1_A, INPUT_PULLUP);
	pinMode(ENC0_B, INPUT_PULLUP);
	pinMode(ENC1_B, INPUT_PULLUP);

	cli();
	PCICR |= (1 << PCIE2);  // turn on port C
	// mask in the pins so they can interrupt
	PCMSK2 |= (1 << PCINT16); 
	PCMSK2 |= (1 << PCINT17);
	PCMSK2 |= (1 << PCINT18);
	PCMSK2 |= (1 << PCINT19);
	PCMSK2 |= (1 << PCINT20);
	sei();
}

void resetSwitch(void) {
	buttonPressed = 0;
}

ISR(PCINT2_vect)
{
	switchState = digitalRead(SW);
	enc0_aState = digitalRead(ENC0_A);
	enc0_bState = digitalRead(ENC0_B);
	enc1_aState = digitalRead(ENC1_A);
	enc1_bState = digitalRead(ENC1_B);
	checkChanges();
}


