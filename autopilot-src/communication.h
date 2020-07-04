#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define UDP_TX_PACKET_MAX_SIZE 255 //increase UDP size

/* arduino box network setup */
#define MAC_ADDRESS 0xAA, 0xAA, 0xBE, 0xEF, 0xFE, 0xED
#define IP_ADDRESS 192, 168, 0, 182
#define ARDUINO_BOX_PORT 8888

/* box sends data to this port to X Plane box */
#define XPLANE_PLUGIN_PORT 8889

#define DEBUG_MODE true

#ifdef __cplusplus
extern "C" {
#endif


	bool getMessage(char* buf);

	bool knowsXPAddr();

	void setupEthernet();

	void sendMessage(const char* msg);


#ifdef __cplusplus
}
#endif
#endif


