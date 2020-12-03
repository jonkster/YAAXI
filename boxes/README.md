# This subdirectory holds arduino box source code.

Making a new Arduino box
========================

### 1. make a new directory for the device and copy the following files from "simplest" to the new directory:

```
main.cpp
box.h
Makefile
simulatePlugin.sh
box.sh
```


### 2. in the new directory, edit box.h and specify an appropriate values for the following pre processor defines:

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
XPlane to potentially present a GUI style configuration option rather than
needing the user to edit the initialisation file by hand, so try and make it
useful so it can be used in future.

The IP addresses must be hard coded at the moment, your arduino will need a
unique IP address on your network.

### 3. create a file:
```
box.cpp
```

In box.cpp, you set up any device specific code.  You must define the following
functions that are declared in the box.h file above:

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


### 4. Write some simple tests in the XPlane simulation script.

Using the existing script box.sh as a guide, add any test messages that can
be used to establish the system is working without having to fire up XPlane
repeatedly.

### 5. Test the device

Use the simulatePlugin.sh shell script to test the device.  It mimics the
behaviour of the XPlane plugin and will run what you have set up in the box.sh
file.  NB it requires netcat.


