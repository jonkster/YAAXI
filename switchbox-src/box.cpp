#include <Arduino.h>
#include "hardware.h"
#include "communication.h"

#define MAX_TASKS 10
#define LAST_ADJUST_SECS 1
#define BOXID "BOXID:Motion Capture-Switch Box v0-deadbeeffeed"
#define BOX_DEFINITIONS "TOGGLE0-CB,TOGGLE1-CB,PUSH0-CB,PUSH1-CB,PUSH2-CB,PUSH3-CB,TWIST0-CB,TWIST1-CB,KNOB0-CI,KNOB1-CI,LED0-DB,LED1-DB,LED2-DB,LED3-DB,LED4-DB,LED5-DB,"
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
	if (lastState.sw0 != state.sw0) {
		snprintf(msg, sizeof(msg), "TOGGLE0:%i\n", state.sw0);
		sendMessage(msg);
	}
	if (lastState.sw1 != state.sw1) {
		snprintf(msg, sizeof(msg), "TOGGLE1:%i\n", state.sw1);
		sendMessage(msg);
	}
	if (lastState.sw2 != state.sw2) {
		snprintf(msg, sizeof(msg), "PUSH0:%i\n", state.sw2);
		sendMessage(msg);
	}
	if (lastState.sw3 != state.sw3) {
		snprintf(msg, sizeof(msg), "PUSH1:%i\n", state.sw3);
		sendMessage(msg);
	}
	if (lastState.sw4 != state.sw4) {
		snprintf(msg, sizeof(msg), "PUSH2:%i\n", state.sw4);
		sendMessage(msg);
	}
	if (lastState.sw5 != state.sw5) {
		snprintf(msg, sizeof(msg), "PUSH3:%i\n", state.sw5);
		sendMessage(msg);
	}
	if (lastState.sw6 != state.sw6) {
		snprintf(msg, sizeof(msg), "TWIST0:%i\n", state.sw6);
		sendMessage(msg);
	}
	if (lastState.sw7 != state.sw7) {
		snprintf(msg, sizeof(msg), "TWIST1:%i\n", state.sw7);
		sendMessage(msg);
	}
	if (lastState.knob0 != state.knob0) {
		int delta = state.knob0 - lastState.knob0;
		if (delta > 1) {
			snprintf(msg, sizeof(msg), "KNOB0:%i\n", 1);
			sendMessage(msg);
		} 
		if (delta > 0) {
			snprintf(msg, sizeof(msg), "KNOB0:%i\n", 1);
			sendMessage(msg);
		} 
	       	if (delta < -1) {
			snprintf(msg, sizeof(msg), "KNOB0:%i\n", 0);
			sendMessage(msg);
		} 
	       	if (delta < 0) {
			snprintf(msg, sizeof(msg), "KNOB0:%i\n", 0);
			sendMessage(msg);
		}
	}
	if (lastState.knob1 != state.knob1) {
		int delta = state.knob1 - lastState.knob1;
		if (delta > 1) {
			snprintf(msg, sizeof(msg), "KNOB1:%i\n", 1);
			sendMessage(msg);
		} 
		if (delta > 0) {
			snprintf(msg, sizeof(msg), "KNOB1:%i\n", 1);
			sendMessage(msg);
		} 
	       	if (delta < -1) {
			snprintf(msg, sizeof(msg), "KNOB1:%i\n", 0);
			sendMessage(msg);
		} 
	       	if (delta < 0) {
			snprintf(msg, sizeof(msg), "KNOB1:%i\n", 0);
			sendMessage(msg);
		}
	}
	lastState = state;
}

int secondDelayCount = 0; 
#define DELAY_FISH_SECS 20
void loop(){
	if (! checkModeChange()) {
		if (getDiagnosticMode()) {
			diagnosticDisplay();
		} else {
			if (! knowsXPAddr()) {
				showErrorLeds(200);
				// trawl for a suitable XP plugin
				delay(1000);
				if ((secondDelayCount++ % DELAY_FISH_SECS) > (DELAY_FISH_SECS - 5)) {
					sendMessage(MSG_XP_PLUGIN_FISH);
				}
			} else {
				// we now know there is an XP plugin, send any changes to it
				if (boxStateChanged()) {
					struct tBoxState state = getBoxState();
					sendAnyChanges(state);
					displayBoxState(state);
				}

			}
			// listen for (and act on) any XP plugin responses
			actOnAnyXPMessage();
		}
	}
}
