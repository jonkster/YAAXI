#include "communication.h"

byte mac[] =  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip( 192, 168, 0, 178 );   // box ip address
IPAddress xpip( 192, 168, 0, 255 ); // xplane host ip address (initially set to broadcast)
char xpData[UDP_TX_PACKET_MAX_SIZE];

unsigned int boxPort = 8888;
unsigned int xpPort = 8889;
EthernetUDP	Udp;

bool foundXP = false;
bool verbose = true;


void setupEthernet() {
	Ethernet.begin(mac, ip);
	Udp.begin(boxPort);
	memset(xpData, 0, sizeof(xpData));
}

void sendMessage(const char* msg) {
	Udp.beginPacket(xpip, xpPort);
	Udp.write(msg);
	Udp.endPacket();
	
	if (verbose) {
		Serial.print("Sending msg: ");
		Serial.println(msg);
	}
}

bool knowsXPAddr() {
	return foundXP;
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


