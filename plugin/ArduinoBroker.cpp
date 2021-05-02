#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>
#include <netdb.h> 
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "XPLMDataAccess.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMMenus.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
//#include "XPStandardWidgets.h"
//#include "XPWidgets.h"


#if IBM
	#include <windows.h>
#endif
#if LIN
	#include <GL/gl.h>
#elif __GNUC__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif
#ifndef XPLM300
	#error This is made to be compiled against the XPLM300 SDK
#endif


#define ARDUINO_WRITE_PORT 8888 // we write to arduino(s) on this port
#define ARDUINO_READ_PORT 8889 // we listen to arduino(s) on this port
//#define ARDUINO_IP "192.168.0.178"

#define CMD_READINI +9999998
#define CMD_PAUSEPI +9999997
#define CMD_PAUSEXP 0

#define INI_DEFAULT_PATH "./Resources/plugins/ArduinoBroker/box.ini"

//#define REFRESH_RATE_SECS  0.1
#define REFRESH_RATE_SECS  0.005

char ardLog[50][128] = {
	"",
	"",
	"",
	"",
	"",
	"",
	"end"
};
bool		callBackActive = false;

bool		paused = true;
bool		sendAll = false;
int		readSock = -1;
int		writeSock = -1;
struct		sockaddr_in listenSockAddr;
struct		sockaddr_in talkSockAddr;

float		flightLoopCallback(float, float, int, void*);
int		initialiseSocket(const char* ip, const int port, int& sock, struct sockaddr_in& server, bool bindToPort);
void		addLogMessage(std::string, const char*);
void		addLogMessage(std::string, const int);
void		addLogMessage(const char*, const char*);
void		addLogMessageInt(const char* msg, const int extra);
void		closeSocket(const int);
void		handleBoxIdMsg(const char* msg);
static void	menuHandlerCallback(void*, void*); 
void		parseAddressEntry(std::string);
void		parseControlEntry(std::string);
void		parseDataRefEntry(std::string line);
void		parseDisplayEntry(std::string);
void		parseInitialEntry(std::string);
bool 		readIniFile();
void 		sendAnyChangesToBox();

int		counter;

PLUGIN_API int XPluginStart( char *  outName, char *  outSig, char *  outDesc)
{
	XPLMMenuID	myMenu;
	int		mySubMenuItem;

	strncpy(outName, "YAAXI ArduinoBroker", sizeof(outName) - 1);
	strncpy(outSig,  "kellavia.interface.arduinobroker", sizeof(outSig) - 1);
	strncpy(outDesc, "A protoype plugin that talks to Arduino based gadgets using YAAXI protocol.", sizeof(outDesc) - 1);

	// setup Menu
	mySubMenuItem = XPLMAppendMenuItem(
			XPLMFindPluginsMenu(),	/* Put in plugins menu */
			"YAAXI Arduino Broker",	/* Item Title */
			0,			/* Item Ref */
			1);			/* Force English */

	myMenu = XPLMCreateMenu(
			"YAAXI Arduino Broker", 
			XPLMFindPluginsMenu(), 
			mySubMenuItem, 			/* Menu Item to attach to. */
			menuHandlerCallback,		/* The handler */
			0);				/* Handler Ref */

	// Menu Lines
	XPLMAppendMenuItem(myMenu, "Pause XPlane", (void *) CMD_PAUSEXP, 1);
	XPLMAppendMenuItem(myMenu, "Pause/Unpause Plugin", (void *) CMD_PAUSEPI, 1);
	XPLMAppendMenuItem(myMenu, "(Re)Read INI file", (void *) CMD_READINI, 1);

	if (! readIniFile()) {
		addLogMessage("could not read ini file", INI_DEFAULT_PATH);
	}

	if (initialiseSocket(NULL, ARDUINO_READ_PORT, readSock, listenSockAddr, true) < 0) {
		addLogMessage("Could not initialise arduino reading socket?", "");
	} else {
		addLogMessage("Initialised arduino reading socket", "");
	}

	/*if (initialiseSocket(ARDUINO_IP, ARDUINO_WRITE_PORT, writeSock, talkSockAddr, false) < 0) {
		addLogMessage("Could not initialise arduino writing socket?", "");
	} else {
		addLogMessage("Initialised arduino writing socket", "");
	}*/

	// main loop
	XPLMRegisterFlightLoopCallback(
			flightLoopCallback,
			REFRESH_RATE_SECS,
			NULL);


	addLogMessage("Plugin Ready", "");
	paused = false;
	return 1;
}


PLUGIN_API void	XPluginStop(void) {
       	XPLMUnregisterFlightLoopCallback(flightLoopCallback, NULL);
	if (readSock != -1) {
		closeSocket(readSock);
	}
	if (writeSock != -1) {
		closeSocket(writeSock);
	}
}
PLUGIN_API void XPluginDisable(void) { }
PLUGIN_API int XPluginEnable(void) { return 1; }
PLUGIN_API void XPluginReceiveMessage( XPLMPluginID inFromWho, int inMessage, void * inParam) { }

/******************************************************************************************************/
#include <bits/stdc++.h>
std::map<std::string,  std::string> ardIPs; 

std::map<std::string,  XPLMCommandRef> controlConfigCommandOn; 
std::map<std::string,  XPLMCommandRef> controlConfigCommandOff; 

std::map<std::string,  XPLMDataRef> dataRefs; 
std::map<std::string,  XPLMDataRef> writeToDataRefs; 
std::map<std::string,  std::string> boxDevices; 
std::map<std::string,  int> dataRefIndex; 
std::map<std::string,  std::string> dataRefLogic; 
std::map<std::string,  std::string> lastValue; 


std::map<std::string, int> refPreviousValueInt;
std::map<std::string, XPLMDataRef> deviceConfig;

void closeSocket(const int sock) {
	if (sock >= 0) {
		close(sock);
	}
}

int initialiseSocket(const char* ip, const int port, int& sock, struct sockaddr_in &server, bool bindToPort) {
	sock = -1;
	char errMsg[256];
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		strncpy(errMsg, "Error: Socket?", sizeof(errMsg) - 1);
		addLogMessage(errMsg, " 10");
		return -1;
	}

	/* Set up the server name */
	bzero((char *)&server, sizeof(server));
	server.sin_family      = AF_INET;
	server.sin_port        = htons(port);
	if (ip == NULL) {
		addLogMessage("adding broadcast ip to listen for boxes: ", INADDR_ANY);
		server.sin_addr.s_addr = htonl(INADDR_ANY);
	} else {
		//addLogMessage("adding specific ip: ", ip);
		server.sin_addr.s_addr = inet_addr(ip);
	}

	if (bindToPort) {
		if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
			addLogMessage("error binding socket to port", "");
			return -1;
		}
	}
	return 0;
}

void sendArduino(const char* boxid, const char* msg, int writeSock, struct sockaddr_in talkSockAddr) {
	int res = sendto(
			writeSock,
			msg,
			(strlen(msg)+1),
			0,
			(struct sockaddr *)&talkSockAddr,
			sizeof(talkSockAddr));
	if (res < 0) {
		addLogMessage("failed to send to box. Error#", strerror(errno));
	} else {
		std::stringstream lmsg("");
		lmsg << "sending to box: " << boxid << " data: " << msg;
		addLogMessage(lmsg.str().c_str(), "");
	}
}

void sendArduinoAtAddress(struct sockaddr_in cliAddr, const char* msg) {
	int writeSock = -1;
	char* addr = inet_ntoa(cliAddr.sin_addr);
	struct sockaddr_in talkSockAddr;
	if (initialiseSocket(addr, ARDUINO_WRITE_PORT, writeSock, talkSockAddr, false) < 0) {
		addLogMessage("failed to initialise addr. ", addr);
	} else {
		sendArduino(addr, msg, writeSock, talkSockAddr);
		closeSocket(writeSock);
		return;
	}
	addLogMessage("failed to send to address: ", addr);
}

void sendArduinoBox(std::string boxName, const char* msg) {
	std::map<std::string, std::string>::iterator it;
	it = ardIPs.find(boxName);
	if (it != ardIPs.end()) {
		std::string ipAddress = it->second;
		int writeSock = -1;
		struct sockaddr_in talkSockAddr;
		if (initialiseSocket(ipAddress.c_str(), ARDUINO_WRITE_PORT, writeSock, talkSockAddr, false) < 0) {
			addLogMessage("failed to initialise box addr. ", ipAddress.c_str());
		} else {
			sendArduino(boxName.c_str(), msg, writeSock, talkSockAddr);
			closeSocket(writeSock);
			return;
		}
	}
	addLogMessage("failed to send to box: ", boxName.c_str());
}


void handlePluginFishMsg(const char* msg, struct sockaddr_in cliAddr) {
	sendArduinoAtAddress(cliAddr, "Avduino Box Fish");
}

void handleBoxIdMsg(const char* msg) {
	addLogMessage("BOX Config: ", msg);
}

void handleBoxChangeMsg(const char* msg) {
	std::string control;
	std::string s(msg);
	int pos = 0;
	if ((pos = s.find(":")) != std::string::npos) {
		control = s.substr(0, pos);
		s.erase(0, pos + 1);
	}

	std::map<std::string, XPLMCommandRef>::iterator v;
	try {
		if (std::stoi(s) == 1) {
			v = controlConfigCommandOn.find(control);
			if (v != controlConfigCommandOn.end()) {
				XPLMCommandOnce(v->second);
				addLogMessage(control.c_str(), "on");
			}
		} else if (std::stoi(s) == 0) {
			v = controlConfigCommandOff.find(control);
			if (v != controlConfigCommandOff.end()) {
				XPLMCommandOnce(v->second);
				addLogMessage(control.c_str(), "off");
			}
		} else {
			addLogMessage("got empty value??:", s.c_str());
		}
	} catch(...) {
			addLogMessage("got non numerical value??:", s.c_str());
	}
}



void speakMsg(const char *msg) {
	std::string words(msg);
	int pos;
	if ((pos = words.find(":")) != std::string::npos) {
		words.erase(0, pos + 1);
		XPLMSpeakString(words.c_str());
	}
}

void actOnBoxMessage(const int sock, const char* msg, struct sockaddr_in cliAddr) {
	if (strlen(msg) == 0) {
		// null msg??
	} else if (strncmp(msg, "XP Plugin Fish", 14) == 0) {
		// box is fishing for an XPlane plugin
		handlePluginFishMsg(msg, cliAddr);
	} else if (strncmp(msg, "BOXID:", 6) == 0) {
		// box has sent details about itself
		handleBoxIdMsg(msg);
	} else if (strncmp(msg, "update", 6) == 0) {
		sendAll = true;
	} else if (strncmp(msg, "SPEAK:", 6) == 0) {
		speakMsg(msg);
	} else {
		handleBoxChangeMsg(msg);
	}
}

void listenForArduinos(const int sock) {
	struct sockaddr_in cliaddr;
	socklen_t len = sizeof(cliaddr);
	char rxBuf[2048];
	int result = recvfrom(
			sock,
			(char *)rxBuf,
			(sizeof(rxBuf)+1),
			MSG_DONTWAIT,
			(struct sockaddr *) &cliaddr,
			&len);

	if (result > 0) {
		rxBuf[result] = '\0';
		addLogMessage("got message from box:", rxBuf);
		actOnBoxMessage(sock, rxBuf, cliaddr);
	} else if (result == 0) {
		addLogMessage("got empty msg??:", "");
	} else if (errno == 11) {
		// nothing sent
	} else {
		addLogMessage("got error on recv:", strerror(errno));
	}
}



void menuHandlerCallback(void * inMenuRef, void * inItemRef) {
	if (inItemRef == (void*) CMD_PAUSEPI) {
		paused = ! paused;
	} else if (inItemRef == (void*) CMD_PAUSEXP) {
		XPLMCommandKeyStroke(xplm_key_pause);
	} else if (inItemRef == (void*) CMD_READINI) {
		readIniFile();
	} else {
		addLogMessage("unknown menu item", "");
	}
}

float	flightLoopCallback(
		float inElapsedSinceLastCall,
		float inElapsedTimeSinceLastFlightLoop,
		int   inCounter,
		void *inRefcon) {

	if (! callBackActive) {
		callBackActive = true;
		addLogMessage("ready for business", "");
	} else {
		listenForArduinos(readSock);
		sendAnyChangesToBox();
	}
	return REFRESH_RATE_SECS; // seconds before next call
}

void addLogMessage(std::string msg, const char* extra) {
	addLogMessage(msg.c_str(), extra);
}

void addLogMessage(std::string msg, const int extra) {
	addLogMessage(msg.c_str(), extra);
}

void addLogMessage(const char* msg, const char* extra) {
	char buf[512];
	sprintf(buf, "[xpduino] '%s%s'\n", msg, extra);
	XPLMDebugString(buf);
}

void addLogMessageInt(const char* msg, const int extra) {
	char buf[512];
	sprintf(buf, "[xpduino] '%s%i'\n", msg, extra);
	XPLMDebugString(buf);
}

std::string getDataRefValue(XPLMDataRef dataRef) {
	XPLMDataTypeID type = XPLMGetDataRefTypes(dataRef);
	std::stringstream ss("");
	int count;
	int ia[32];
	float fa[32];
	switch (type) {
		case 0:
			// unknown type
			return "";
			break;
		case 1:
			// 4 byte int
			return std::to_string(XPLMGetDatai(dataRef));
			break;
		case 2:
			// 4 byte float
			return std::to_string(XPLMGetDataf(dataRef));
			break;
		case 4:
			// 4 byte double
			return std::to_string(XPLMGetDatad(dataRef));
			break;
		case 8:
			// 4 byte float array
			count = XPLMGetDatavf(dataRef, fa, 0, 32);
			for (int i = 0; i < count; i++) {
				ss << fa[i] << ",";
			}
			return ss.str();
			break;
		case 16:
			// 4 byte int array
			count = XPLMGetDatavi(dataRef, ia, 0, 32);
			for (int i = 0; i < count; i++) {
				ss << ia[i] << ",";
			}
			return ss.str();
			break;
		case 32:
			// block of data
			return "";
			break;
	}
	return "";

}

void iniLineParse(std::string line) {
	/*
	 *	C:KNOB0:sim/autopilot/heading_down:sim/autopilot/heading_up
	 * 	D:LED1:sim/flightmodel/2/gear/deploy_ratio:0:EQ 0?1,0
	*/

	// remove comments
	std::string cleanLine = line.substr( 0, line.find_first_of('#'));
	cleanLine = std::regex_replace(cleanLine, std::regex("^ +| +$|( ) +"), "$1");
	// parse line
	if (cleanLine.length() > 0) {
		int pos;
		if ((pos = cleanLine.find(":")) != std::string::npos) {
			std::string type = cleanLine.substr(0, pos);
    			cleanLine.erase(0, pos + 1);
    			if ((type == "C") || (type == "c")) {
				parseControlEntry(cleanLine);
    			} else if ((type == "D") || (type == "d")) {
				parseDisplayEntry(cleanLine);
    			} else if ((type == "I") || (type == "i")) {
				parseInitialEntry(cleanLine);
    			} else if ((type == "A") || (type == "a")) {
				parseAddressEntry(cleanLine);
    			} else if ((type == "DR") || (type == "dr")) {
				parseDataRefEntry(cleanLine);
    			} else {
				addLogMessage("ignoring line, expected 'C' or 'D': ", type.c_str());
			}
		} else {
			addLogMessage("ignoring line, expected ':', could not parse: ", line.c_str());
		}
	}
}

void parseAddressEntry(std::string line) {
	int pos;
	if ((pos = line.find(":")) != std::string::npos) {
		std::string boxName = line.substr(0, pos).c_str();
		line.erase(0, pos + 1);
		std::string ipAddress;
		if ((pos = line.find(":")) != std::string::npos) {
			std::string ipAddress = line.substr(0, pos).c_str();
			std::stringstream msg("");
			msg << "adding IP address " << ipAddress << " for box named: " << boxName;
			addLogMessage(msg.str().c_str(), "");
			int sock = -1;
			ardIPs.insert({boxName, ipAddress});
			return;
		}
	}
	addLogMessage("ignoring line, expected box ip address, could not parse: ", line.c_str());

}

void parseControlEntry(std::string line) {
	int pos;
	if ((pos = line.find(":")) != std::string::npos) {
		std::string device = line.substr(0, pos).c_str();
		line.erase(0, pos + 1);
		std::string onCmd;
		if ((pos = line.find(":")) != std::string::npos) {
			std::string onCmd = line.substr(0, pos).c_str();
			line.erase(0, pos + 1);
			if ((pos = line.find(":")) != std::string::npos) {
				std::string offCmd = line.substr(0, pos).c_str();
				XPLMCommandRef on = XPLMFindCommand(onCmd.c_str());
				XPLMCommandRef off = XPLMFindCommand(offCmd.c_str());
				controlConfigCommandOn.insert({device, on});
				controlConfigCommandOff.insert({device, off});
			} else {
				addLogMessage("ignoring line, expected Off Command: ", line.c_str());
			}
		} else {
			addLogMessage("ignoring line, expected On Command: ", line.c_str());
		}
		std::string offCmd;
	} else {
		addLogMessage("ignoring line, expected device, could not parse: ", line.c_str());
	}
}

void parseDataRefEntry(std::string line) {
	int pos;
	addLogMessage("parsing data ref entry: ", line.c_str());
	if ((pos = line.find(":")) != std::string::npos) {
		std::string device = line.substr(0, pos).c_str();
		line.erase(0, pos + 1);
		if ((pos = line.find(":")) != std::string::npos) {
			std::string dataRefSt = line.substr(0, pos).c_str();
			line.erase(0, pos + 1);
			if ((pos = line.find(":")) != std::string::npos) {
				std::string index = line.substr(0, pos).c_str();
				line.erase(0, pos + 1);
				if ((pos = line.find(":")) != std::string::npos) {
					std::string logic = line.substr(0, pos).c_str();
					line.erase(0, pos + 1);
					std::string boxName = line;
					addLogMessage(boxName.c_str(), device.c_str());
					XPLMDataRef dataRef = XPLMFindDataRef(dataRefSt.c_str());
					writeToDataRefs.insert({device, dataRef});
				} else {
					addLogMessage("ignoring line, expected logic field, could not parse: ", line.c_str());
				}
			} else {
				addLogMessage("ignoring line, expected index field, could not parse: ", line.c_str());
			}
		} else {
			addLogMessage("ignoring line, expected dataref field, could not parse: ", line.c_str());
		}
	} else {
		addLogMessage("ignoring line, expected device, could not parse: ", line.c_str());
	}
}

void parseDisplayEntry(std::string line) {
	int pos;
	if ((pos = line.find(":")) != std::string::npos) {
		std::string device = line.substr(0, pos).c_str();
		line.erase(0, pos + 1);
		if ((pos = line.find(":")) != std::string::npos) {
			std::string dataRefSt = line.substr(0, pos).c_str();
			line.erase(0, pos + 1);
			if ((pos = line.find(":")) != std::string::npos) {
				std::string index = line.substr(0, pos).c_str();
				line.erase(0, pos + 1);
				if ((pos = line.find(":")) != std::string::npos) {
					std::string logic = line.substr(0, pos).c_str();
					line.erase(0, pos + 1);
					std::string boxName = line;
					addLogMessage(boxName.c_str(), device.c_str());

					XPLMDataRef dataRef = XPLMFindDataRef(dataRefSt.c_str());
					dataRefs.insert({device, dataRef});
					boxDevices.insert({device, boxName});
					if (index == "") {
						index = "-1";
					}
					dataRefIndex.insert({device, std::stoi(index)});
					dataRefLogic.insert({device, logic});
					std::string value = getDataRefValue(dataRef);
					lastValue.insert({device, value});
				} else {
					addLogMessage("ignoring line, expected logic field, could not parse: ", line.c_str());
				}
			} else {
				addLogMessage("ignoring line, expected index field, could not parse: ", line.c_str());
			}
		} else {
			addLogMessage("ignoring line, expected dataref field, could not parse: ", line.c_str());
		}
	} else {
		addLogMessage("ignoring line, expected device, could not parse: ", line.c_str());
	}
}

void parseInitialEntry(std::string line) {
	int pos;
	if ((pos = line.find(":")) != std::string::npos) {
		std::string device = line.substr(0, pos).c_str();
		line.erase(0, pos + 1);
		if ((pos = line.find(":")) != std::string::npos) {
			std::string value = line.substr(0, pos).c_str();
			line.erase(0, pos + 1);
			std::string boxName = line;
			std::stringstream msg("");
		        msg << "setting initial value of " << device << " on box "  << boxName << " to: " << value;
			addLogMessage(msg.str().c_str(), "");
			sendArduinoBox(boxName, (device + ":" + value).c_str());
			msg.str("");
		        msg << device << " initial value " << value << " sent to " << boxName;
			addLogMessage(msg.str().c_str(), "");
			return;
		}
	}
	addLogMessage("ignoring line, expected inital setting, could not parse: ", line.c_str());
}

void eraseIniData() {
	controlConfigCommandOn.clear();
	controlConfigCommandOff.clear();
	dataRefIndex.clear();
	dataRefLogic.clear();
	dataRefs.clear();
	writeToDataRefs.clear();
}

bool readIniFile() {
	eraseIniData();
	addLogMessage("parsing ini file:", INI_DEFAULT_PATH);
	std::ifstream iniFile(INI_DEFAULT_PATH);
	if (iniFile.is_open()) {
		std::string line;
		while(getline(iniFile, line)) {
			iniLineParse(line);
		}
		iniFile.close();
		return true;
	} else {
		addLogMessage("could not open ini file:", INI_DEFAULT_PATH);
		return false;
	}
}

std::string getLogicOperator(std::string logicSt) {
	int pos;
	if ((pos = logicSt.find("?")) != std::string::npos) {
		return logicSt.substr(0, pos);
	}
	return logicSt;
}

std::string getTrueValue(std::string logicSt) {
	int pos;
	if ((pos = logicSt.find("?")) != std::string::npos) {
		logicSt.erase(0, pos + 1);
		if ((pos = logicSt.find(",")) != std::string::npos) {
			return logicSt.substr(0, pos);
		}
	}
	return "??";
}

std::string getFalseValue(std::string logicSt) {
	int pos;
	if ((pos = logicSt.find(",")) != std::string::npos) {
		logicSt.erase(0, pos + 1);
		return logicSt;
	}
	return "??";
}

std::string getOpCode(std::string opSt) {
	int pos;
	if ((pos = opSt.find(" ")) != std::string::npos) {
		return opSt.substr(0, pos);
	}
	return opSt;
}

std::string getOpValue(std::string opSt) {
	int pos;
	if ((pos = opSt.find(" ")) != std::string::npos) {
		opSt.erase(0, pos + 1);
	}
	return opSt;
}

std::string getValueByIndex(std::string value, int index) {
	if (index == -1) {
		return value;
	}
	int pos;
	for (int i = 0; i < index; i++) {
		if ((pos = value.find(",")) != std::string::npos) {
			value.erase(0, pos + 1);
		}
	}
	if ((pos = value.find(",")) != std::string::npos) {
		return value.substr(0, pos);
	}
	return value;
}

void calculateResponse(std::string boxName, std::string device, std::string oldValue, std::string currentValue, std::string logic) {
	std::string op = getLogicOperator(logic);
	std::string opCode = getOpCode(op);
	std::string trueResponse = getTrueValue(logic);
	std::string falseResponse = getFalseValue(logic);
	if (op == "EXACT") {
		opCode = op;
	}
	if (oldValue != currentValue) {
		std::stringstream lmsg("");
		lmsg << device << " values changed: " << oldValue << " != " << currentValue;
		addLogMessage(lmsg.str().c_str(), "");

		if (opCode == "CHG") {
			sendArduinoBox(boxName, (device + ":" + trueResponse).c_str());
		} else {
			std::map<std::string, int>::iterator iit = dataRefIndex.find(device);
			if (iit != dataRefIndex.end()) {
				int index = iit->second;
				currentValue = getValueByIndex(currentValue, index);

				std::string opValue = getOpValue(op);
				bool match = false;
				if (opCode == "EQ") {
					match = std::stod(currentValue) == std::stod(opValue);
		lmsg.str("");
		lmsg << "cf : " << currentValue << " == " << opValue << "?   " << match;
		addLogMessage(lmsg.str().c_str(), "");
				} else if (opCode == "EXACT") {
					sendArduinoBox(boxName, (device + ":" + currentValue).c_str());
					return;
				} else if (opCode == "GT") {
					match = currentValue > opValue;
				} else if (opCode == "LT") {
					match = currentValue < opValue;
				} else if (opCode == "GTE") {
					match = currentValue >= opValue;
				} else if (opCode == "LTE") {
					match = currentValue <= opValue;
				} else if (opCode == "EXACTIFDIFFGT") {
					//addLogMessage((currentValue + " " + opCode + " " + opValue).c_str() , (", was " + oldValue + " -> " + currentValue).c_str());
					if (abs(std::stof(currentValue) - std::stof(oldValue)) > std::stof(opValue)) {
						//addLogMessage("YES current = ", (oldValue + " -> " + currentValue).c_str());
						sendArduinoBox(boxName, (device + ":" + currentValue).c_str());
					} else if (sendAll) {
						sendArduinoBox(boxName, (device + ":" + currentValue).c_str());
					}
					//addLogMessage("NO current = ", (oldValue + " -> " + currentValue).c_str());
					return;
				} else {
					addLogMessage("Unknown operator: ", opCode.c_str());
					return;
				}
				if (match) {
					sendArduinoBox(boxName, (device + ":" + trueResponse).c_str());
				} else {
					sendArduinoBox(boxName, (device + ":" + falseResponse).c_str());
				}
			}
		}
	} else {
		/*if (opCode == "CHG") {
			// avoid flooding data - only do every second or so
			addLogMessage("counter: ", counter++);
			if (counter % 100 == 0) {
				sendArduinoBox(boxName, (device + ":" + falseResponse).c_str());
			}
		}*/
	}
}

std::string getBoxDevice(std::string device) {
	std::map<std::string, std::string>::iterator it = boxDevices.find(device);
	if (it != boxDevices.end()) {
		return it->second;
	}
	return "";
}

void sendAnyChangesToBox() {
	std::map<std::string, XPLMDataRef>::iterator v = dataRefs.begin();
	while (v != dataRefs.end()) {
		std::string device = v->first;
		std::string boxName = getBoxDevice(device);
		if (boxName != "") {
			XPLMDataRef dataRef = v->second;
			std::string value = getDataRefValue(dataRef);
			std::map<std::string, std::string>::iterator last = lastValue.find(device);
			// do we have a 'last' value?
			if (last != lastValue.end()) {
				std::map<std::string, std::string>::iterator lit = dataRefLogic.find(device);
				if (lit != dataRefLogic.end()) {
					std::string logic = lit->second;
					calculateResponse(boxName, device, last->second, value, logic);
					// update 'last' value
					last->second = value;
				}
			} else {
			}
		}
		v++;
	}
	sendAll = false;
}
