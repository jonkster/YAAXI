# YAAXI

A simple Arduino/X-Plane interface system for adding physical cockpit controls
to X-Plane.

(NB simple means simple configuration and set up if you are comfortable with
Arduino programming in C and interfacing of devices to Arduino.  It doesn't
mean gui configuration screens and/or drag and drop customisation etc).

## Current Status

Very RAW.

### Philosophy

The Arduino device ("Box") has one or more devices connected to it, like
switches, LEDs, rotary encoders, LCD displays etc.

Devices are classified as either:

 - Controls (eg switches etc) that are designed to send data to X Plane.

 - Displays (eg LEDs, LCD displays etc) that are designed to display information
sent from X Plane.

An Arduino box can have a mixture of control and display devices.

X Plane requies a plug in that can interact with one or more "Boxes".

Configuring what X Plane should do with control data sent to it and also what
display data it needs to send to the box, is done with a configuration file (or
files) located with the X Plane Plugin.

The configuration file is simple text and should be reasonably simple to set
up, it will list each device on a box with instructions about:

  - what X Plane Commands correspond to messages from box controls
  - what X-Plane data ref data should be sent to the box for display

## Overview

The system allows you to set up physical hardware (switches, lights, motors,
displays etc) driven by an Arduino, to interact with X-Plane.

It assumes you are comfortable interfacing devices to an Arduino and with
programing an Arduino.

It requires an Arduino with an Ethernet shield (or on board ethernet - eg like
the Freetronics Ether\* Arduino clones).

A simple default example of the Arduino code can be used as the basis of your
customised system.


### It consists of 3 components:

*AvduinoBox* an Arduino program that communicates with the X-Plane Arduino
Broker Plugin.

*ArduinoBroker* is an X-Plane plugin that communicates with an Arduino that is
running the xpDuinoBox program.

*INI File(s)* on the X Plane system that tailor specify how X Plane should
interpret and act on the box devices.


General Box description
=======================

a box has:
- controls (like switches/knobs etc) that can send their current state.
- displays (like lights/gauges/displays etc) that can have their state changed

Each box should have a unique ID
(suggested format "MANUFACTURER_ID-BOX_TYPE-XXXXX" eg "Motion Capture-Switch
Box v0-9aef456")

Each of the controls/displays should have a unique string name (eg TOGGLE0,
LED0 etc)

Prototype Box Operation
=======================

The protoptype box initially used to develop the system consists of a number of
devices connected to an Arduino Mega with an etherney shield.

The prototype example box consists of:

Controls:
---------
- 2 toggle switches,
- 2 momentary on switches,
- 2 rotary encoders that also have momentary on switches and
- a 3 position rotary switch that acts like 2 toggle switches but only 1 can be
  on at a time)

Displays:
--------
- 6 leds (3 red and 3 green)

The box has 2 modes: normal and diagnostic. It should boot up in normal mode.

In normal mode, the box will flash all red leds regularly until it can connect
to an XP plugin.  After that it will send any detected control changes
(ie changes of switch/knob positions) to the plugin and act on device messages
from the plugin by setting leds.

In diagnostic mode, changing switches on the box should generate different led light
displays - this used to test all switches work

to change modes:
----------------

- hold both rotary encoder push buttons down simultaneously to put box in diagnostic mode

- hold both red push buttons down simultaneously to return box to normal mode



Communication Protocol
======================

box IP currently hard coded to be 192.168.0.178

messages from box to XP
-----------------------

Box sends UDP messages to XP plugin using port 8889

To find the Xplane plugin, on box startup it will send a "XP Plugin Fish" string
on the broadcast address using port 8889.

If it gets a response "Avduino Box Fish" on port 8888 it will set the IP
address of the responder as the XPlane address.

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


messages to box from XP:
------------------------

The box reads UDP messages sent to it on port 8888

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


see simulatePlugin.sh for examples of communication protocol


### out of the box

The default Arduino program supplied is set up to respond to a switch on pin 8
and a led on pin 7 that allow you to test the system works (the switch should
set the pitot heat switch in X-Plane and the LED should reflect the position of
the Nav Light switch in X-Plane).

Once you confirm this works you can then:

-  modify the Arduino code (and add whatever appropriate physical devices to
   it) to match the cockpit devices you wish to use.

- modify the ini file to reflect the devices you add to the Arduino box

## How it works

The Arduino is programmed with code you write, based on the supplied default C
code.  The Arduino should be connected via ethernet to the same network as the
computer
running X-Plane.

X-Plane needs to have the ArduinoBroker plugin installed.

### Simple example


## Installation

1. Set up the Arduino initially with a switch connected to data pin 8 and a led
   to pin 7 (this just for testing purposes)
2. Compile and Upload the default C program to the Arduino
3. Connect the Arduino via ethernet to the same network as the X-Plane box
4. Copy the plugin directory to X-Plane (this will include the test INI file.
5. Run X-Plane with standard C172
6. Confirm that the physical switch connected to data pin 8 will set the
   'virtual' Pitot Heat Switch in X-Plane
7. Confirm that the 'virtual' Nav Light Switch changed in X-Plane will light a
   LED connected to Arduino pin 7

*THEN*

8. Set up your hardware (switches, lights, displays, motors etc) to the Arduino
   and modify the Arduino code and INI file to match your needs.

## Configuration and Setting up Your Specific Arduino Code

This assumes you have set up your hardware to the Arduino and know what
X Plane data refs and data commands you want to interact with via the Arduino.



