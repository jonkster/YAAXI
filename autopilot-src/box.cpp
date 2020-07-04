#include <Arduino.h>
#include "hardware.h"
#include "communication.h"

#define BOXID "BOXID:Motion Capture-GNSX30 Box v0-beefdeadfeed"
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


#define KNOB_SPEED_UP 4
void sendAnyChanges(struct tBoxState state) {
	char msg[48];
	if (lastState.sw00 != state.sw00) {
		snprintf(msg, sizeof(msg), "AP_ON:%i\n", state.sw00);
		sendMessage(msg);
	}
	if (lastState.sw01 != state.sw01) {
		snprintf(msg, sizeof(msg), "AP_FD:%i\n", state.sw01);
		sendMessage(msg);
	}
	if (lastState.sw02 != state.sw02) {
		snprintf(msg, sizeof(msg), "AP_HDG:%i\n", state.sw02);
		sendMessage(msg);
	}
	if (lastState.sw03 != state.sw03) {
		snprintf(msg, sizeof(msg), "AP_NAV:%i\n", state.sw03);
		sendMessage(msg);
	}
	if (lastState.sw04 != state.sw04) {
		snprintf(msg, sizeof(msg), "AP_APR:%i\n", state.sw04);
		sendMessage(msg);
	}
	if (lastState.sw05 != state.sw05) {
		snprintf(msg, sizeof(msg), "AP_VS:%i\n", state.sw05);
		sendMessage(msg);
	}
	if (lastState.sw06 != state.sw06) {
		snprintf(msg, sizeof(msg), "AP_ALT:%i\n", state.sw06);
		sendMessage(msg);
	}
	if (lastState.sw07 != state.sw07) {
		snprintf(msg, sizeof(msg), "AP_UP:%i\n", state.sw07);
		sendMessage(msg);
	}
	if (lastState.sw08 != state.sw08) {
		snprintf(msg, sizeof(msg), "AP_DN:%i\n", state.sw08);
		sendMessage(msg);
	}
	if (lastState.long00 != state.long00) {
		snprintf(msg, sizeof(msg), "ALT_ASSIGNED:%li\n", state.long00);
		sendMessage(msg);
	}
}

void loop(){
	if (! checkModeChange()) {
		if (getDiagnosticMode()) {
			diagnosticDisplay();
		} else {
			if (! knowsXPAddr()) {
				showErrorLeds(200);
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
