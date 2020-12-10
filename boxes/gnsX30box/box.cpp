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
	delay(500);
	digitalWrite(LED0, 1);
	digitalWrite(LED1, 1);
	delay(500);
	digitalWrite(LED0, 0);
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
	currentState.crsr = digitalRead(CRSR);
	currentState.dto = digitalRead(DTO);
	currentState.menu = digitalRead(MENU);
	currentState.clr = digitalRead(CLR);
	currentState.enter = digitalRead(ENTER);
	currentState.proc = digitalRead(PROC);
	currentState.fpl = digitalRead(FPL);
	currentState.msg = digitalRead(MSG);
	currentState.obs = digitalRead(OBS);
	currentState.cdi = digitalRead(CDI);
	currentState.vnav = digitalRead(VNAV);
	currentState.rangeOut = digitalRead(RANGE_OUT);
	currentState.rangeIn = digitalRead(RANGE_IN);
}

void sendChanges() {
	if (bouncer-- <= 0) {
		bouncer = 0;
		if (currentState.crsr != prevState.crsr) {
			sendDataTypeBool("CRSR", currentState.crsr);
			markStart(10);
		}
		if (currentState.dto != prevState.dto) {
			sendDataTypeBool("DTO", currentState.dto);
			markStart(10);
		}
		if (currentState.menu != prevState.menu) {
			sendDataTypeBool("MENU", currentState.menu);
			markStart(10);
		}
		if (currentState.clr != prevState.clr) {
			sendDataTypeBool("CLR", currentState.clr);
			markStart(10);
		}
		if (currentState.enter != prevState.enter) {
			sendDataTypeBool("ENTER", currentState.enter);
			markStart(10);
		}
		if (currentState.proc != prevState.proc) {
			sendDataTypeBool("PROC", currentState.proc);
			markStart(10);
		}
		if (currentState.fpl != prevState.fpl) {
			sendDataTypeBool("FPL", currentState.fpl);
			markStart(10);
		}
		if (currentState.msg != prevState.msg) {
			sendDataTypeBool("MSG", currentState.msg);
			markStart(10);
		}
		if (currentState.obs != prevState.obs) {
			sendDataTypeBool("OBS", currentState.obs);
			markStart(10);
		}
		if (currentState.cdi != prevState.cdi) {
			sendDataTypeBool("CDI", currentState.cdi);
			markStart(10);
		}
		if (currentState.vnav != prevState.vnav) {
			sendDataTypeBool("VNAV", currentState.vnav);
			markStart(10);
		}
		if (currentState.rangeOut != prevState.rangeOut) {
			sendDataTypeBool("RANGEOUT", currentState.rangeOut);
			markStart(10);
		}
		if (currentState.rangeIn != prevState.rangeIn) {
			sendDataTypeBool("RANGEIN", currentState.rangeIn);
			markStart(10);
		}
		prevState = currentState;
	} else {
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
