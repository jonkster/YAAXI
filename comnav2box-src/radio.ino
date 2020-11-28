#include <Arduino.h>
#include <LiquidCrystal.h>
#include <avr/interrupt.h>
#include "encoderlib.h"
#include "communication.h"


#define BOXID "BOXID:Motion Capture-CN001A-00000"
#define BOX_DEFINITIONS "C1USE-CI,C1STBY-CI,C2USE-CI,C2STBY-CI,N1USE-CI,N1STBY-CI,N2USE-CI,N2STBY-CI"
#define MSG_BOX_FISH "Avduino Box Fish"
#define MSG_XP_PLUGIN_FISH "XP Plugin Fish"

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

void initialiseState(void) {
	currentState.c1Use = 120100;
	currentState.c1Stb = 125100;
	currentState.c2Use = 121900;
	currentState.c2Stb = 124550;
	currentState.n1Use = 110200;
	currentState.n1Stb = 107100;
	currentState.n2Use = 109300;
	currentState.n2Stb = 108200;
	currentState.activeDisplay = 0;
}

LiquidCrystal lcd0(RS_0, RW_0, EN_0, D4_0, D5_0, D6_0, D7_0);
LiquidCrystal lcd1(RS_1, RW_1, EN_1, D4_1, D5_1, D6_1, D7_1);

void setControl(char* device, char* value) {
	unsigned long v = atol(value);
	if (strcmp("C1USE", device) == 0) {
		currentState.c1Use = v;	
	} else if (strcmp("C1STBY", device) == 0) {
		currentState.c1Stb = v;	
	} else if (strcmp("C2USE", device) == 0) {
		currentState.c2Use = v;	
	} else if (strcmp("C2STBY", device) == 0) {
		currentState.c2Stb = v;	
	} else if (strcmp("N1USE", device) == 0) {
		currentState.n1Use = v;	
	} else if (strcmp("N1STBY", device) == 0) {
		currentState.n1Stb = v;	
	} else if (strcmp("N2USE", device) == 0) {
		currentState.n2Use = v;	
	} else if (strcmp("N2STBY", device) == 0) {
		currentState.n2Stb = v;	
	} else {
		Serial.println("could not find device!");
		digitalWrite(LED_BUILTIN, 0);
	}
}

void actOnControlMessage(char* msg) {
	char seps[] = ":";
	char* token;
	token = strtok(msg, seps);
	if (token != NULL) {
		char device[12];
		strncpy(device, token, 12);
		token = strtok(NULL, seps);
		if (token != NULL) {
			setControl(device, token);
		} else {
			Serial.print("message: '"); Serial.print(msg); Serial.println("' - no value field??");
		}
	} else {
		Serial.print("message: '"); Serial.print(msg); Serial.println("' - no msg??");
	}
}

void sendBoxConfig() {
	sendMessage(BOXID ":" BOX_DEFINITIONS);
}

void actOnAnyXPMessage() {
	char msg[48];
	memset(msg, 0, 48);
	if (getMessage(msg)) {
		lcd0.clear();
		lcd1.clear();
		if (strcmp(MSG_BOX_FISH, msg) == 0) {
			sendBoxConfig();
		} else {
			actOnControlMessage(msg);
		}
	}
}

void sendAnyChanges(struct tBoxState state) {
	char msg[48] = "      ";
	snprintf(msg, sizeof(msg), "C1USE:%ld\n", state.c1Use);
	sendMessage(msg);
	snprintf(msg, sizeof(msg), "C1STDBY:%ld\n", state.c1Stb);
	sendMessage(msg);
	snprintf(msg, sizeof(msg), "C2USE:%ld\n", state.c2Use);
	sendMessage(msg);
	snprintf(msg, sizeof(msg), "C2STDBY:%ld\n", state.c2Stb);
	sendMessage(msg);
	snprintf(msg, sizeof(msg), "N1USE:%ld\n", state.n1Use);
	sendMessage(msg);
	snprintf(msg, sizeof(msg), "N1STDBY:%ld\n", state.n1Stb);
	sendMessage(msg);
	snprintf(msg, sizeof(msg), "N2USE:%ld\n", state.n2Use);
	sendMessage(msg);
	snprintf(msg, sizeof(msg), "N2STDBY:%ld\n", state.n2Stb);
	sendMessage(msg);
}

void identifyScreens() {
	lcd0.cursor();
	lcd1.cursor();

	lcd0.setCursor(0, 0);
	lcd0.print("TEST COM1");
	delay(200);
	lcd0.print(" OK");
	lcd0.setCursor(0, 1);
	lcd0.print("TEST COM2");
	delay(200);
	lcd0.print(" OK");

	lcd1.setCursor(0, 0);
	lcd1.print("TEST NAV1");
	delay(200);
	lcd1.print(" OK");
	lcd1.setCursor(0, 1);
	lcd1.print("TEST NAV2");
	lcd1.print(" OK");
	delay(500);

	lcd0.setCursor(0, 0);
	lcd0.print("            ");
	delay(200);
	lcd0.setCursor(0, 1);
	lcd0.print("            ");
	delay(200);

	lcd1.setCursor(0, 0);
	lcd1.print("            ");
	delay(200);
	lcd1.setCursor(0, 1);
	lcd1.print("            ");
	delay(500);

	lcd0.noCursor();
	lcd1.noCursor();
}

void showNetworkInfo() {
	lcd1.setCursor(0, 0);
	lcd1.print("ME ");
	lcd1.print(IP_ADDRESS_ST);

	lcd1.setCursor(0, 1);
	lcd1.print("XP ");
	char *xip;
	xip = getXpIPAddress();
	lcd1.print(xip);
	free(xip);
}


void setup() {
	Serial.begin(115200);

	setupEthernet();
	sendMessage(MSG_XP_PLUGIN_FISH);

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

	identifyScreens();

	pinMode(FF_SWITCH_PUSH, INPUT_PULLUP);
	pinMode(FF_SWITCH_LO, OUTPUT);
	digitalWrite(FF_SWITCH_LO, 0);
	setupEncoders();

	delay(200);
	lcd0.clear();
	lcd1.clear();

	initialiseState();
}


unsigned long adjustFreq(unsigned long v, unsigned long min, unsigned long max) {
	if (v > max) {
		Serial.print("v="); Serial.print(v);
		Serial.print("  min="); Serial.print(min);
		Serial.print("  max="); Serial.print(max);
		Serial.println("v is > max, set it to min");
		v = min;
	}
	else if (v < min)  {
		Serial.print("v="); Serial.print(v);
		Serial.print("  min="); Serial.print(min);
		Serial.print("  max="); Serial.print(max);
		Serial.println("v is < min, set it to max");
		v = max;
	}
	return v;
}

void drawActive() {
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

void flipFlop() {
	unsigned long tmp;
	switch(currentState.activeDisplay) {
		case 0:
			tmp = currentState.c1Stb;
			currentState.c1Stb = currentState.c1Use;
			currentState.c1Use = tmp;
			break;
		case 1:
			tmp = currentState.c2Stb;
			currentState.c2Stb = currentState.c2Use;
			currentState.c2Use = tmp;
			break;
		case 2:
			tmp = currentState.n1Stb;
			currentState.n1Stb = currentState.n1Use;
			currentState.n1Use = tmp;
			break;
		case 3:
		default:
			tmp = currentState.n2Stb;
			currentState.n2Stb = currentState.n2Use;
			currentState.n2Use = tmp;
			break;
	}
}

void normaliseFreqs() {
	// adjust freqs to fit spectrum
	currentState.c1Use = adjustFreq(currentState.c1Use, 118000, 136975);
	currentState.c1Stb = adjustFreq(currentState.c1Stb, 118000, 136975);
	currentState.c2Use = adjustFreq(currentState.c2Use, 118000, 136975);
	currentState.c2Stb = adjustFreq(currentState.c2Stb, 118000, 136975);
	currentState.n1Use = adjustFreq(currentState.n1Use, 108000, 117950);
	currentState.n1Stb = adjustFreq(currentState.n1Stb, 108000, 117950);
	currentState.n2Use = adjustFreq(currentState.n2Use, 108000, 117950);
	currentState.n2Stb = adjustFreq(currentState.n2Stb, 108000, 117950);
}

void printFreqs() {
	char buf[15];
	// c1 use
	lcd0.setCursor(0, 0);
	dtostrf(truncf(100 * (double)currentState.c1Use/1000)/100, 5, 2, buf);
	lcd0.print(buf);

	// c1 stdby
	lcd0.setCursor(8, 0);
	dtostrf(truncf(100 * (double)currentState.c1Stb/1000)/100, 5, 2, buf);
	lcd0.print(buf);

	// c2 use
	lcd0.setCursor(0, 1);
	dtostrf(truncf(100 * (double)currentState.c2Use/1000)/100, 5, 2, buf);
	lcd0.print(buf);

	// c2 stdby
	lcd0.setCursor(8, 1);
	dtostrf(truncf(100 * (double)currentState.c2Stb/1000)/100, 5, 2, buf);
	lcd0.print(buf);

	// n1 use
	lcd1.setCursor(0, 0);
	dtostrf(truncf(100 * (double)currentState.n1Use/1000)/100, 5, 2, buf);
	lcd1.print(buf);

	// n1 stdby
	lcd1.setCursor(8, 0);
	dtostrf(truncf(100 * (double)currentState.n1Stb/1000)/100, 5, 2, buf);
	lcd1.print(buf);

	// n2 use
	lcd1.setCursor(0, 1);
	dtostrf(truncf(100 * (double)currentState.n2Use/1000)/100, 5, 2, buf);
	lcd1.print(buf);

	// n2 stdby
	lcd1.setCursor(8, 1);
	dtostrf(truncf(100 * (double)currentState.n2Stb/1000)/100, 5, 2, buf);
	lcd1.print(buf);
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

bool stateChanged = false;
void loop() {
	if (! knowsXPAddr()) {
		lcd0.setCursor(0, 0);
		lcd0.print("Cannot find XP");
		showNetworkInfo();
		delay(500);
		lcd0.clear();
		lcd0.setCursor(0, 0);
		lcd0.print("Looking for XP");
		sendMessage(MSG_XP_PLUGIN_FISH);
		actOnAnyXPMessage();
		delay(500);
	} else {
		actOnAnyXPMessage();
		printFreqs();
		drawActive();
		// change active unit if button pressed
		if (checkSwitchPushed()) {
			currentState.activeDisplay = (currentState.activeDisplay+1) % 4;
			stateChanged = true;
		} 

		// get changes to frequency
		int incKhz = getEncoderDir(0);
		int incMhz = getEncoderDir(1);
		if ((incKhz != 0) || (incMhz != 0)) {
			stateChanged = true;
			// make changes to active Unit
			switch(currentState.activeDisplay) {
				case 0:
					currentState.c1Stb -= incKhz * 50;
					currentState.c1Stb -= incMhz * 1000;
					break;
				case 1:
					currentState.c2Stb -= incKhz * 50;
					currentState.c2Stb -= incMhz * 1000;
					break;
				case 2:
					currentState.n1Stb -= incKhz * 50;
					currentState.n1Stb -= incMhz * 1000;
					break;
				case 3:
				default:
					currentState.n2Stb -= incKhz * 50;
					currentState.n2Stb -= incMhz * 1000;
					break;
			}
		}
		normaliseFreqs();

		// act on flipflop switch
		if (flipFlopPushed()) {
			flipFlop();
			stateChanged = true;
		}
		if (stateChanged) {
			stateChanged = false;
			sendAnyChanges(currentState);
		}

	}
}

