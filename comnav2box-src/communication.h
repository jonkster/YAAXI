#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

//#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define UDP_TX_PACKET_MAX_SIZE 255 //increase UDP size

/* arduino box network setup */

/* Some random MAC Numbers
40:7b:d8:0b:85:16
9c:a1:cb:e2:6b:47
c5:df:d5:4b:82:58
ff:ce:f5:a2:a2:28
*/

#define MAC_ADDRESS 0xC0,0x90,0x33,0x53,0xB8,0xDF
#define IP_ADDRESS 192, 168, 0, 190
#define IP_ADDRESS_ST "192.168.0.190"

#define XIP_ADDRESS 192, 168, 0, 255

// this is the port the arduino listens on
#define ARDUINO_BOX_PORT 8888

/* arduino sends data to this port to X Plane box */
#define XPLANE_PLUGIN_PORT 8889

#define DEBUG_MODE true

#ifdef __cplusplus
extern "C" {
#endif


	bool getMessage(char* buf);

	char *getXpIPAddress(void);

	bool knowsXPAddr();

	void setupEthernet();

	void sendMessage(const char* msg);


#ifdef __cplusplus
}
#endif
#endif


