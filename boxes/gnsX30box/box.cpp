#include <Arduino.h>
#include "encoderlib.h"
#include "communication.h"
#include "box.h"

// PIN ASSIGNMENTS
// DISPLAY devices
#define LED0 8
#define LED1 9

// CONTROL devices
#define CRSR 25
#define RANGE_OUT 26
#define RANGE_IN 27
#define DTO 28
#define MENU 29
#define CLR 30
#define ENTER 31
#define PROC 32
#define VNAV 33
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
	bool vnav;
	bool rangeOut;
	bool rangeIn;
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
	currentState.vnav = false;
	currentState.rangeOut = false;
	currentState.rangeIn = false;
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
	pinMode(VNAV, INPUT_PULLUP);
	pinMode(RANGE_OUT, INPUT_PULLUP);
	pinMode(RANGE_IN, INPUT_PULLUP);
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

int bouncer = 0;
void markStart(int delayTime) {
	bouncer = delayTime;
}

void checkSwitches(void) {
	if (! digitalRead(CRSR)) {
		currentState.crsr = true;
		markStart(10);
	}
	if (! digitalRead(DTO)) {
		currentState.dto = true;
		markStart(10);
	}
	if (! digitalRead(MENU)) {
		currentState.menu = true;
		markStart(10);
	}
	if (! digitalRead(CLR)) {
		currentState.clr = true;
		markStart(10);
	}
	if (! digitalRead(ENTER)) {
		currentState.enter = true;
		markStart(10);
	}
	if (! digitalRead(PROC)) {
		currentState.proc = true;
		markStart(10);
	}
	if (! digitalRead(FPL)) {
		currentState.fpl = true;
		markStart(10);
	}
	if (! digitalRead(MSG)) {
		currentState.msg = true;
		markStart(10);
	}
	if (! digitalRead(OBS)) {
		currentState.obs = true;
		markStart(10);
	}
	if (! digitalRead(CDI)) {
		currentState.cdi = true;
		markStart(10);
	}
	if (! digitalRead(VNAV)) {
		currentState.vnav = true;
		markStart(10);
	}
	if (! digitalRead(RANGE_OUT)) {
		currentState.rangeOut = true;
		markStart(10);
	}
	if (! digitalRead(RANGE_IN)) {
		currentState.rangeIn = true;
		markStart(10);
	}
}

void sendChanges() {
	if (bouncer-- <= 0) {
		bouncer = 0;
		if ((currentState.crsr != prevState.crsr) && (currentState.crsr)) {
			sendMessage("CRSR:1");
			currentState.crsr = false;
		}
		if ((currentState.dto != prevState.dto) && (currentState.dto)) {
			sendMessage("DTO:1");
			currentState.dto = false;
		}
		if ((currentState.menu != prevState.menu) && (currentState.menu)) {
			sendMessage("MENU:1");
			currentState.menu = false;
		}
		if ((currentState.clr != prevState.clr) && (currentState.clr)) {
			sendMessage("CLR:1");
			currentState.clr = false;
		}
		if ((currentState.enter != prevState.enter) && (currentState.enter)) {
			sendMessage("ENTER:1");
			currentState.enter = false;
		}
		if ((currentState.proc != prevState.proc) && (currentState.proc)) {
			sendMessage("PROC:1");
			currentState.proc = false;
		}
		if ((currentState.fpl != prevState.fpl) && (currentState.fpl)) {
			sendMessage("FPL:1");
			currentState.fpl = false;
		}
		if ((currentState.msg != prevState.msg) && (currentState.msg)) {
			sendMessage("MSG:1");
			currentState.msg = false;
		}
		if ((currentState.obs != prevState.obs) && (currentState.obs)) {
			sendMessage("OBS:1");
			currentState.obs = false;
		}
		if ((currentState.cdi != prevState.cdi) && (currentState.cdi)) {
			sendMessage("CDI:1");
			currentState.cdi = false;
		}
		if ((currentState.vnav != prevState.vnav) && (currentState.vnav)) {
			sendMessage("VNAV:1");
			currentState.vnav = false;
		}
		if ((currentState.rangeOut != prevState.rangeOut) && (currentState.rangeOut)) {
			sendMessage("RANGEOUT:1");
			currentState.rangeOut = false;
		}
		if ((currentState.rangeIn != prevState.rangeIn) && (currentState.rangeIn)) {
			sendMessage("RANGEIN:1");
			currentState.rangeIn = false;
		}
		prevState = currentState;
	}
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
