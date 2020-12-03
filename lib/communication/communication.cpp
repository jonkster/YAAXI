#include "communication.h"

#define UDP_TX_PACKET_MAX_SIZE 255 //increase UDP size
#define MSG_XP_PLUGIN_FISH "XP Plugin Fish"

// this is the port the arduino listens on
#define ARDUINO_BOX_PORT 8888

// arduino sends data to this port to X Plane box
#define XPLANE_PLUGIN_PORT 8889

IPAddress ip;   // box ip address
IPAddress xpip; // xplane host ip address (initially set to broadcast)
char xpData[UDP_TX_PACKET_MAX_SIZE];

/* box listens on this port for stuff from X Plane box */
unsigned int boxPort = ARDUINO_BOX_PORT;

/* box sends data to this port to X Plane box */
unsigned int xpPort = XPLANE_PLUGIN_PORT;


char* boxId;
char* boxDefs;

EthernetUDP	Udp;

bool foundXP = false;
bool verbose = DEBUG_MODE;

void sendMessage(const char* msg);

void setupEthernet(byte *mac, IPAddress ipa, IPAddress bCast, const char* bid, const char* bDefs) {
	Ethernet.begin(mac, ipa);
	ip = ipa;
	xpip = bCast;
	Udp.begin(boxPort);
	memset(xpData, 0, sizeof(xpData));

	boxId = (char*)malloc(strlen(bid)+1);
	strcpy(boxId, bid);

	boxDefs = (char*)malloc(strlen(bDefs)+1);
	strcpy(boxDefs, bDefs);
}

void sendDataTypeLong(const char* code, long data ) {
	char msg[48];
	snprintf(msg, sizeof(msg), "%s:%ld\n", code, data);
	sendMessage(msg);
}

void sendMessage(const char* msg) {
	if (verbose) {
		Serial.print("Sending msg: ");
		Serial.print(msg);
		Serial.print(" to 192.168.0.");
		Serial.print(xpip[3]);
		Serial.print(" :");
		Serial.print(xpPort);
		Serial.print("... ");
	}
	Udp.beginPacket(xpip, xpPort);
	Udp.write(msg);
	Udp.endPacket();
	if (verbose) {
		Serial.println("sent");
	}
}

bool knowsXPAddr() {
	return foundXP;
}


void fishForPlugin() {
	char msg[120];
	snprintf(msg, sizeof(msg), "%s:%d.%d.%d.%d", MSG_XP_PLUGIN_FISH, ip[0], ip[1], ip[2], ip[3]);
	sendMessage(msg);
}

void sendBoxConfig() {
	char msg[120];
	snprintf(msg, sizeof(msg), "BOXID:%s:%d.%d.%d.%d:%s", boxId, ip[0], ip[1], ip[2], ip[3], boxDefs);
	sendMessage(msg);
}

char *getMyIPAddress(void) {
	char *buf = (char*)malloc(25);
	snprintf(buf, 25, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	return buf;
}

char *getXpIPAddress(void) {
	char *buf = (char*)malloc(25);
	snprintf(buf, 25, "%d.%d.%d.%d", xpip[0], xpip[1], xpip[2], xpip[3]);
	return buf;
}


bool getMessage(char* buf) {
	memset(xpData, 0, sizeof(xpData));
	int packetSize = Udp.parsePacket();
	while (packetSize > 0) {
		char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
		memset(packetBuffer, 0, sizeof(packetBuffer));
		Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
		strncat(xpData, packetBuffer, packetSize);

		if (xpip[3] == 255) {
			// We have been sending on the broadcast address, now we have response
			// get XPlane IP address
			if (strcmp("Avduino Box Fish", xpData) == 0) {
				IPAddress remote = Udp.remoteIP();
				xpip[3] = remote[3];
				if (verbose) {
					Serial.print("found XPlane @"); Serial.print(getXpIPAddress()); Serial.println(xpip[3]);
				}
				foundXP = true;
			}
		}
		if (verbose) {
			IPAddress remote = Udp.remoteIP();
			char remoteSt[20];
			snprintf(remoteSt, sizeof(remoteSt)-1, "%d.%d.%d.%d", remote[0], remote[1], remote[2], remote[3]);
			Serial.print("Received packet of size "); Serial.print(packetSize);
			Serial.print(" From "); Serial.print(remoteSt);
			Serial.print(", port "); Serial.println(Udp.remotePort());
			Serial.print("Contents:"); Serial.println(xpData);
		}
		packetSize = Udp.available();
	}
	strncat(buf, xpData, strlen(xpData));
	return (strlen(xpData) > 0);

}


