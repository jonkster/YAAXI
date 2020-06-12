#include "./Communication.h"

byte mac[] =  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEA };
IPAddress ip( 192, 168, 0, 178 );
IPAddress xpip( 192, 168, 0, 255 );
unsigned int localPort = 8888;
unsigned int xpPort = 8889;
EthernetUDP	Udp;

bool verbose = false;
bool foundXP = false;

tQueue udpRxQ;
tQueue udpTxQ;

#define TX_UDP_QUEUE_SIZE 32
#define RX_UDP_QUEUE_SIZE 32
#define MAX_MESSAGE_LENGTH 32

void setupEthernet() {
	Ethernet.begin(mac, ip);
	Udp.begin(localPort);
	udpRxQ = makeNewQueue("RX Queue", RX_UDP_QUEUE_SIZE, MAX_MESSAGE_LENGTH);
	udpTxQ = makeNewQueue("TX Queue", TX_UDP_QUEUE_SIZE, MAX_MESSAGE_LENGTH);
}


void sendMsg(const char* msg) {
	IPAddress xpip(192, 168, 0, 255);
	Udp.beginPacket(xpip, 8889);
	Udp.write(msg);
	Udp.endPacket();
	if (verbose) {
		Serial.print("sending:"); Serial.println(msg);
	}
}

bool knowsXPAddr() {
	return foundXP;
}

void readReply() {
	char data[UDP_TX_PACKET_MAX_SIZE];
	memset(data, 0, sizeof(data));
	int packetSize = Udp.parsePacket();
	while (packetSize > 0) {
		char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
		memset(packetBuffer, 0, sizeof(packetBuffer));
		Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
		strncat(data, packetBuffer, packetSize);

		if (xpip[3] == 255) {
			IPAddress remote = Udp.remoteIP();
			xpip[3] = remote[3];
			Serial.print("found XPlane @192.168.0."); Serial.println(xpip[3]);
			foundXP = true;
		}
		if (verbose) {
			IPAddress remote = Udp.remoteIP();
			char remoteSt[20];
			snprintf(remoteSt, sizeof(remoteSt)-1, "%d.%d.%d.%d", remote[0], remote[1], remote[2], remote[3]);
			Serial.print("Received packet of size "); Serial.print(packetSize);
			Serial.print(" From "); Serial.print(remoteSt);
			Serial.print(", port "); Serial.println(Udp.remotePort());
			Serial.print("Contents:"); Serial.println(data);
		}
		packetSize = Udp.available();
	}
	if (strlen(data) > 0) {
		queueMsg(udpRxQ, data);
	}
}

void unloadRxQueue() {
	unloadQueue(udpRxQ);
}

void unloadTxQueue() {
	unloadQueue(udpTxQ);
}

void queueTx(const char* msg) {
	queueMsg(udpTxQ, msg);
}

int rxQSize() {
	return udpRxQ.idx;
}

char* getNextRx() {
	char * msg = grabOldestFromQueue(udpRxQ);
	return msg;
}

void updateComms() {
	for (int i = 0; i < 4; i++) {
		char* nextMsg = grabOldestFromQueue(udpTxQ);
		if (nextMsg != NULL) {
			sendMsg(nextMsg);
			free(nextMsg);
			readReply();
		} 
	}
}


