General Box description
=======================
a box has:
- controls (like switches/knobs etc) that can send their current state.
- displays (like lights/gauges/displays etc) that can have their state changed

Each box should have a unique ID
(suggested format "MANUFACTURER_ID-BOX_TYPE-XXXXX" eg "Motion Capture-Switch
Box v0-9aef456")

Each of the controls/displays you want to control from the Arduino should be
given a unique string name (eg TOGGLE0, LED0, SWITCH2 etc) that you will use to
communicate between XPlane and the arduino box.

How the System works
====================

The arduino box starts calling on the network broadcast address trying to find
the XPlane plugin that is listening for arduino devices.

When the plugin finds a new arduino device, it sends a response to the arduino
device and now both XPlane and the ardiuno know each other's address.

The XPlane plugin sends messages to the arduino device something like:
```
LED2:1
LCDLINE1:Hello!
```

The box interprets this and can act on it.

The box can send the XPlane plugin messages like:

```
SWITCH0:1
ENCODER1:234
```


Making a new Arduino box
========================

1. copy the generic files to a new directory:
```
	main.cpp
	box.h
	Makefile
	simulatePlugin.sh
	box.sh
```


2. in box.h specify an appropriate values for the following pre processor defines:

```
	BOXID
	BOX_DEFINITIONS
	IP_ADDRESS
	MAC_ADDRESS
	NETWORK_BROADCAST_ADDRESS
```

	eg

```
	#define BOXID "Motion Capture-CN001A-00000"
	#define BOX_DEFINITIONS "C1U-CI,C1S-CI,C2U-CI,C2S-CI,N1U-CI,N1S-CI,N2U-CI,N2S-CI"
	#define IP_ADDRESS 192, 168, 0, 190
	#define MAC_ADDRESS 0xC0, 0x90, 0x33, 0x53, 0xB8, 0xDF
	#define NETWORK_BROADCAST_ADDRESS 192, 168, 0, 255
```

	NB the BOX_DEFINITIONS setting is not currently used but is meant to allow
	XPlane to potentially present a GUI style configuration option rather
	than needing the user to edit the initialisation file by hand, so try
	and make it useful so it can be used in future.

	The IP addresses must be hard coded at the moment, your arduino will
	need a unique IP address on your network.

3. create a file:
```
	box.cpp
```

	In box.cpp, you set up any device specific code.  You must define the
	following functions that are declared in the box.h file above:

```
	void boxSetup(void);
	void clearChanges(void);
	void setStartState(void);
	void noConnectionActions(void);
	bool sendAnyChanges(void);
	void setControl(char* device, char* value);
	void boxMainLoop(void);
```

### What the required methods above should implement:
*void boxSetup(void)*
		where you set up arduino pins, initialise displays, hardware etc
 
*void clearChanges(void)*
		called after "sendAnyChanges" - use it if you need to remove
		any flags that you use to say changes need to be sent to XPlane

*void setStartState(void)*
		use if you want to set anything up once communication between
		the box and XPlane is made.  (eg you may display some
		diagnostic, welcome screens prior to contact with XPlane.  This
		method is called  once XPlane has been found and communication
		established - you may want to first remove diagnostic messages,
		flashing lights etc and get ready for use).

*void noConnectionActions(void)*
	any actions you want to take if the device hasn't (yet) found XPlane
	(eg show diagnostic messages, flash warning lights etc)

*bool sendAnyChanges(void)*
	where you check if anything has changed on the device (eg a switch was
	pushed) and if so, send an appropriate message(s) to XPlane,

*void setControl(char* device, char* value)*
	 set something on the arduino in response to a message from XPlane, (eg
	turn on a LED, display a value, run a motor etc)

*void boxMainLoop(void)*
	called repeatedly, this is where you can adjust displays, check
	switches, get encoder values etc.


4. Using the existing script box.sh as a guide, add any test messages that can
be used to establish the system is working without having to fire up XPlane
repeatedly.



Communication Protocol
======================

box IP currently hard coded (eg 192.168.0.190).  This may be refactored at some
point to allow the arduino to be assigned an address but currently you will
need to hard code a unique address to each arduino device you use.

messages from box to XP
-----------------------

Box sends UDP messages to XP plugin using port 8889

To find the Xplane plugin, on box startup it will send a "XP Plugin Fish" string
on the broadcast address using port 8889.

If it gets a response "Avduino Box Fish" on port 8888 it will set the IP
address of the responder as the XPlane address.

```
	format: "XP Plugin Fish"
		try and get a response from the plugin
		(expecting an "Avduino Box Fish" message in return)

	format "BOXID:[NAME-TYPE, NAME-TYPE, ...]
		description of box where:
		BOXID  = unique box identifier (eg "MC:SB00:AEDFE0A5B3")
		NAME = a string identifying a control or device (eg SWITCH0, LED1 etc)
		TYPE   = a description of the control or device
		eg
			CB : control that sends 0:1 (binary like a switch)
			CI : control that sends 0,1,2... (integer value eg heading)
			CF : control that sends XXX.YYY (float like a radio freq 125.100)	
			DB : device that accepts binary (eg LED that can be turned off|on)
			DI : device that accepts integer value
			DF : device that accepts a float value


	format: "NAME:Y"
		where NAME = control name (eg SWITCH2)
		      Y = value 
		eg "TWIST1:270" or "SWITCH2:1"

```

messages to box from XP:
------------------------

The box reads UDP messages sent to it on port 8888

```
format: "XP Avduino Fish" 
	
	treat IP address this message comes from as the IP address of the XP
	plugin (and respond with a description of the box)

format: "NAME:Y"
	set device value in box
		where NAME = device name (eg LED1)
		      Y = value

	eg:
		LED2:1	 - means turn LED2 on

all other messages:
	return unique box identifier

```

see simulatePlugin.sh for examples of communication protocol



