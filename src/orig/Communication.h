#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "queues.h"

#define UDP_TX_PACKET_MAX_SIZE 256 //increase UDP size

#define IP_ADDRESS 
#define MAC_ADDRESS 


void queueTx(const char* msg);

char* getNextRx();

bool knowsXPAddr();

int rxQSize();

void setupEthernet();

void serialSetup();

void setParams();

void unloadRxQueue();

void unloadTxQueue();

void updateComms();

#endif

