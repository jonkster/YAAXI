#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define UDP_TX_PACKET_MAX_SIZE 255 //increase UDP size

#define IP_ADDRESS 
#define MAC_ADDRESS 

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


