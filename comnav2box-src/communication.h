#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

//#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define MSG_BOX_FISH "Avduino Box Fish"

#define DEBUG_MODE false

#ifdef __cplusplus
extern "C" {
#endif

	void fishForPlugin(void);

	bool getMessage(char* buf);

	char *getMyIPAddress(void);

	char *getXpIPAddress(void);

	bool knowsXPAddr(void);

	void sendBoxConfig(void);

	void sendMessage(const char* msg);

	void setupEthernet(byte *mac, IPAddress ipa, IPAddress bCast, const char* bid, const char* bDefs);

#ifdef __cplusplus
}
#endif
#endif


