#include <Arduino.h>
#include <LiquidCrystal.h>
#include <avr/interrupt.h>
#include "communication.h"
#include "box.h"


void actOnXPControlMessage(char* msg) {
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
			if (DEBUG_MODE) {
				Serial.print("message: '"); Serial.print(msg); Serial.println("' - no value field??");
			}
		}
	} else {
		if (DEBUG_MODE) {
			Serial.print("message: '"); Serial.print(msg); Serial.println("' - no msg??");
		}
	}
}


bool actOnAnyXPMessage() {
	char msg[48];
	memset(msg, 0, 48);
	if (getMessage(msg)) {
		if (strcmp(MSG_BOX_FISH, msg) == 0) {
			sendBoxConfig();
		} else {
			actOnXPControlMessage(msg);
		}
		return true;
	}
	return false;
}

void setup() {
	byte mac[] = { MAC_ADDRESS };
	IPAddress ip( IP_ADDRESS );
	IPAddress broadcastAddress( NETWORK_BROADCAST_ADDRESS ); 
	setupEthernet(mac, ip, broadcastAddress, BOXID, BOX_DEFINITIONS);
	boxSetup();
	fishForPlugin();
}


void loop() {
	if (! knowsXPAddr()) {
		noConnectionActions();
		if (actOnAnyXPMessage()) {
			initialiseBox();
		}
		delay(500);
	} else {
		actOnAnyXPMessage();
		updateState();
		if (sendAnyChanges(currentState, prevState)) {
			prevState = currentState;
		}

	}
}

