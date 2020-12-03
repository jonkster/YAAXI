#include <Arduino.h>
#include <LiquidCrystal.h>
#include "encoderlib.h"
#include "communication.h"
#include "box.h"

// PIN ASSIGNMENTS
#define FF_SWITCH_PUSH 24
#define FF_SWITCH_LO   25

#define POS_BL 2
#define CONTRAST_0 3
#define CONTRAST_1 4

#define POS_LCD_0 39
#define RS_0  41
#define RW_0  42
#define EN_0  43
#define D4_0  44
#define D5_0  45
#define D6_0  46
#define D7_0  47

#define POS_LCD_1 29
#define RS_1  31
#define RW_1  32
#define EN_1  33
#define D4_1  34
#define D5_1  35
#define D6_1  36
#define D7_1  37

// display contrast/brightness
#define CONTRASTV_0 30
#define CONTRASTV_1 80
#define BL 180

#define ACTIVE_CHAR 0x7f
//#define ACTIVE_CHAR 0xff



LiquidCrystal lcd0(RS_0, RW_0, EN_0, D4_0, D5_0, D6_0, D7_0);
LiquidCrystal lcd1(RS_1, RW_1, EN_1, D4_1, D5_1, D6_1, D7_1);

struct tBoxState {
	unsigned long c1Use;
	unsigned long c1Stb;
	unsigned long c2Use;
	unsigned long c2Stb;
	unsigned long n1Use;
	unsigned long n1Stb;
	unsigned long n2Use;
	unsigned long n2Stb;
	int activeDisplay;
};
struct tBoxState currentState;
struct tBoxState prevState;


void testScreens(void);
void showNetworkInfo(void);

unsigned long adjustFreq(unsigned long v, unsigned long min, unsigned long max) {
	if (v > max) {
		v = min;
	}
	else if (v < min)  {
		v = max;
	}
	return v;
}

void markActiveRadio() {
	lcd0.setCursor(15, 0);
	lcd0.print(" ");
	lcd0.setCursor(15, 1);
	lcd0.print(" ");
	lcd1.setCursor(15, 0);
	lcd1.print(" ");
	lcd1.setCursor(15, 1);
	lcd1.print(" ");
	switch(currentState.activeDisplay) {
		case 0:
			lcd0.setCursor(15, 0);
			lcd0.write(ACTIVE_CHAR);
			break;
		case 1:
			lcd0.setCursor(15, 1);
			lcd0.write(ACTIVE_CHAR);
			break;
		case 2:
			lcd1.setCursor(15, 0);
			lcd1.write(ACTIVE_CHAR);
			break;
		case 3:
		default:
			lcd1.setCursor(15, 1);
			lcd1.write(ACTIVE_CHAR);
			break;
	}
}

void flipFrequencies() {
	unsigned long tmp;
	switch(currentState.activeDisplay) {
		case 0:
			tmp = currentState.c1Stb;
			currentState.c1Stb = currentState.c1Use;
			currentState.c1Use = tmp;
			sendMessage("FLIP_COM1\n");
			break;
		case 1:
			tmp = currentState.c2Stb;
			currentState.c2Stb = currentState.c2Use;
			currentState.c2Use = tmp;
			sendMessage("FLIP_COM2\n");
			break;
		case 2:
			tmp = currentState.n1Stb;
			currentState.n1Stb = currentState.n1Use;
			currentState.n1Use = tmp;
			sendMessage("FLIP_NAV1\n");
			break;
		case 3:
		default:
			tmp = currentState.n2Stb;
			currentState.n2Stb = currentState.n2Use;
			currentState.n2Use = tmp;
			sendMessage("FLIP_NAV2\n");
			break;
	}
}


int flipFlopDelay = 0;
bool flipFlopPushed() {
	if (flipFlopDelay == 0) {
		bool pushed = (! digitalRead(FF_SWITCH_PUSH));
		if (pushed) {
			flipFlopDelay = 25;	
		}
		return pushed;
	}
	else {
		flipFlopDelay--;
		return false;
	}
}

void setStartState(void) {
	// display starting message
	lcd0.clear();
	lcd1.clear();

	lcd0.setCursor(0, 0);
	lcd0.print("Found XPlane @");
	char* xip = getXpIPAddress();
	lcd0.setCursor(0, 1);
	lcd0.print(xip);

	lcd1.setCursor(0, 0);
	lcd1.print("Starting Radio");
	delay(2000);

	// clear screen
	lcd0.clear();
	lcd1.clear();
	
	// set starting frequencies
	currentState.c1Use = 12010;
	currentState.c1Stb = 12510;
	currentState.c2Use = 12190;
	currentState.c2Stb = 12455;
	currentState.n1Use = 11020;
	currentState.n1Stb = 10710;
	currentState.n2Use = 10930;
	currentState.n2Stb = 10820;
	currentState.activeDisplay = 0;

}

void boxSetup() {

	// power up lcd
	pinMode(POS_LCD_0, OUTPUT);
	digitalWrite(POS_LCD_0, 1);
	pinMode(POS_LCD_1, OUTPUT);
	digitalWrite(POS_LCD_1, 1);

	// contrast and backlight settings
	pinMode(CONTRAST_0, OUTPUT);
	analogWrite(CONTRAST_0, CONTRASTV_0);
	pinMode(CONTRAST_1, OUTPUT);
	analogWrite(CONTRAST_1, CONTRASTV_1);
	pinMode(POS_BL, OUTPUT);
	analogWrite(POS_BL, BL);

	// set up the LCD's number of columns and rows:
	LiquidCrystal lcdA(RS_0, RW_0, EN_0, D4_0, D5_0, D6_0, D7_0);
	LiquidCrystal lcdB(RS_1, RW_1, EN_1, D4_1, D5_1, D6_1, D7_1);
	delay(200);
	lcd0.begin(16, 2);
	lcd1.begin(16, 2);
	delay(200);
	testScreens();

	// set up switch and encoders
	pinMode(FF_SWITCH_PUSH, INPUT_PULLUP);
	pinMode(FF_SWITCH_LO, OUTPUT);
	digitalWrite(FF_SWITCH_LO, 0);
	setupEncoders();

	delay(200);
	lcd0.clear();
	lcd1.clear();

}

void testScreens() {
	lcd0.cursor();
	lcd1.cursor();

	lcd0.setCursor(0, 0);
	lcd0.print("Test Screen 1");
	delay(200);
	lcd0.print(" OK");
	lcd0.setCursor(0, 1);
	lcd0.print("Test Screen 2");
	delay(200);
	lcd0.print(" OK");

	lcd1.setCursor(0, 0);
	lcd1.print("Test Screen 3");
	delay(200);
	lcd1.print(" OK");
	lcd1.setCursor(0, 1);
	lcd1.print("Test Screen 4");
	lcd1.print(" OK");
	delay(500);

	lcd0.setCursor(0, 0);
	lcd0.print("                ");
	delay(200);
	lcd0.setCursor(0, 1);
	lcd0.print("                ");
	delay(200);

	lcd1.setCursor(0, 0);
	lcd1.print("                ");
	delay(200);
	lcd1.setCursor(0, 1);
	lcd1.print("                ");
	delay(500);

	lcd0.noCursor();
	lcd1.noCursor();
}

void normaliseFreqs() {
	// adjust freqs to fit spectrum
	currentState.c1Use = adjustFreq(currentState.c1Use, 11800, 13695);
	currentState.c1Stb = adjustFreq(currentState.c1Stb, 11800, 13695);
	currentState.c2Use = adjustFreq(currentState.c2Use, 11800, 13695);
	currentState.c2Stb = adjustFreq(currentState.c2Stb, 11800, 13695);
	currentState.n1Use = adjustFreq(currentState.n1Use, 10800, 11795);
	currentState.n1Stb = adjustFreq(currentState.n1Stb, 10800, 11795);
	currentState.n2Use = adjustFreq(currentState.n2Use, 10800, 11795);
	currentState.n2Stb = adjustFreq(currentState.n2Stb, 10800, 11795);
}


void noConnectionActions(void) {
	// display diagnostics while waiting for connection
	lcd0.setCursor(0, 0);
	lcd0.print("Cannot find XP");
	showNetworkInfo();
	delay(500);
	lcd0.clear();
	lcd0.setCursor(0, 0);
	lcd0.print("Looking for XP");
	fishForPlugin();
}

void printFreqs() {
	char buf[15];
	// c1 use
	lcd0.setCursor(0, 0);
	dtostrf((double)(currentState.c1Use)/100, 6, 2, buf);
	lcd0.print(buf);

	// c1 stdby
	lcd0.setCursor(8, 0);
	dtostrf((double)(currentState.c1Stb)/100, 6, 2, buf);
	lcd0.print(buf);

	// c2 use
	lcd0.setCursor(0, 1);
	dtostrf((double)(currentState.c2Use)/100, 6, 2, buf);
	lcd0.print(buf);

	// c2 stdby
	lcd0.setCursor(8, 1);
	dtostrf((double)(currentState.c2Stb)/100, 6, 2, buf);
	lcd0.print(buf);

	// n1 use
	lcd1.setCursor(0, 0);
	dtostrf((double)(currentState.n1Use)/100, 6, 2, buf);
	lcd1.print(buf);

	// n1 stdby
	lcd1.setCursor(8, 0);
	dtostrf((double)(currentState.n1Stb)/100, 6, 2, buf);
	lcd1.print(buf);

	// n2 use
	lcd1.setCursor(0, 1);
	dtostrf((double)(currentState.n2Use)/100, 6, 2, buf);
	lcd1.print(buf);

	// n2 stdby
	lcd1.setCursor(8, 1);
	dtostrf((double)(currentState.n2Stb)/100, 6, 2, buf);
	lcd1.print(buf);
}

void showNetworkInfo(void) {
	lcd1.setCursor(0, 0);
	lcd1.print("ME ");
	char *ip = getMyIPAddress();
	lcd1.print(ip);
	free(ip);

	lcd1.setCursor(0, 1);
	lcd1.print("XP ");
	char *xip = getXpIPAddress();
	lcd1.print(xip);
	free(xip);
}

void setControl(char* device, char* value) {
	// act on message from XPlane by setting our systems state
	unsigned long v = atol(value);
	if (strcmp("C1U", device) == 0) {
		currentState.c1Use = v;	
	} else if (strcmp("C1S", device) == 0) {
		currentState.c1Stb = v;	
	} else if (strcmp("C2U", device) == 0) {
		currentState.c2Use = v;	
	} else if (strcmp("C2S", device) == 0) {
		currentState.c2Stb = v;	
	} else if (strcmp("N1U", device) == 0) {
		currentState.n1Use = v;	
	} else if (strcmp("N1S", device) == 0) {
		currentState.n1Stb = v;	
	} else if (strcmp("N2U", device) == 0) {
		currentState.n2Use = v;	
	} else if (strcmp("N2S", device) == 0) {
		currentState.n2Stb = v;	
	} else {
		if (DEBUG_MODE) {
			Serial.println("could not find device!");
		}
	}
}


bool sendAnyChanges() {
	// send any changes we know about to XPlane
	bool changes = false;
	/*if (currentState.c1Use != prevState.c1Use) {
		sendDataTypeLong("C1U", currentState.c1Use);   
		changes = true;
	}
	if (currentState.c1Stb != prevState.c1Stb) {
		sendDataTypeLong("C1S", currentState.c1Use);   
		changes = true;
	}
	if (currentState.c2Use != prevState.c2Use) {
		sendDataTypeLong("C2U", currentState.c1Use);   
		changes = true;
	}
	if (currentState.c2Stb != prevState.c2Stb) {
		sendDataTypeLong("C2S", currentState.c1Use);   
		changes = true;
	}
	if (currentState.n1Use != prevState.n1Use) {
		sendDataTypeLong("N1U", currentState.c1Use);   
		changes = true;
	}
	if (currentState.n1Stb != prevState.n1Stb) {
		sendDataTypeLong("N1S", currentState.c1Use);   
		changes = true;
	}
	if (currentState.n2Use != prevState.n2Use) {
		sendDataTypeLong("N2U", currentState.c1Use);   
		changes = true;
	}
	if (currentState.n2Stb != prevState.n2Stb) {
		sendDataTypeLong("N2S", currentState.c1Use);   
		changes = true;
	}*/
	return changes;
}

void clearChanges(void) {
	prevState = currentState;
}

void boxMainLoop(void) {
		printFreqs();
		markActiveRadio();
		// change active unit if button pressed
		if (checkSwitchPushed()) {
			currentState.activeDisplay = (currentState.activeDisplay+1) % 4;
		} 

		// get changes to frequency
		int incKhz = getEncoderDir(0);
		int incMhz = getEncoderDir(1);
		if (incKhz != 0) {
			switch(currentState.activeDisplay) {
				case 0:
					sendDataTypeInt("KHZ_KNOB_COM1", incKhz);
					break;
				case 1:
					sendDataTypeInt("KHZ_KNOB_COM2", incKhz);
					break;
				case 2:
					sendDataTypeInt("KHZ_KNOB_NAV1", incKhz);
					break;
				case 3:
				default:
					sendDataTypeInt("KHZ_KNOB_NAV2", incKhz);
					break;
			}
		}
		if (incMhz != 0) {
			switch(currentState.activeDisplay) {
				case 0:
					sendDataTypeInt("MHZ_KNOB_COM1", incMhz);
					break;
				case 1:
					sendDataTypeInt("MHZ_KNOB_COM2", incMhz);
					break;
				case 2:
					sendDataTypeInt("MHZ_KNOB_NAV1", incMhz);
					break;
				case 3:
				default:
					sendDataTypeInt("MHZ_KNOB_NAV2", incMhz);
					break;
			}
		}
		/*if ((incKhz != 0) || (incMhz != 0)) {
			// make changes to active Unit
			switch(currentState.activeDisplay) {
				case 0:
					currentState.c1Stb -= incKhz * 5;
					currentState.c1Stb -= incMhz * 100;
					break;
				case 1:
					currentState.c2Stb -= incKhz * 5;
					currentState.c2Stb -= incMhz * 100;
					break;
				case 2:
					currentState.n1Stb -= incKhz * 5;
					currentState.n1Stb -= incMhz * 100;
					break;
				case 3:
				default:
					currentState.n2Stb -= incKhz * 5;
					currentState.n2Stb -= incMhz * 100;
					break;
			}
		}*/
		normaliseFreqs();

		if (flipFlopPushed()) {
			flipFrequencies();
		}
}