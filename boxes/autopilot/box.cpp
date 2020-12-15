#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include "encoderlib.h"
#include "communication.h"
#include "box.h"

// PIN ASSIGNMENTS
// DISPLAY devices

// CONTROL devices
#define AP   42
#define FD   41
#define HDG  40
#define ALT  39
#define NAV  38
#define APR  37
#define VS   36
#define UP   35
#define DOWN 34
#define OBS_PUSH 43
#define HDG_PUSH 44
#define ALT_PUSH 45

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
	bool obs_push;
	bool hdg_push;
	bool alt_push;
	int  altSelected;
	int  vspeedSelected;
	int  altitude;
};
struct tBoxState currentState;
struct tBoxState prevState;

void showAnnunciators() {
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_unifont_tf);
	if (currentState.ap || currentState.fd) {
		if (currentState.ap) {
			u8g2.drawFrame(10, 14, 20, 18);
			u8g2.drawStr(13,27,"AP");
		}
		if (currentState.fd) {
			u8g2.drawStr(48,27,"FD");
		}

		// roll modes
		// either hdg, roll, nav or apr
		if (currentState.hdg) {
			u8g2.drawStr(0,12,"HDG");
		} else if (currentState.apr) {
			u8g2.drawStr(0,12,"APR");
		} else if (currentState.nav) {
			u8g2.drawStr(0,12,"NAV");
		} else {
			u8g2.drawStr(0,12,"ROLL");
		}

		// pitch modes
		// either alt, vs or pitch
		if (currentState.alt) {
			u8g2.drawStr(0,45,"ALT");
		} else if (currentState.vs) {
			u8g2.setCursor(0,45);
			u8g2.print("VS ");
			u8g2.print(currentState.vspeedSelected);
			u8g2.print(" fpm");
		} else {
			u8g2.drawStr(0,45,"PITCH");
		}
	}
	if (currentState.altSelected > 0) {
		u8g2.setCursor(24,64);
		u8g2.print(currentState.altSelected);
		u8g2.print("'");
	}
	u8g2.drawVLine(100, 0, 64);
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
	currentState.hdg_push = false;
	currentState.obs_push = false;
	currentState.alt_push = false;
	currentState.altSelected = 1800;
	currentState.vspeedSelected = 0;
	currentState.altitude = 230;
	prevState = currentState;

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
	pinMode(AP, INPUT_PULLUP);
	pinMode(FD, INPUT_PULLUP);
	pinMode(HDG, INPUT_PULLUP);
	pinMode(ALT, INPUT_PULLUP);
	pinMode(NAV, INPUT_PULLUP);
	pinMode(APR, INPUT_PULLUP);
	pinMode(VS, INPUT_PULLUP);
	pinMode(UP, INPUT_PULLUP);
	pinMode(DOWN, INPUT_PULLUP);
	pinMode(OBS_PUSH, INPUT_PULLUP);
	pinMode(HDG_PUSH, INPUT_PULLUP);
	pinMode(ALT_PUSH, INPUT_PULLUP);
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
	if (strcmp("AP_MODE", device) == 0) {
		currentState.ap = (v == 1);
	} 
       	if (strcmp("FD_MODE", device) == 0) {
		currentState.fd = (v == 1);
	} 
       	if (strcmp("HDG_MODE", device) == 0) {
		currentState.hdg = (v == 1);
	} 
       	if (strcmp("ALT_MODE", device) == 0) {
		currentState.alt = (v == 1);
	} 
       	if (strcmp("NAV_MODE", device) == 0) {
		currentState.nav = (v == 1);
	} 
       	if (strcmp("APR_MODE", device) == 0) {
		currentState.apr = (v == 1);
	} 
       	if (strcmp("VS_MODE", device) == 0) {
		currentState.vs = (v == 1);
	} 
       	if (strcmp("ALT_SEL", device) == 0) {
		currentState.altSelected = v;
	} 
       	if (strcmp("AP_VS", device) == 0) {
		currentState.vspeedSelected = v;
	} 
       	if (strcmp("ALTITUDE", device) == 0) {
		currentState.altitude = v;
	} 
}

int bouncer = 0;
void markStart(int delayTime) {
	bouncer = delayTime;
}

void checkSwitches(void) {
}

void sendChanges() {
	if (bouncer-- <= 0) {
		bouncer = 0;
		if (!digitalRead(AP)) {
			sendDataTypeBool("AP_TOGGLE", 1);
			markStart(10);
		}
		if (!digitalRead(FD)) {
			sendDataTypeBool("FD_TOGGLE", 1);
			markStart(10);
		}
		if (!digitalRead(HDG)) {
			sendDataTypeBool("HDG_TOGGLE", 1);
			markStart(10);
		}
		if (!digitalRead(ALT)) {
			sendDataTypeBool("ALT_TOGGLE", 1);
			markStart(10);
		}
		if (!digitalRead(NAV)) {
			sendDataTypeBool("NAV_TOGGLE", 1);
			markStart(10);
		}
		if (!digitalRead(APR)) {
			sendDataTypeBool("APR_TOGGLE", 1);
			markStart(10);
		}
		if (!digitalRead(VS)) {
			sendDataTypeBool("VS_TOGGLE", 1);
			markStart(10);
		}
		if (!digitalRead(UP)) {
			sendDataTypeBool("AP_UP", 1);
			markStart(10);
		}
		if (!digitalRead(DOWN)) {
			sendDataTypeBool("AP_DN", 1);
			markStart(10);
		}
		if (!digitalRead(OBS_PUSH)) {
			sendDataTypeBool("OBS_SYNC_TOGGLE", 1);
			markStart(10);
		}
		if (!digitalRead(HDG_PUSH)) {
			sendDataTypeBool("HDG_SYNC_TOGGLE", 1);
			markStart(10);
		}
		if (!digitalRead(ALT_PUSH)) {
			sendDataTypeBool("ALT_SYNC_TOGGLE", 1);
			markStart(10);
		}
	} else {
	}
}

void boxMainLoop(void) {
	checkSwitches();
	sendChanges();
	int crs = getEncoderDir(0);
	int hdg = getEncoderDir(1);
	int altThousands = getEncoderDir(2);
	int altHundreds = getEncoderDir(3);
	if (crs != 0) {
		if (crs == -1) {
			crs = 0;
		}
		sendDataTypeInt("CRS_KNOB", crs);
	} 
	if (hdg != 0) {
		if (hdg == -1) {
			hdg = 0;
		}
		sendDataTypeInt("HDG_KNOB", hdg);
	} 
       	if (altThousands != 0) {
		if (altThousands == -1) {
			altThousands = 0;
		}
		sendDataTypeInt("ALT_TH_KNOB", altThousands);
	} 
       	if (altHundreds != 0) {
		if (altHundreds == -1) {
			altHundreds = 0;
		}
		sendDataTypeInt("ALT_HU_KNOB", altHundreds);
	}
	showAnnunciators();
}
