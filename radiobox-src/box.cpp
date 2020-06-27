#include <Arduino.h>
#include <XC3714.h>
#include "hardware.h"
#include "communication.h"

#define BOXID "BOXID:Motion Capture-Radio Box v0-feeddeadfeed"
#define BOX_DEFINITIONS "CDI-CB,OBS-CB,MSG-CB,FPL-MSG,PROC-CB,ENTER-CB,CLR-CB,MENU-CB,DTO-CB,RNG-IN-CB,RNG-OUT-CB,OUTER_NAV_KNOB-CB,INNER_NAV_KNOB-CB"
#define MSG_BOX_FISH "Avduino Box Fish"
#define MSG_XP_PLUGIN_FISH "XP Plugin Fish"
struct tBoxState lastState;

void actOnControlMessage(char* msg);
void sendBoxConfig();
void setControl(char* device, char* value);


void actOnAnyXPMessage() {
	char msg[48];
	memset(msg, 0, 48);
	if (getMessage(msg)) {
		if (strcmp(MSG_BOX_FISH, msg) == 0) {
			sendBoxConfig();
		} else {
			actOnControlMessage(msg);
		}
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

void setup(){
	Serial.begin(115200);
	initHardware();
	setupEthernet();
	testLeds(50);
	lastState = getBoxState();
	sendMessage(MSG_XP_PLUGIN_FISH);
}


void sendAnyChanges(struct tBoxState state) {
	int rmode = getRadioMode();
	char mode[5] = "COM1";
	switch (rmode) {
		case COM1_M:
			strcpy(mode, "COM1");
			break;
		case COM2_M:
			strcpy(mode, "COM2");
			break;
		case NAV1_M:
			strcpy(mode, "NAV1");
			break;
		case NAV2_M:
			strcpy(mode, "NAV2");
			break;
		case ADF1_M:
			strcpy(mode, "ADF1");
			break;
	}
	char msg[48];
	if (lastState.sw00 != state.sw00) {
		snprintf(msg, sizeof(msg), "FLIP_%s:%i\n", mode, state.sw00);
		sendMessage(msg);
	}
	if (lastState.sw01 != state.sw01) {
		if (state.sw01) {
			incMode();
		}
	}
	if (lastState.sw02 != state.sw02) {
		if (state.sw02) {
			incMode();
		}
	}
	if (lastState.knob00 != state.knob00) {
		int delta = state.knob00 - lastState.knob00;
		if (delta > 0) {
			snprintf(msg, sizeof(msg), "MHZ_KNOB_%s:%i\n", mode, 1);
			sendMessage(msg);
		} else if (delta < 0) {
			snprintf(msg, sizeof(msg), "MHZ_KNOB_%s:%i\n", mode, 0);
			sendMessage(msg);
		}
	}
	if (lastState.knob01 != state.knob01) {
		int delta = state.knob01 - lastState.knob01;
		if (delta > 0) {
			snprintf(msg, sizeof(msg), "KHZ_KNOB_%s:%i\n", mode, 1);
			sendMessage(msg);
		} else if (delta < 0) {
			snprintf(msg, sizeof(msg), "KHZ_KNOB_%s:%i\n", mode, 0);
			sendMessage(msg);
		}
	}
}

void loop(){
	if (! checkModeChange()) {
		if (getDiagnosticMode()) {
			diagnosticDisplay();
		} else {
			if (! knowsXPAddr()) {
				showNoConnection(1000);
				// trawl for a suitable XP plugin
				delay(1000);
				sendMessage(MSG_XP_PLUGIN_FISH);
				actOnAnyXPMessage();
			} else {
				// we now know there is an XP plugin, send any changes to it
				if (boxStateChanged()) {
					struct tBoxState state = getBoxState();
					sendAnyChanges(state);
					lastState = state;
					displayBoxState(state);
				}

			}
			// listen for (and act on) any XP plugin responses
			actOnAnyXPMessage();
		}
	}
}
