#include "./queues.h"
#include <stdlib.h>
#include <string.h>
#include <Arduino.h>


tQueue makeNewQueue(const char* name, const unsigned int qSize, const unsigned int maxMsgLength) {
	tQueue tmp;
	tmp.name = strdup(name);
	tmp.qSize = qSize;
	tmp.maxMsg = maxMsgLength;
	tmp.idx = 0;
	tmp.queue = (char**) calloc(qSize, sizeof(char*));
	return tmp;
}

void destroyQueue(const tQueue &q) {
	for (unsigned int i = 0; i < q.qSize; i++) {
		free(q.queue[i]);
	}
	free(q.queue);
}

void unloadQueue(tQueue &q) {
	if (q.idx > 0) {
		for (unsigned int i = 0; i < q.idx; i++) {
			if (q.queue[i] != NULL) {
				free(q.queue[i]);
			}
		}
		q.idx = 0;
	}
}


void queueMsg(tQueue &q, const char* msg) {
	if (msg == NULL) {
		Serial.println("NULL MSG??");
		return;
	}
	char *v = strdup(msg);
	if (v == NULL) {
		Serial.print("queue MSG: ERROR! OUT OF MEMORY!"); Serial.println(msg);
		return;
	}
	q.queue[q.idx] = v;
	q.idx++;
	if (q.idx >= q.qSize) {
		q.idx--;
		Serial.println("!!!queue limit exceed!!!");
		for (unsigned int i = 0; i < (q.qSize-1); i++) {
			free(q.queue[i]);
			q.queue[i] = strdup(q.queue[i+1]);
		}
	}
}

void debugQ(tQueue q) {
	Serial.print("------- ");
	Serial.print(q.name);
	Serial.print(" (");
	Serial.print(q.idx);
	Serial.print(" recs)");
	Serial.println(" Dump  -----------------------");
	Serial.println(" idx :    value");
	Serial.println("---------------------------------------");
	for (unsigned int i = 0; i < q.idx; i++) {
		Serial.print("#"); Serial.print(i); Serial.print(" : "); Serial.println(q.queue[i]);
	}
	Serial.println("---------------------------------------");
}

char* grabFromQueue(tQueue &q) {
	if (q.idx > 0) {
		q.idx--;
		return q.queue[q.idx];
	} else {
		return NULL;
	}
}

char* grabOldestFromQueue(tQueue &q) {
	if (q.idx > 0) {
		char* oldest = q.queue[0];
		for (unsigned int i = 1; i < q.idx; i++) {
			q.queue[i-1] = q.queue[i];
		}
		q.idx--;
		return oldest;
	} else {
		return NULL;
	}
}
