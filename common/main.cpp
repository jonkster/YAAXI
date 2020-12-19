#include <Arduino.h>
#include "communication.h"
#include "box.h"

void actOnXPControlMessage(char* msg) {
	char seps[] = ":";
	char* token;
	token = strtok(msg, seps);
	if (token != NULL) {
		char device[16];
		strncpy(device, token, 15);
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
	Serial.begin(115200);
	byte mac[] = { MAC_ADDRESS };
	IPAddress ip( IP_ADDRESS );
	byte gw[] = { GATEWAY_ADDRESS };
	byte mask[] = { NETMASK };
	IPAddress broadcastAddress( NETWORK_BROADCAST_ADDRESS ); 
	setupEthernet(mac, ip, gw, mask, broadcastAddress, BOXID, BOX_DEFINITIONS);
	boxSetup();
	fishForPlugin();
}


void loop() {
	if (! knowsXPAddr()) {
		if (actOnAnyXPMessage()) {
			setStartState();
			Serial.println("found plugin");
		} else {
			Serial.println("plugin not found, wait");
			noConnectionActions();
			delay(2000);
		}
	} else {
		actOnAnyXPMessage();
		boxMainLoop();
	}
}

