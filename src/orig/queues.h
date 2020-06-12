#ifndef __QUEUES_H__
#define __QUEUES_H__

typedef struct {
	unsigned int  qSize;
	unsigned int  maxMsg;
	unsigned int  idx;
	char*	      name;
	char** queue;
} tQueue;

tQueue makeNewQueue(const char* name, const unsigned int qSize, const unsigned int maxMsgLength);

void destroyQueue(const tQueue &q);

void debugQ(tQueue q);

void queueMsg(tQueue &q, const char* msg);

char* grabFromQueue(tQueue &q);

char* grabOldestFromQueue(tQueue &q);

void unloadQueue(tQueue &q);

#endif
