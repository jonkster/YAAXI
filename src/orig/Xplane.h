#ifndef __XPLANE_H__
#define __XPLANE_H__
#ifdef __cplusplus

#include "queues.h"

extern "C" {
#endif

// handler function for dealing with XP data
typedef char* (*xpResponseHandler)(const char* val);
typedef char* (*getArduinoDataHandler)();

/* the type that stores details of XP data regs and handlers */
typedef struct {
	int			id;		// unique id#
	char const		*drPath;	// XP dataRef string
	xpResponseHandler	handler;	// callback function to set arduino data
	getArduinoDataHandler	getDataHandler;	// callback function to get arduino data
	char*			value;
} drConfig;


void addGetXPReg(const char* regPath, const xpResponseHandler handler);

void addSetXPReg(const char* regPath, const getArduinoDataHandler handler);

void debugHandler(const char* name, const char* path, const char* val);

void queueNextXPRequest();

void actOnXPResponses();

void pushBoxToXP();

#ifdef __cplusplus
}
#endif
#endif
