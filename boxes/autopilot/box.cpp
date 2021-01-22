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
#define AP   35
#define FD   34
#define HDG  37
#define ALT  36
#define NAV  39
#define APR  38
#define VS   40
#define FLC   41
#define UP   33
#define DOWN 32
#define OBS_PUSH 30
#define HDG_PUSH 31
#define ALT_PUSH 29


#define ENC_NUM_OBS 3
#define ENC_NUM_HDG 2
#define ENC_NUM_ALT100 1
#define ENC_NUM_ALT1000 0

#define GEAR_TRANS 42
#define GEAR_1 49
#define GEAR_2 48
#define GEAR_3 44
#define FLAP_TRANS 47
#define FLAP_APP 46
#define FLAP_FULL 45

#define ANN_TEST 43

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
	bool flc;
	bool up;
	bool down;
	bool obs_push;
	bool hdg_push;
	bool alt_push;
	int  altSelected;
	int  vspeedSelected;
	int  altitude;

	bool annucGearTransit;
	bool annucGear1;
	bool annucGear2;
	bool annucGear3;
	bool annucFlapTransit;
	bool annucFlapApp;
	bool annucFlapFull;
	bool annucTestMode;
};
struct tBoxState currentState;
struct tBoxState prevState;

unsigned long testModeStart = 0;
void flashAnnunciators(bool state) {
	testModeStart = millis();
	digitalWrite(GEAR_TRANS, state);
	digitalWrite(GEAR_1, state);
	digitalWrite(GEAR_2, state);
	digitalWrite(GEAR_3, state);
	digitalWrite(FLAP_TRANS, state);
	digitalWrite(FLAP_APP, state);
	digitalWrite(FLAP_FULL, state);
}
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
		} else if (currentState.flc) {
			u8g2.drawStr(0,45,"FLC");
		} else {
			u8g2.drawStr(0,45,"PITCH");
		}
	}
	if (currentState.altSelected > 0) {
		u8g2.setCursor(24,64);
		u8g2.print(currentState.altSelected);
		u8g2.print("'");
	}
	//u8g2.drawVLine(100, 0, 64);
	u8g2.sendBuffer();

	if (! currentState.annucTestMode) {
		digitalWrite(GEAR_TRANS, currentState.annucGearTransit);
		digitalWrite(GEAR_1, currentState.annucGear1);
		digitalWrite(GEAR_2, currentState.annucGear2);
		digitalWrite(GEAR_3, currentState.annucGear3);
		digitalWrite(FLAP_TRANS, currentState.annucFlapTransit);
		digitalWrite(FLAP_APP, currentState.annucFlapApp);
		digitalWrite(FLAP_FULL, currentState.annucFlapFull);
	} else {
		if ((millis() - testModeStart) > 1000) {
			testModeStart = 0;
			currentState.annucTestMode = false;
		}
	}
}


void setStartState(void) {
	currentState.ap = false;
	currentState.fd = false;
	currentState.hdg = false;
	currentState.alt = false;
	currentState.nav = false;
	currentState.apr = false;
	currentState.flc = false;
	currentState.vs = false;
	currentState.up = false;
	currentState.down = false;
	currentState.hdg_push = false;
	currentState.obs_push = false;
	currentState.alt_push = false;
	currentState.altSelected = 1800;
	currentState.vspeedSelected = 0;
	currentState.altitude = 230;
	currentState.annucGearTransit = 0;
	currentState.annucGear1 = 0;
	currentState.annucGear2 = 0;
	currentState.annucGear3 = 0;
	currentState.annucFlapTransit = 0;
	currentState.annucFlapApp = 0;
	currentState.annucFlapFull = 0;
	currentState.annucTestMode = 0;
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
	pinMode(FLC, INPUT_PULLUP);
	pinMode(ANN_TEST, INPUT_PULLUP);
	pinMode(UP, INPUT_PULLUP);
	pinMode(DOWN, INPUT_PULLUP);
	pinMode(OBS_PUSH, INPUT_PULLUP);
	pinMode(HDG_PUSH, INPUT_PULLUP);
	pinMode(ALT_PUSH, INPUT_PULLUP);
	setupEncoders();

	pinMode(GEAR_TRANS, OUTPUT);
	pinMode(GEAR_1, OUTPUT);
	pinMode(GEAR_2, OUTPUT);
	pinMode(GEAR_3, OUTPUT);
	pinMode(FLAP_TRANS, OUTPUT);
	pinMode(FLAP_APP, OUTPUT);
	pinMode(FLAP_FULL, OUTPUT);
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
       	if (strcmp("FLC_MODE", device) == 0) {
		currentState.flc = (v == 1);
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
       	if (strcmp("GEARSAFE1", device) == 0) {
		currentState.annucGear1 = (v == 1);
	}
       	if (strcmp("GEARSAFE2", device) == 0) {
		currentState.annucGear2 = (v == 1);
	}
       	if (strcmp("GEARSAFE3", device) == 0) {
		currentState.annucGear3 = (v == 1);
	} 
       	if (strcmp("GEARTRANSIT", device) == 0) {
		currentState.annucGearTransit = (v == 1);
	}
       	if (strcmp("FLAP", device) == 0) {
		currentState.annucFlapApp = (v == 1);
		currentState.annucFlapFull = (v == 2);
	} 
       	if (strcmp("FLAPTRANSIT", device) == 0) {
		currentState.annucFlapTransit = (v == 1);
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
		if (!digitalRead(FLC)) {
			sendDataTypeBool("FLC_TOGGLE", 1);
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
		if (!digitalRead(ANN_TEST)) {
			currentState.annucTestMode = 1;
			flashAnnunciators(1);
			markStart(10);
		}
	} else {
	}
}

void boxMainLoop(void) {
	checkSwitches();
	sendChanges();
	int hdg = getEncoderDir(ENC_NUM_HDG);
	int obs = getEncoderDir(ENC_NUM_OBS);
	int altThousands = getEncoderDir(ENC_NUM_ALT1000);
	int altHundreds = getEncoderDir(ENC_NUM_ALT100);
	if (obs != 0) {
		// allow acceleration based on knob speed
		int rep = abs(obs);
		if (obs < 0) {
			obs = 1;
		} else {
			obs = 0;
		}
		for (int i = 0; i < rep; i++) {
			sendDataTypeInt("OBS_KNOB", obs);
		}
	} 
	if (hdg != 0) {
		// allow acceleration based on knob speed
		int rep = abs(hdg);
		if (hdg < 0) {
			hdg = 0;
		} else {
			hdg = 1;
		}
		for (int i = 0; i < rep; i++) {
			sendDataTypeInt("HDG_KNOB", hdg);
		}
	} 
       	if (altThousands != 0) {
		if (altThousands < 0) {
			altThousands = 0;
		} else {
			altThousands = 1;
		}
		for (int i = 0; i < 2; i++) {
			sendDataTypeInt("ALT_TH_KNOB", altThousands);
		}
	} 
       	if (altHundreds != 0) {
		if (altHundreds < 0) {
			altHundreds = 1;
		} else {
			altHundreds = 0;
		}
		sendDataTypeInt("ALT_HU_KNOB", altHundreds);
	}
	showAnnunciators();
}
