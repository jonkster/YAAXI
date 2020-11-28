#include <Arduino.h>
#include <LiquidCrystal.h>
#include <avr/interrupt.h>
#include "encoderlib.h"
#include "communication.h"


#define BOXID "BOXID:Motion Capture-CN001A-00000"
#define BOX_DEFINITIONS "C1USE-CF,C1STBY-CF,C2USE-CF,C2STBY-CF,N1USE-CF,N1STBY-CF,N2USE-CF,N2STBY-CF"
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



float c1Use = 120.10;
float c1Stb = 125.10;
float c2Use = 121.90;
float c2Stb = 124.55;
float n1Use = 110.20;
float n1Stb = 107.10;
float n2Use = 109.30;
float n2Stb = 108.20;
int activeDisplay = 0;

LiquidCrystal lcd0(RS_0, RW_0, EN_0, D4_0, D5_0, D6_0, D7_0);
LiquidCrystal lcd1(RS_1, RW_1, EN_1, D4_1, D5_1, D6_1, D7_1);

void setControl(char* device, char* value) {
	float v = atof(value);
	if (strcmp("C1USE", device) == 0) {
		c1Use = v;	
	} else if (strcmp("C1STBY", device) == 0) {
		c1Stb = v;	
	} else if (strcmp("C2USE", device) == 0) {
		c2Use = v;	
	} else if (strcmp("C2STBY", device) == 0) {
		c2Stb = v;	
	} else if (strcmp("N1USE", device) == 0) {
		n1Use = v;	
	} else if (strcmp("N1STBY", device) == 0) {
		n1Stb = v;	
	} else if (strcmp("N2USE", device) == 0) {
		n2Use = v;	
	} else if (strcmp("N2STBY", device) == 0) {
		n2Stb = v;	
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

	delay(1000);
	lcd0.clear();
	lcd1.clear();
}


float adjustFreq(float v, float min, float max) {
	if (v > max)
		v = min;
	else if (v < min) 
		v = max;
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
	switch(activeDisplay) {
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
	float tmp;
	switch(activeDisplay) {
		case 0:
			tmp = c1Stb;
			c1Stb = c1Use;
			c1Use = tmp;
			break;
		case 1:
			tmp = c2Stb;
			c2Stb = c2Use;
			c2Use = tmp;
			break;
		case 2:
			tmp = n1Stb;
			n1Stb = n1Use;
			n1Use = tmp;
			break;
		case 3:
		default:
			tmp = n2Stb;
			n2Stb = n2Use;
			n2Use = tmp;
			break;
	}
}

void normaliseFreqs() {
	// adjust freqs to fit spectrum
	c1Use = adjustFreq(c1Use, 118.0, 136.975);
	c1Stb = adjustFreq(c1Stb, 118.0, 136.975);
	c2Use = adjustFreq(c2Use, 118.0, 136.975);
	c2Stb = adjustFreq(c2Stb, 118.0, 136.975);
	n1Use = adjustFreq(n1Use, 108.0, 117.95);
	n1Stb = adjustFreq(n1Stb, 108.0, 117.95);
	n2Use = adjustFreq(n2Use, 108.0, 117.95);
	n2Stb = adjustFreq(n2Stb, 108.0, 117.95);
}

void printFreqs() {
	char buf[15];
	// c1 use
	lcd0.setCursor(0, 0);
	dtostrf(c1Use, 5, 2, buf);
	lcd0.print(buf);

	// c1 stdby
	lcd0.setCursor(8, 0);
	dtostrf(c1Stb, 5, 2, buf);
	lcd0.print(buf);

	// c2 use
	lcd0.setCursor(0, 1);
	dtostrf(c2Use, 5, 2, buf);
	lcd0.print(buf);

	// c2 stdby
	lcd0.setCursor(8, 1);
	dtostrf(c2Stb, 5, 2, buf);
	lcd0.print(buf);

	lcd1.setCursor(0, 0);
	dtostrf(n1Use, 5, 2, buf);
	lcd1.print(buf);
	lcd1.setCursor(8, 0);
	dtostrf(n1Stb, 5, 2, buf);
	lcd1.print(buf);
	lcd1.setCursor(0, 1);
	dtostrf(n2Use, 5, 2, buf);
	lcd1.print(buf);
	lcd1.setCursor(8, 1);
	dtostrf(n2Stb, 5, 2, buf);
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
		printFreqs();
		drawActive();
		// change active unit if button pressed
		if (checkSwitchPushed()) {
			activeDisplay = (activeDisplay+1) % 4;
		} 

		// get changes to frequency
		int incKhz = getEncoderDir(0);
		int incMhz = getEncoderDir(1);

		// make changes to active Unit
		switch(activeDisplay) {
			case 0:
				c1Stb -= ((float)incKhz * 0.05);
				c1Stb -= incMhz;
				break;
			case 1:
				c2Stb -= (float)(incKhz * 0.05);
				c2Stb -= incMhz;
				break;
			case 2:
				n1Stb -= (float)(incKhz * 0.05);
				n1Stb -= incMhz;
				break;
			case 3:
			default:
				n2Stb -= (float)(incKhz * 0.05);
				n2Stb -= incMhz;
				break;
		}
		normaliseFreqs();

		// act on flipflop switch
		if (flipFlopPushed()) {
			flipFlop();
		}
		actOnAnyXPMessage();
	}
}

