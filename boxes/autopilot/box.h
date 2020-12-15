#ifndef __BOX_H__
#define __BOX_H__

/*
 * Define box network and message stuff here.
 * You need to define:
 * *
 *  IP_ADDRESS: 		IP address of the arduino, comma separated -
 * 				nice to be able to set this dynamically but
 * 				this functionality not yet implemented. Must
 * 				be unique on the network.
 *
 *  NETWORK_BROADCAST_ADDRESS:	IP4 broadcast address for the network the
 *				simulator is running on, comma separated - used
 *				by the device to initially hunt down the XPlane
 *				box. (eg: 192,168,0,255)
 *
 *  MAC_ADDRESS:		a MAC address, comma separated - should be
 *				unique to each Arduino used in the simulator.
 *
 *  BOXID:			a name that identifies the device.
 *
 *  BOX_DEFINITIONS:		a list of messages the box can send to XPlane
 *				or that the box can respond to if sent from
 *				XPlane.
 *
 *  NB do not use colon character ':' in BOXID or BOX_DEFINITIONS
 *
 * Some example random MAC Numbers
 * 0x40,0x7B,0xD8,0x0B,0x85,0x16
 * 0x9C,0xA1,0xCB,0xE2,0x6B,0x47
 * 0xC5,0xDF,0xD5,0x4B,0x82,0x58
 * 0xFF,0xCE,0xF5,0xA2,0xA2,0x28
 *
 *
 *
 * */

#define BOXID "Motion Capture-AP-00000"
#define BOX_DEFINITIONS "LED0, LED1, AP, FD, HDG, NAV, APP, VS, HDG, CRS"
#define MAC_ADDRESS 0xC0, 0x90, 0x33, 0x53, 0xB7, 0xAA
#define IP_ADDRESS 192, 168, 0, 192
#define GATEWAY_ADDRESS 192, 168, 0, 1
#define NETMASK 255, 255, 255, 0
#define NETWORK_BROADCAST_ADDRESS 192, 168, 0, 255

/*
 *
 * Declare standard methods.  You must implement each of the following for your
 * arduino device:
 * *
 *   void boxSetup(void)		where you set up arduino pins,
 *   					initialise displays, hardware etc
 *
 *   void setStartState(void);		use if you want to set anything up once
 *					communication between the box and
 *					XPlane is made.  (eg you may display
 *					some diagnostic, welcome screens prior
 *					to contact with XPlane.  This method
 *					is called  once XPlane has been found
 *					and communication established - you may
 *					want to first remove diagnostic messages,
 *					flashing lights etc and get ready for
 *					use).
 *
 *   void noConnectionActions(void);	any actions you want to take if the
 *					device hasn't (yet) found XPlane (eg
 *					show diagnostic messages, flash warning
 *					lights etc)
 *
 *   void setControl(char* device, char* value); set something on the arduino
 *					in response to a message from XPlane,
 *					(eg turn on a LED, display a value, run
 *					a motor etc)
 *
 *   void boxMainLoop(void);		called repeatedly, this is where you
 *					can adjust displays, check switches,
 *					get encoder values etc.
 *
 * */

void boxSetup(void);
void setStartState(void);
void noConnectionActions(void);
void setControl(char* device, char* value);
void boxMainLoop(void);

#endif
