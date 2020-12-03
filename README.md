﻿# YAAXI

A simple Arduino/X-Plane interface system for adding physical cockpit controls
to X-Plane.

### NB simple means simple configuration and set up if you are comfortable (or
### are willing to get comfortable) with Arduino programming in C and
### interfacing of devices to Arduino.  It doesn't mean gui configuration
### screens and/or drag and drop customisation etc.

## Overview

The system allows you to set up physical hardware (switches, lights, motors,
displays etc) driven by an Arduino (or several Arduinos), to interact with
X-Plane.

It assumes you are comfortable interfacing devices like LEDS, switches displays
etc to an Arduino and with programing an Arduino in C.

It requires an Arduino with an Ethernet shield (or on board ethernet - eg like
the Freetronics Ether\* Arduino clones).

A simple default example of the Arduino code is supplied that can be used to
test the setup and communications with X-Plane as the basis of your customised
systems.


## Current Status

Very RAW.  Developed on a Linux system.  The plugin only tested on linux, it
should be able to be ported to Windows systems (and Mac) but I do not have the
environment to try this out.

The Arduino stuff does not use the Arduino IDE, instead you edit the source
code using whatever editor you prefer and compile and load the code using
Makefiles run from the command line. 
 
### Philosophy

The Arduino (or Arduinos - you can have multiple Arduinos) will have
sensors or display devices connected to it, like switches, LEDs, rotary
encoders, LCD display, OLED displays, stepper motors etc.

Devices connected to the Arduino are classified as either:

 - INPUTS (eg switches etc) that are designed to send data to X Plane.

 - OUTPUTS (eg LEDs, LCD displays, stepper motors etc) that are designed to
   display or react to information sent from X Plane.

An Arduino box can have a mixture of INPUT and OUTPUT devices connected to it.

X Plane requies a plug-in that can interact with one (or more) Arduino "Boxes".

Configuring what X Plane should do with control data sent to it and also what
data it needs to send to the Arduino, is done with a configuration file (or
files) located with the X Plane Plugin.

The configuration file is simple text and should be reasonably simple to set
up, it will list each device on a box with instructions about:

  - what X Plane Commands correspond to messages from box INPUTS (eg "pitot heat switch was turned on")

  - what X-Plane data ref data should be sent to the box as OUTPUTS (eg "the undercarriage warning light should be on")

### The YAAXI system consists of 3 components:

One (or more) *ArduinoBoxes* - an arduino connected to sensors (eg switches)
and displays or actuators (eg leds, motors) running a program that communicates
with the X-Plane Arduino Broker Plugin.

An *ArduinoBroker* - an X-Plane plugin that communicates with *ArduinoBoxes*.

*INI File(s)* - placed on the X Plane system, these are used by the broker to
specify how X Plane should interpret and act on messages from the box devices
and what data X-Plane should send to the device(s).


General Box description
=======================

a box has:

- INPUTS (like switches/knobs etc) that can send their current state.
- OUTPUTS (like lights/gauges/displays etc) that can have their state changed

Each box should have a unique ID string to identify it.  (eg "Switch Box v0")

Each of the devices connected to the arduino should have a unique string name
(eg TOGGLE0, LED0 etc) that is used in messages passed between the arduino and
the X-Plane broker.  What they actually correspond to (eg "LED0 is the Landing
Gear Safe Light" or "SWITCH1 is the Pitot Heat Switch") is specified in the INI
file.

Writing Code for an Arduino in the YAAXI system
===============================================

The process typically will some standard common files (main.cpp, Makefile
helper.mk, simulatePlugin.sh etc).  The idea is these should require no (or very minor changes between
projects).

You write your customised device code in box.cpp in which you will implement
several functions declared in box.h (eg a method that sets up your
Arduino pins, a method that the system will repeatedly call where you should
read any sensor values, a method that sets your output devices if it receives a
message from X-Plane etc).

The functions you should implement (and what they do) are listed in the:
```
boxes/README.md
```
file.  Use the example boxes as a guide.


You will probably also need to set some pre-processor definitions in box.h to
match your network.


Simplest System
---------------

A simple test example is located in
```
boxes/simplest
```

Use this code to set up your system and check it all works with X-Plane before
developing more complicated setups.  You can then copy the simplest code to a
new directory and build on it to make what you need.

The simplest Arduino setup should have a switch on pin 9 and a LED on pin 8
(with a resistor on the cathode connected to ground).  You can modify the pin
numbers in:
```
boxes/simplest/box.cpp
```

This allows example you to test the system works and is used as the basis of
developing other systems.  Having this simple device available for future use
can help in debugging communications between X-Plane and arduinos when you work
on more complicated setups.

A more involved system is a dual COM and NAV radio system - see
```
boxes/comnav2box
```


YAAXI Communication Protocol
============================

messages from box to X-Plane
----------------------------

The Arduino Box sends UDP messages to the X-Plane plugin using port 8889

To find the Xplane plugin, on box startup it will send a "XP Plugin Fish" string
on the broadcast address using port 8889.

If it gets a response "Avduino Box Fish" on port 8888 it will set the IP
address of the responder as the X-Plane address.
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

messages to box from X-Plane:
------------------------

The box reads UDP messages sent to it on port 8888

```
format: "XP Avduino Fish" 
	
	treat IP address this message comes from as the IP address of the X-Plane
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

"Simplest" Box Operation
=======================

The simplest box initially used to test the system consists of a LED on pin 8 and a
switch on pin 9 of an arduino with an ethernet shield.

The code for this is in the
```
boxes/simplest
```
directory.

The "simplest" box IP currently hard coded to be 192.168.0.189 - change this to
an appropriate address for your network.

Initial test
------------

Set up the Arduino as described above.

Connect the Arduino to your development box via a USB cable, and an ethernet
cable to your network and in the simplest directory do:
```
make
make upload
```
to program the Arduino. (NB the simplest Makefile assumes an Arduino Uno, edit
the Makefile if using a different device.)


Now run the 'simulatePlugin.sh' shell script.  It should flash the LED several
times and if you pull pin 9 to ground or let it float by using the switch, it
should display:
```
 SWITCH0:1
	or
 SWITCH0:0
```
as the switch turns on and off.

Test with X-Plane
-----------------

If the initial simulated plugin test works test against X-Plane.

Make sure:

1. the plugin is installed in X-Plane, 
2. the ini file is set up as described,
3. the Arduino and X-Plane are connected on the same network
4. X-Plane is running the standard Cessna 172

Run X-Plane and look at the C172 instrument panel (the pitot heat switch and
nav light switch).

The switch on the Arduino should now be able to control the position of the
pitot heat switch displayed in X-Plane and the LED should light according to
the position you set the Nav Light switch in X-Plane using the mouse.


The default Arduino program supplied is set up to respond to a switch on pin 9
and a led on pin 8 that allow you to test the system works (the switch should
set the pitot heat switch in X-Plane and the LED should reflect the position of
the Nav Light switch in X-Plane).

Once you confirm this works you can then:

-  modify the Arduino code (and add whatever appropriate physical devices to
   it) to match the cockpit devices you wish to use.

- modify the ini file to reflect the devices you add to the Arduino box

### It may be worth keeping the "simplest box" device around as it can verify
### X-Plane is talking properly to Arduinos via the plugin.

