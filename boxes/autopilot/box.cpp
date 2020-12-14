#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include "encoderlib.h"
#include "communication.h"
#include "box.h"

// PIN ASSIGNMENTS
// DISPLAY devices
#define LED0 8
#define LED1 9

// CONTROL devices
#define AP   25
#define FD   26
#define HDG  27
#define ALT  28
#define NAV  29
#define APR  30
#define VS   31
#define UP   32
#define DOWN 33

# define CLK 10  // E
# define DATA 9 // R/W
# define CS 8   // RS
U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, CLK, DATA, CS);

struct tBoxState {
	bool ap;
	bool fd;
	bool hdg;
	bool alt;
	bool nav;
	bool apr;
	bool vs;
	bool up;
	bool down;
	int  selAlt;
};
struct tBoxState currentState;
struct tBoxState prevState;

void showAnnunciators() {
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_unifont_tf);
	u8g2.drawStr(0,12,"AP  FD  HDG");
	u8g2.drawStr(0,24,"ALT NAV  APR");
	u8g2.drawStr(0,36,"VS 500fpm");
	u8g2.drawStr(0,48,"alt 12000 armed");
	u8g2.sendBuffer();
}


void setStartState(void) {
	currentState.ap = false;
	currentState.fd = false;
	currentState.hdg = false;
	currentState.alt = false;
	currentState.nav = false;
	currentState.apr = false;
	currentState.vs = false;
	currentState.up = false;
	currentState.down = false;
	currentState.selAlt = 0;
	prevState = currentState;
	digitalWrite(LED0, 0);
	digitalWrite(LED1, 1);

	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_courB08_tr);
	char* xip = getXpIPAddress();
	u8g2.drawStr(0,12,"Found X Plane @");
	u8g2.drawStr(10,24,xip);
	free(xip);
	u8g2.drawStr(0,36,"Power-up autopilot");
	u8g2.sendBuffer();
	delay(500);
	u8g2.clearBuffer();
	u8g2.drawStr(0,60,"starting...");
	u8g2.sendBuffer();
	delay(500);
	u8g2.clearBuffer();
	u8g2.sendBuffer();
}

void testScreens() {
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_courB10_tr);
	u8g2.drawStr(0,12,"TESTING SCREEN");
	u8g2.sendBuffer();
	delay(200);
	u8g2.clearBuffer();
	u8g2.drawStr(0,24,"TESTING SCREEN");
	u8g2.sendBuffer();
	delay(200);
	u8g2.clearBuffer();
	u8g2.drawStr(0,36,"TESTING SCREEN");
	u8g2.sendBuffer();
	delay(200);
	u8g2.clearBuffer();
	u8g2.drawStr(0,48,"TESTING SCREEN");
	u8g2.sendBuffer();
	delay(200);
	u8g2.clearBuffer();
	u8g2.drawStr(0,60,"TESTING SCREEN");
	u8g2.sendBuffer();
	delay(200);
	u8g2.clearBuffer();
}

void boxSetup() {
	pinMode(LED0, OUTPUT);
	pinMode(LED1, OUTPUT);
	digitalWrite(LED0, 1);
	digitalWrite(LED1, 1);

	pinMode(AP, INPUT_PULLUP);
	pinMode(FD, INPUT_PULLUP);
	pinMode(HDG, INPUT_PULLUP);
	pinMode(ALT, INPUT_PULLUP);
	pinMode(NAV, INPUT_PULLUP);
	pinMode(APR, INPUT_PULLUP);
	pinMode(VS, INPUT_PULLUP);
	pinMode(UP, INPUT_PULLUP);
	pinMode(DOWN, INPUT_PULLUP);
	setupEncoders();

	u8g2.begin();
	//u8g2.setFont(u8g2_font_courB08_tr);
	u8g2.firstPage();
	testScreens();
}

void showNetworkInfo(void) {
	char *ip = getMyIPAddress();
	char *xip = getXpIPAddress();
	u8g2.setFont(u8g2_font_courB08_tr);
	char buf[20];
	sprintf(buf, "ME :%s",  ip);
	u8g2.drawStr(0,48, buf);
	sprintf(buf, "XP :%s",  xip);
	u8g2.drawStr(0,60,buf);
	u8g2.sendBuffer();
	free(ip);
	free(xip);
}


void noConnectionActions(void) {
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_courB08_tr);
	u8g2.drawStr(0,12,"Looking for XP...");
	u8g2.sendBuffer();
	showNetworkInfo();
	fishForPlugin();
	delay(500);
	u8g2.drawStr(0,24,"Cannot find XP");
	u8g2.sendBuffer();
	delay(500);
}

void setControl(char* device, char* value) {
	// act on message from XPlane by setting our systems state
	unsigned int v = atoi(value);
	if (strcmp("LED0", device) == 0) {
		digitalWrite(LED0, v);	
	} else if (strcmp("LED1", device) == 0) {
		digitalWrite(LED1, v);	
	}
}

int bouncer = 0;
void markStart(int delayTime) {
	bouncer = delayTime;
}

void checkSwitches(void) {
	currentState.ap = digitalRead(AP);
	currentState.fd = digitalRead(FD);
	currentState.hdg = digitalRead(HDG);
	currentState.alt = digitalRead(ALT);
	currentState.nav = digitalRead(NAV);
	currentState.apr = digitalRead(APR);
	currentState.vs = digitalRead(VS);
	currentState.up = digitalRead(UP);
	currentState.down = digitalRead(DOWN);
}

void sendChanges() {
	if (bouncer-- <= 0) {
		bouncer = 0;
		if (currentState.ap != prevState.ap) {
			sendDataTypeBool("AP_ON", !currentState.ap);
			markStart(10);
		}
		if (currentState.fd != prevState.fd) {
			sendDataTypeBool("FD_ON", !currentState.fd);
			markStart(10);
		}
		if (currentState.hdg != prevState.hdg) {
			sendDataTypeBool("AP_HDG", !currentState.hdg);
			markStart(10);
		}
		if (currentState.alt != prevState.alt) {
			sendDataTypeBool("AP_ALT", !currentState.alt);
			markStart(10);
		}
		if (currentState.nav != prevState.nav) {
			sendDataTypeBool("AP_NAV", !currentState.nav);
			markStart(10);
		}
		if (currentState.apr != prevState.apr) {
			sendDataTypeBool("AP_APR", !currentState.apr);
			markStart(10);
		}
		if (currentState.vs != prevState.vs) {
			sendDataTypeBool("AP_VS", !currentState.vs);
			markStart(10);
		}
		if (currentState.up != prevState.up) {
			sendDataTypeBool("AP_UP", !currentState.up);
			markStart(10);
		}
		if (currentState.down != prevState.down) {
			sendDataTypeBool("AP_DOWN", !currentState.down);
			markStart(10);
		}
		if (bouncer != 0) {
			u8g2.setCursor(0, 15);
			u8g2.print("PRESSED!");
		} else {
			u8g2.setCursor(0, 15);
			u8g2.print("        ");
		}
		prevState = currentState;
	} else {
	}
}

void boxMainLoop(void) {
	checkSwitches();
	sendChanges();
	int crs = getEncoderDir(0);
	int hdg = getEncoderDir(1);
	if (crs != 0) {
		if (crs == -1) {
			crs = 0;
		}
		sendDataTypeInt("CRS_KNOB", crs);
	} else if (hdg != 0) {
		if (hdg == -1) {
			hdg = 0;
		}
		sendDataTypeInt("HDG_KNOB", hdg);
	}
	showAnnunciators();
}
