#include "communication.h"

byte mac[] =  { MAC_ADDRESS };
IPAddress ip( IP_ADDRESS );   // box ip address
IPAddress xpip( 192, 168, 0, 255 ); // xplane host ip address (initially set to broadcast)
char xpData[UDP_TX_PACKET_MAX_SIZE];

/* box listens on this port for stuff from X Plane box */
unsigned int boxPort = ARDUINO_BOX_PORT;
/* box sends data to this port to X Plane box */
unsigned int xpPort = XPLANE_PLUGIN_PORT;


EthernetUDP	Udp;

bool foundXP = false;
bool verbose = DEBUG_MODE;


void setupEthernet() {
	Ethernet.begin(mac, ip);
	Udp.begin(boxPort);
	memset(xpData, 0, sizeof(xpData));
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

char *getXpIPAddress(void) {
	char *buf = malloc(25);
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
			// get XPlane IP address
			if (strcmp("Avduino Box Fish", xpData) == 0) {
				IPAddress remote = Udp.remoteIP();
				xpip[3] = remote[3];
				if (verbose) {
					Serial.print("found XPlane @192.168.0."); Serial.println(xpip[3]);
				}
				foundXP = true;
			} else if (strncmp("LED", xpData, 3) == 0) {
				IPAddress remote = Udp.remoteIP();
				xpip[3] = remote[3];
				if (verbose) {
					Serial.print("found XPlane @192.168.0."); Serial.println(xpip[3]);
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


