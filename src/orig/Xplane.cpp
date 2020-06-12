#include <Arduino.h>
#include <string.h>
#include "Xplane.h"
#include "./Communication.h"

#define MAX_REGS 64
#define MAX_REG_LENGTH 64
#define MAX_HANDLERS 16


drConfig	xpDataHandlers[MAX_HANDLERS];
unsigned int	currentXPHandlerIdx = 0;
unsigned int	xpHandlerCount = 0;

drConfig	setXPHandlers[MAX_HANDLERS];
unsigned int	setXPHandlersIdx = 0;
unsigned int	setXPHandlerCount = 0;

void addGetXPReg(const char* regPath, const xpResponseHandler handler) {
	drConfig entry = { -1, regPath, handler, NULL, NULL };
	if (xpHandlerCount < MAX_HANDLERS) {
		entry.id = xpHandlerCount;
		xpDataHandlers[xpHandlerCount++] = entry;
	}
	if (xpHandlerCount >= MAX_HANDLERS) {
		Serial.println("Handler registrations @ maximum");
		xpHandlerCount--;
	}
}

void addSetXPReg(const char* regPath, const getArduinoDataHandler handler) {
	drConfig entry = { -1, regPath, NULL, handler, NULL };
	if (setXPHandlerCount < MAX_HANDLERS) {
		entry.id = setXPHandlerCount;
		setXPHandlers[setXPHandlerCount++] = entry;
	}
	if (setXPHandlerCount >= MAX_HANDLERS) {
		Serial.println("Handler registrations @ maximum");
		setXPHandlerCount--;
	}
}

void debugHandler(const char* name, const char* path, const char* val) {
	Serial.print(name);
	Serial.print(" debug handler: ");
	Serial.print(path);
	Serial.print(" = ");
	Serial.println(val);
}


void queueNextXPRequest() {
	const char *reg = xpDataHandlers[currentXPHandlerIdx++].drPath;
	queueTx(reg);
	if (currentXPHandlerIdx >= xpHandlerCount) {
		currentXPHandlerIdx = 0;
	}
}

drConfig findHandlerMatch(const char prefix, const char* reg, const char* type) {
	char target[64];
	snprintf(target, sizeof(target) - 1, "%c:%s:%s", prefix, reg, type);
	for (unsigned int i = 0; i < xpHandlerCount; i++) {
		if (strncmp(xpDataHandlers[i].drPath, target, sizeof(target)) == 0) {
			return xpDataHandlers[i];
		}
	}
	return { -1, NULL,  NULL, NULL, NULL };

}

drConfig makeEntryFromXPValue(const char* xpValue, drConfig entry) {
	char *p = strdup(xpValue);
	char *tmp = p;
	if (tmp == NULL) {
		Serial.print("makeEntryFromXPValue: ERROR! OUT OF MEMORY!"); Serial.println(xpValue);
	}
	char* prefix = strsep(&tmp, ":");
	if (tmp != NULL) {
		if (strncmp(prefix, "V", 1) == 0) {
			char* reg = strsep(&tmp, ":");
			if (tmp != NULL) {
				char* type = strsep(&tmp, ":");
				if (tmp != NULL) {
					char* value = strsep(&tmp, ":");
					if (value != NULL) {
						entry =  findHandlerMatch('G', reg, type);
						if (entry.id != -1) {
							entry.value = strdup(value);
							if (entry.value == NULL) {
								Serial.print("makeEntryFromXPValue 2: ERROR! OUT OF MEMORY!"); Serial.println(value);
							}
						} else {
							Serial.print("Cannot find handler for: "); Serial.println(xpValue);
						}
					} else {
						Serial.print("no value in response: "); Serial.println(xpValue);
					}
				} else {
					Serial.print("no type in response: "); Serial.println(xpValue);
				}	
			} else {
				Serial.print("no reg path in response: "); Serial.println(xpValue);
			}
		} else {
			Serial.print("incorrect prefix in response: "); Serial.println(xpValue);
		}
	} else {
		Serial.print("cannot parse response: "); Serial.println(xpValue);
	}
	free(p);
	return entry;
}

void updateArduino(const char* xpValue) {
	// eg:
	// V:sim/cockpit/radios/com1_stdby_freq_hz:i:11910

	drConfig entry = {-1, NULL,  NULL, NULL, NULL };
	entry = makeEntryFromXPValue(xpValue, entry);
	if (entry.id != -1) {
		entry.handler(entry.value);
		if (entry.value != NULL) {
			free(entry.value);
		}
	} else {
		Serial.print("Cannot find handler for XP response:"); Serial.println(xpValue);
	}
}

void actOnXPResponses() {
	char *rx = getNextRx();
	if (rx != NULL) {
		updateArduino(rx);
		free(rx);
	} 
}

void pushBoxToXP() {
	for (unsigned int i = 0; i < setXPHandlerCount; i++) {
		char* v = setXPHandlers[i].getDataHandler();
		if (v == NULL) {
			continue;
		}
		// only push to XP if new value
		if (strncmp(v, setXPHandlers[i].value, strlen(v)) != 0) {
			if (setXPHandlers[i].value != NULL) {
				free(setXPHandlers[i].value);
			}
			setXPHandlers[i].value = v;

			char buf[64];
			snprintf(buf, sizeof(buf), "%s:%s", setXPHandlers[i].drPath, setXPHandlers[i].value);
			queueTx(buf);
		} else {
			if (v != NULL) {
				free(v);
			}
		}

	}
}

