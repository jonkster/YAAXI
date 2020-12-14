#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <stdbool.h>

#define MSG_BOX_FISH "Avduino Box Fish"

#define DEBUG_MODE true

#ifdef __cplusplus
extern "C" {
#endif

	void fishForPlugin(void);

	bool getMessage(char* buf);

	char *getMyIPAddress(void);

	char *getXpIPAddress(void);

	bool knowsXPAddr(void);

	void sendBoxConfig(void);

	void sendDataTypeBool(const char* code, bool data);

	void sendDataTypeInt(const char* code, int data);

	void sendDataTypeLong(const char* code, long data);

	void sendMessage(const char* msg);

	void setupEthernet(byte *mac, IPAddress ipa, byte *gw, byte* mask, IPAddress bCast, const char* bid, const char* bDefs);

#ifdef __cplusplus
}
#endif
#endif


