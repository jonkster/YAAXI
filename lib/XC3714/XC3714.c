#include <Arduino.h>
#include "XC3714.h"

#define MAX_DISPLAYS 4

int clkPins[MAX_DISPLAYS] = {};
int csPins[MAX_DISPLAYS] = {};
int dataPins[MAX_DISPLAYS] = {};
int displayCount = 0;

void MAX7219senddata(int unitNumber, byte reg, byte data){
	if (unitNumber >= MAX_DISPLAYS) {
		return;
	}
	int csPin = csPins[unitNumber];
	int clkPin = clkPins[unitNumber];
	int dataPin = dataPins[unitNumber];

	digitalWrite(csPin, LOW);   //CS on
	shiftOut(dataPin, clkPin, MSBFIRST, reg);
	shiftOut(dataPin, clkPin, MSBFIRST, data);
	digitalWrite(csPin, HIGH);   //CS off
}

void MAX7219brightness(int unitNumber, byte b){  //0-15 is range high nybble is ignored
	if (unitNumber >= MAX_DISPLAYS) {
		return;
	}
	MAX7219senddata(unitNumber, BRIGHTNESS_REG, b);        //intensity  
}

void blankDigit(int unitNumber, const int d) {
	if (unitNumber >= MAX_DISPLAYS) {
		return;
	}
	MAX7219senddata(unitNumber, d, 0);
}

void blankAll(int unitNumber) {
	for(int i = 0; i < 8; i++) {
		blankDigit(unitNumber, i + 1);		//blank all
	}
}

void MAX7219init(int unitNumber, int csPin, int clkPin, int dataPin) {
	pinMode(csPin, OUTPUT);
	pinMode(dataPin, OUTPUT);
	pinMode(clkPin, OUTPUT);

	digitalWrite(csPin, HIGH); //CS off
	digitalWrite(clkPin, LOW); //CLK low

	MAX7219senddata(unitNumber, TESTMODE_REG, 0);       //test mode off
	MAX7219senddata(unitNumber, SHUTDOWN_REG, 0);       //display off
	MAX7219senddata(unitNumber, DECODE_REG, 0x00);      // no decode of any digit
	MAX7219senddata(unitNumber, SCAN_LIMIT_REG, 7);
	blankAll(unitNumber);
	MAX7219senddata(unitNumber, SHUTDOWN_REG, 1);

}

int MAX7219addDisplay(int csPin, int clkPin, int dataPin){
	int idx = displayCount;
	if (idx >= MAX_DISPLAYS) {
		return -1;
	}
	csPins[idx] = csPin;
	dataPins[idx] = dataPin;
	clkPins[idx] = clkPin;

	MAX7219init(idx, csPin, clkPin, dataPin);
	displayCount++;
	return idx;
}

void showE(int unitNumber, int isErr) {
	int code = 0x00;
	if (isErr) {
		code = 0x4f;
	}
	MAX7219senddata(unitNumber, 7, code);        
}

unsigned int getSymCode(char n) {
	switch(n) {
		case ' ': return 0b00000000;
		case '0': return 0b01111110;
		case '1': return 0b00110000;
		case '2': return 0b01101101;
		case '3': return 0b01111001;
		case '4': return 0b00110011;
		case '5': return 0b01011011;
		case '6': return 0b01011111;
		case '7': return 0b01110000;
		case '8': return 0b01111111;
		case '9': return 0b01110011;
		case 'E': return 0b00011111;
		case '-': return 0b00000001;
		case '|': return 0b01100000;
		case 'A': return 0b01110111;
		case 'C': return 0b01001110;
		case 'N': return 0b01110110;
		case 'c': return 0b00001101;
		case 'i': return 0b00000100;
		case 'n': return 0b00010101;
		case '~': return 0b00010100;
	}
	return 0;
}

void MAX7219showSymAtDigit(int unitNumber, int digit, char sym){
	unsigned int s = getSymCode(sym);
	MAX7219senddata(unitNumber, digit+1, s);        
}


void MAX7219shownum(int unitNumber, unsigned long n, const int dpPos){
	blankAll(unitNumber);
	unsigned long k = n;
	for(int digit = 0; digit < 8; digit++){
		char sym = (k % 10UL) + '0';
		int code = getSymCode(sym);
		if (digit > 5) {
			code = getSymCode(' ');
		} else if (digit == 3) {
			code += 0x80; // decimal point
		}
		MAX7219senddata(unitNumber, digit+1, code);
		k = k / 10UL;
	}
}


