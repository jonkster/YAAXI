#include <Arduino.h>
#include "encoderlib.h"
#include "communication.h"
#include "box.h"

// PIN ASSIGNMENTS
// DISPLAY devices
#define LED0 8
#define LED1 9

// CONTROL devices
#define CRSR 28
#define DTO 29
#define MENU 30
#define CLR 31
#define ENTER 32
#define PROC 33
#define FPL 34
#define MSG 35
#define OBS 36
#define CDI 37

struct tBoxState {
	bool crsr;
	bool dto;
	bool menu;
	bool clr;
	bool enter;
	bool proc;
	bool fpl;
	bool msg;
	bool obs;
	bool cdi;
	bool led0;
	bool led1;
};
struct tBoxState currentState;
struct tBoxState prevState;


void setStartState(void) {
	currentState.crsr = false;
	currentState.dto = false;
	currentState.menu = false;
	currentState.clr = false;
	currentState.enter = false;
	currentState.proc = false;
	currentState.fpl = false;
	currentState.msg = false;
	currentState.obs = false;
	currentState.cdi = false;
	prevState = currentState;
	digitalWrite(LED0, 0);
	digitalWrite(LED1, 1);
}

void boxSetup() {
	pinMode(LED0, OUTPUT);
	pinMode(LED1, OUTPUT);
	digitalWrite(LED0, 1);
	digitalWrite(LED1, 1);

	pinMode(CRSR, INPUT_PULLUP);
	pinMode(DTO, INPUT_PULLUP);
	pinMode(MENU, INPUT_PULLUP);
	pinMode(CLR, INPUT_PULLUP);
	pinMode(ENTER, INPUT_PULLUP);
	pinMode(PROC, INPUT_PULLUP);
	pinMode(FPL, INPUT_PULLUP);
	pinMode(MSG, INPUT_PULLUP);
	pinMode(OBS, INPUT_PULLUP);
	pinMode(CDI, INPUT_PULLUP);
	setupEncoders();
}

void noConnectionActions(void) {
	digitalWrite(LED0, 0);
	digitalWrite(LED1, 1);
	delay(500);
	fishForPlugin();
	digitalWrite(LED0, 1);
	digitalWrite(LED1, 0);
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

void checkSwitches(void) {
	if (! digitalRead(CRSR)) {
		currentState.crsr = true;
	}
	if (! digitalRead(DTO)) {
		currentState.dto = true;
	}
	if (! digitalRead(MENU)) {
		currentState.menu = true;
	}
	if (! digitalRead(CLR)) {
		currentState.clr = true;
	}
	if (! digitalRead(ENTER)) {
		currentState.enter = true;
	}
	if (! digitalRead(PROC)) {
		currentState.proc = true;
	}
	if (! digitalRead(FPL)) {
		currentState.fpl = true;
	}
	if (! digitalRead(MSG)) {
		currentState.msg = true;
	}
	if (! digitalRead(OBS)) {
		currentState.obs = true;
	}
	if (! digitalRead(CDI)) {
		currentState.cdi = true;
	}
}

void sendChanges() {
	if ((currentState.crsr != prevState.crsr) && (currentState.crsr)) {
		sendMessage("CRSR:1");
	}
	if ((currentState.dto != prevState.dto) && (currentState.dto)) {
		sendMessage("DTO:1");
	}
	if ((currentState.menu != prevState.menu) && (currentState.menu)) {
		sendMessage("MENU:1");
	}
	if ((currentState.clr != prevState.clr) && (currentState.clr)) {
		sendMessage("CLR:1");
	}
	if ((currentState.enter != prevState.enter) && (currentState.enter)) {
		sendMessage("ENTER:1");
	}
	if ((currentState.proc != prevState.proc) && (currentState.proc)) {
		sendMessage("PROC:1");
	}
	if ((currentState.fpl != prevState.fpl) && (currentState.fpl)) {
		sendMessage("FPL:1");
	}
	if ((currentState.msg != prevState.msg) && (currentState.msg)) {
		sendMessage("MSG:1");
	}
	if ((currentState.obs != prevState.obs) && (currentState.obs)) {
		sendMessage("OBS:1");
	}
	if ((currentState.cdi != prevState.cdi) && (currentState.cdi)) {
		sendMessage("CDI:1");
	}
	prevState = currentState;
}

void boxMainLoop(void) {
	checkSwitches();
	sendChanges();
	int bigKnob = getEncoderDir(0);
	int littleKnob = getEncoderDir(1);
	if (bigKnob != 0) {
		if (bigKnob == -1) {
			bigKnob = 0;
		}
		sendDataTypeInt("OUTER_NAV_KNOB", bigKnob);
	} else if (littleKnob != 0) {
		if (littleKnob == -1) {
			littleKnob = 0;
		}
		sendDataTypeInt("INNER_NAV_KNOB", littleKnob);
	}
}
