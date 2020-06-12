#include <Arduino.h>
#include "config.h"
#include "Xplane.h"
#include "Communication.h"
#include "switches.h"

char* setArduinoGearUnsafe(const char* val);

char* setArduinoGearLocked(const char* val);

char* setArduinoXPHSISrc(const char* val);

char* setArduinoAutopilotHdg(const char* val);

char* setArduinoOBS(const char* val);

char* setXPFuelPump();

char* setXPFuelTank();

char* setXPAutopilotHdg();

char* setXPOBS();

char* setXPHSISrc();

char* getValAsSt(const int val);

void addGetXPRegs() {
	addGetXPReg("G:sim/cockpit/warnings/annunciators/gear_unsafe:i", &setArduinoGearUnsafe );
	addGetXPReg("G:sim/aircraft/parts/acf_gear_deploy:vf", &setArduinoGearLocked );
	addGetXPReg("G:sim/cockpit/switches/HSI_selector:i", &setArduinoXPHSISrc );
	addGetXPReg("G:sim/cockpit/autopilot/heading:f", &setArduinoAutopilotHdg);
	addGetXPReg("G:sim/cockpit2/radios/actuators/hsi_obs_deg_mag_pilot:f", &setArduinoOBS);
}

void addSetXPRegs() {
	addSetXPReg("S:sim/cockpit/engine/fuel_pump_on:vi", &setXPFuelPump);
	addSetXPReg("S:sim/cockpit/engine/fuel_tank_selector:i", &setXPFuelTank);
	addSetXPReg("S:sim/cockpit/autopilot/heading:f", &setXPAutopilotHdg);
	addSetXPReg("S:sim/cockpit2/radios/actuators/hsi_obs_deg_mag_pilot:f", &setXPOBS);
	addSetXPReg("S:sim/cockpit/switches/HSI_selector:i", &setXPHSISrc);
}

char* setArduinoGearUnsafe (const char* val) {
       	if (strncmp(val, "1", 1) == 0) {
			lightSet(1, HIGH);
	} else {
			lightSet(1, LOW);
	}
	return (char*)val;
}

char* setArduinoGearLocked (const char* val) {
	// value looks like: [0.5,0.5,0.5....]
       	if (strncmp(val, "[1.0", 4) == 0) {
		lightSet(0, HIGH);
	} else {
		lightSet(0, LOW);
	}
	return (char*) val;
}

int autopilotHdg = 0;
char* setArduinoAutopilotHdg(const char* val) {
	int v = atoi(val);
	autopilotHdg = v;
	return (char*) val;
}

float obs = 0;
char* setArduinoOBS(const char* val) {
	float v = atof(val);
	obs = v;
	return (char*) val;
}

int hsiSrc = 0;
char* setArduinoXPHSISrc(const char* val) {
	int v = atoi(val);
	hsiSrc = v;
	return (char*) val;
}

char* setXPFuelPump () { 
	int state = 0;
	if (getSwitchState(TOGGLE0_IDX)) {
		state = 2;
	} else if (getSwitchState(TOGGLE1_IDX)) {
		state = 1;
	}
	char buf[32];
       	snprintf(buf, sizeof(buf), "[%i, 0, 0, 0, 0, 0, 0, 0]", state);
       	return strdup(buf);
}

char* setXPFuelTank () {
	int twistState0 = getSwitchState(TWIST0_IDX);
	int twistState1 = getSwitchState(TWIST1_IDX);
	int s = 0;
	if (twistState0 == 0) {
		s = 1;
	} else if (twistState1 == 0) {
		s = 3;
	}
	char buf[32];
       	snprintf(buf, sizeof(buf), "%i", s);
       	return strdup(buf);
}

char* setXPAutopilotHdg () { 
	int rot = getLastRot(1);
	if (rot != 0) {
		autopilotHdg += rot;
		char buf[32];
		snprintf(buf, sizeof(buf), "%i", autopilotHdg);
		return strdup(buf);
	}
	return NULL;
}

char* setXPOBS () { 
	int rot = getLastRot(0);
	if (rot != 0) {
		obs += rot;
		int obsi = obs;
		char buf[32];
		snprintf(buf, sizeof(buf), "%i", obsi);
		return strdup(buf);
	}
	return NULL;
}

char* setXPHSISrc () { 
	if ((getSwitchChanged(PUSH2_IDX)) && (getSwitchState(PUSH2_IDX) == 0)) {
		if (hsiSrc == 0) {
			hsiSrc = 2;
		} else if (hsiSrc == 2) {
			hsiSrc = 0;
		}
		char buf[5];
		snprintf(buf, sizeof(buf), "%i", hsiSrc);
		return strdup(buf);
	}
	return NULL;
}

char* getValAsSt(const int val) {
	char buf[64];
	snprintf(buf, sizeof(buf), "%i", val);
	return strdup(buf);
}

