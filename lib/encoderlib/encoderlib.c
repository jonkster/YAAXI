#include "encoderlib.h"

volatile bool enc0_aState = false;
volatile bool enc0_bState = false;
volatile bool enc1_aState = false;
volatile bool enc1_bState = false;
volatile bool enc2_aState = false;
volatile bool enc2_bState = false;
volatile bool enc3_aState = false;
volatile bool enc3_bState = false;

volatile bool prevSwitchState = false;
volatile bool prevEnc0_aState = false;
volatile bool prevEnc0_bState = false;
volatile bool prevEnc1_aState = false;
volatile bool prevEnc1_bState = false;
volatile bool prevEnc2_aState = false;
volatile bool prevEnc2_bState = false;
volatile bool prevEnc3_aState = false;
volatile bool prevEnc3_bState = false;

volatile int encoder0Pos = 0;
volatile int encoder1Pos = 0;
volatile int encoder2Pos = 0;
volatile int encoder3Pos = 0;

volatile int encoder0LastPos = 0;
volatile int encoder1LastPos = 0;
volatile int encoder2LastPos = 0;
volatile int encoder3LastPos = 0;

#define BOUNCE_DELAY 2000
volatile unsigned long tNow = 0;
volatile unsigned long tPrev = 0;
volatile bool fast = false;

int calcEncoderDelta(int aNow, int aPrev, int bNow, int bPrev) {
	if ((aPrev == LOW) && (aNow == HIGH)) {
		if (bNow == LOW) {
			return -1;
		} else {
			return 1;
		}
	} else if ((aPrev == HIGH) && (aNow == LOW)) {
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
	}  else if ((bPrev == HIGH) && (bNow == LOW)) {
		if (aNow == LOW) {
			return -1;
		} else {
			return 1;
		}
	}
	return 0;
}

void checkChanges(void) {
	if (enc0_aState != prevEnc0_aState) {
		encoder0LastPos = encoder0Pos;
		encoder0Pos += calcEncoderDelta(enc0_aState, prevEnc0_aState, enc0_bState, prevEnc0_bState);
		prevEnc0_aState = enc0_aState;
	}
	if (enc0_bState != prevEnc0_bState) {
		encoder0LastPos = encoder0Pos;
		encoder0Pos += calcEncoderDelta(enc0_aState, prevEnc0_aState, enc0_bState, prevEnc0_bState);
		prevEnc0_bState = enc0_bState;
	}
	if (enc1_aState != prevEnc1_aState) {
		encoder1LastPos = encoder1Pos;
		encoder1Pos += calcEncoderDelta(enc1_aState, prevEnc1_aState, enc1_bState, prevEnc1_bState);
		prevEnc1_aState = enc1_aState;
	}
	if (enc1_bState != prevEnc1_bState) {
		encoder1LastPos = encoder1Pos;
		encoder1Pos += calcEncoderDelta(enc1_aState, prevEnc1_aState, enc1_bState, prevEnc1_bState);
		prevEnc1_bState = enc1_bState;
	}
	if (enc2_aState != prevEnc2_aState) {
		encoder2LastPos = encoder2Pos;
		encoder2Pos += calcEncoderDelta(enc2_aState, prevEnc2_aState, enc2_bState, prevEnc2_bState);
		prevEnc2_aState = enc2_aState;
	}
	if (enc2_bState != prevEnc2_bState) {
		encoder2LastPos = encoder2Pos;
		encoder2Pos += calcEncoderDelta(enc2_aState, prevEnc2_aState, enc2_bState, prevEnc2_bState);
		prevEnc2_bState = enc2_bState;
	}
	if (enc3_aState != prevEnc3_aState) {
		encoder3LastPos = encoder3Pos;
		encoder3Pos += calcEncoderDelta(enc3_aState, prevEnc3_aState, enc3_bState, prevEnc3_bState);
		prevEnc3_aState = enc3_aState;
	}
	if (enc3_bState != prevEnc3_bState) {
		encoder3LastPos = encoder3Pos;
		encoder3Pos += calcEncoderDelta(enc3_aState, prevEnc3_aState, enc3_bState, prevEnc3_bState);
		prevEnc3_bState = enc3_bState;
	}
}

int getEncoderDir(int encNum) {
	int mag = 0;
	switch (encNum) {
		case 0:
			mag = (encoder0Pos - encoder0LastPos);
			encoder0LastPos = encoder0Pos;
			if (fast) {
				return mag * 3;
			}
			return mag;
			break;
		case 1:
			mag = (encoder1Pos - encoder1LastPos);
			encoder1LastPos = encoder1Pos;
			if (fast) {
				return mag * 3;
			}
			return mag;
			break;
		case 2:
			mag = (encoder2Pos - encoder2LastPos);
			encoder2LastPos = encoder2Pos;
			if (fast) {
				return mag * 3;
			}
			return mag;
			break;
		case 3:
			mag = (encoder3Pos - encoder3LastPos);
			encoder3LastPos = encoder3Pos;
			if (fast) {
				return mag * 3;
			}
			return mag;
			break;
		default:
			return 0;
			break;
	}
}

int getEncoderValue(int encNum) {
	switch (encNum) {
		case 0:
			return encoder0Pos;
			break;
		case 1:
			return encoder1Pos;
			break;
		case 2:
			return encoder2Pos;
			break;
		case 3:
			return encoder3Pos;
			break;
		default:
			return 0;
			break;
	}
}

void setupEncoders(void) {
	pinMode(ENC0_A, INPUT_PULLUP);
	pinMode(ENC0_B, INPUT_PULLUP);
	pinMode(ENC1_A, INPUT_PULLUP);
	pinMode(ENC1_B, INPUT_PULLUP);
	pinMode(ENC2_A, INPUT_PULLUP);
	pinMode(ENC2_B, INPUT_PULLUP);
	pinMode(ENC3_A, INPUT_PULLUP);
	pinMode(ENC3_B, INPUT_PULLUP);

	cli();
	PCICR |= (1 << PCIE2);  // turn on port C
	// mask in the pins so they can interrupt
	PCMSK2 |= (1 << PCINT16); 
	PCMSK2 |= (1 << PCINT17);
	PCMSK2 |= (1 << PCINT18);
	PCMSK2 |= (1 << PCINT19);
	PCMSK2 |= (1 << PCINT20);
	PCMSK2 |= (1 << PCINT21);
	PCMSK2 |= (1 << PCINT22);
	PCMSK2 |= (1 << PCINT23);
	sei();
}

ISR(PCINT2_vect)
{
	tNow = micros();
	if ((tNow - tPrev) > BOUNCE_DELAY) {
		enc0_aState = digitalRead(ENC0_A);
		enc0_bState = digitalRead(ENC0_B);
		enc1_aState = digitalRead(ENC1_A);
		enc1_bState = digitalRead(ENC1_B);
		enc2_aState = digitalRead(ENC2_A);
		enc2_bState = digitalRead(ENC2_B);
		enc3_aState = digitalRead(ENC3_A);
		enc3_bState = digitalRead(ENC3_B);
		checkChanges();
		fast = (tNow - tPrev) < (5 * BOUNCE_DELAY);
	}
	tPrev = tNow;
}


