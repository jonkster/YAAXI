# YAAXI - Yet Another Arduino X-Plane Interface :)

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

A more complicated example is also provided (a dual COM and dual NAV radio
setup).

![Front Panel](boxes/comnav2box/comnav2box.jpg?raw=true "COM-NAV box front")

There are other Arduino/X-Plane interfaces available - this one is not to
replace them as they may be more refined and better suited to most users.  This
one is meant to be quick and dirty and flexible and allows (hopefully) a lot of
flexibility.

## Current Status

The code is rapidly changing, beware if you start using this at the moment,
particularly configuration syntax!  Hopefully the structure will bed down soon.

NB it is all very RAW.

YAAXI is being developed on a Linux system.  The plugin only tested on Linux,
it should be able to be ported to Windows systems (and Mac) but I do not have
the environment to try this out.

The Arduino stuff does not use the Arduino IDE, instead you edit the source
code using whatever editor you prefer and compile and load the code using
Makefiles run from the command line. (It should be able to be set up to use the
IDE but I find it faster and more convenient to run it all from the command
line)

## In a Nutshell

YAAXI is based on a simple message protocol.  Communication between the
Arduino(s) and X-Plane is via UDP over ethernet.  **The Arduino devices do not
know anything about X-Plane**, a configuration file in X-Plane worries about
that.

As an example, the Arduino might know it has a Switch called SWITCH0 and a LED
called LED0 (you write the code to set this up).  The Arduino doesn't know what
they correspond to in X-Plane.  It doesn't know or care about X-Plane data-refs
or commands.  It can only send the X-Plane plugin info like SWITCH0 is now "on"
and it can respond to messages from X-Plane that ask it to turn LED0 "off".

The configuration file in X-Plane might be set up to map LED0 to the gear
unsafe light and SWITCH0 to the gear selector.

The reason for this is **changes to configuration/behaviour should not involve
re-programming the Arduino**, instead changes can be made with a simple text
change in a configuration file located on the X-Plane box.  Potentially this
configuration could even be done via a GUI interface in a similar style to the
way Joysticks/Throttle quadrants etc are configured in X-Plane.


### The YAAXI system consists of 3 components:

One (or more) *ArduinoBoxes* - an arduino connected to sensors (eg switches)
and displays or actuators (eg leds, motors) running a program that communicates
with the X-Plane Arduino Broker Plugin.

An *ArduinoBroker* - an X-Plane plugin that communicates with *ArduinoBoxes*.

*INI File(s)* - placed on the X Plane system, these are used by the broker to
specify how X Plane should interpret and act on messages from the box devices
and what data X-Plane should send to the device(s).

 
### Philosophy

The Arduino box (or boxes - you can have multiple Arduinos) will have
sensors or display devices or actuators connected to it, like switches, LEDs,
rotary encoders, LCD display, OLED displays, stepper motors etc.

Devices connected to the Arduino are classified as either:

 - CONTROLS - inputs (eg switches etc) that are designed to send data to X Plane.

 - DISPLAYS - outputs (eg LEDs, LCD displays, stepper motors etc) that are designed to
   display or react to information sent from X Plane.

(NB actuators like relays, stepper motors etc are classified in the YAAXI
system as displays)

A single Arduino box can have a mixture of input and output (CONTROL/DISPLAY)
devices connected to it.

X Plane requires the YAAXI plug-in that interacts with one (or more) YAAXI
Arduino "Boxes".

Configuring what X Plane should do with control data sent to it and also what
data it needs to send to the Arduino, is done with a configuration file (or
files) located with the X Plane Plugin.

The configuration file is simple text and should be reasonably simple to set
up, it will list each device on a box with instructions about:

  - what X Plane Commands correspond to messages from box CONTROLS (eg "pitot heat switch was turned on")

  - what X-Plane data ref data should be sent to the box as DISPLAYS (eg "the undercarriage warning light should be on")

The Plugin Configuration File on X-Plane
=========================================

An example of how configuration values might be set using a simple for a
COM/NAV radio is shown below.

Assume the Arduino box has been programmed to respond to commands that refer to the following displays:

```
CU (the COM "In Use" frequency display)
CS (the COM "Standby" frequency display)
NU (the NAV "In Use" frequency display)
NS (the NAV "Standby" frequency display)
```

So if the Arduino receives a message from X-Plane like:
```
CS:12010
```
it will change the COM Standby frequency display to show the frequency 120.10

The box also has some controls - some rotary encoders and push button switches.
The Arduino has been programmed to send messages like:
```
FLIP_COM (sent when the COM "swap" frequency button is pressed)
FLIP_NAV (sent when the NAV "swap" frequency button is pressed)
KHZ_KNOB_COM (sent when the COM frequency inner knob is rotated - if clockwise send an additional "1" or "-1" if counter-clockwise) 
KHZ_KNOB_COM (sent when the COM frequency outer knob is rotated - if clockwise send an additional "1" or "-1" if counter-clockwise) 
KHZ_KNOB_NAV (sent when the NAV frequency inner knob is rotated - if clockwise send an additional "1" or "-1" if counter-clockwise) 
KHZ_KNOB_NAV (sent when the NAV frequency outer knob is rotated - if clockwise send an additional "1" or "-1" if counter-clockwise) 
```

The X-Plane plugin configuration file might look like this:
```
# give BOX an index name and enter its IP address
A:BOX4:192.168.0.190:

# Initial values to send to Arduino when plugin finds the box
I:CU:12455:BOX4
I:CS:12465:BOX4
I:NU:10905:BOX4
I:NS:10915:BOX4

# What X-Plane should do with messages from Arduino
C:FLIP_COM:sim/radios/com1_standy_flip::
C:FLIP_NAV:sim/radios/nav1_standy_flip::
C:MHZ_KNOB_COM:sim/radios/stby_com1_coarse_down:sim/radios/stby_com1_coarse_up:
C:KHZ_KNOB_COM:sim/radios/stby_com1_fine_down:sim/radios/stby_com1_fine_up:
C:MHZ_KNOB_NAV:sim/radios/stby_nav1_coarse_down:sim/radios/stby_nav1_coarse_up:
C:KHZ_KNOB_NAV:sim/radios/stby_nav1_fine_down:sim/radios/stby_nav1_fine_up:

# What data X-Plane should send to the Arduino (sent whenever these change)
D:CU:sim/cockpit/radios/com1_freq_hz::EXACT?,:BOX4
D:CS:sim/cockpit/radios/com1_stdby_freq_hz::EXACT?,:BOX4
D:NU:sim/cockpit/radios/nav1_freq_hz::EXACT?,:BOX4
D:NS:sim/cockpit/radios/nav1_stdby_freq_hz::EXACT?,:BOX4
```


The format of configuration file entries follow the structure below.

```
 Configuration 
 
 TYPE:
 	C = Control (eg switch on box etc)
 	D = Display (eg LED on BOX or instrument or stepper actuator etc)	
	I = send this value to Arduino box on startup
	DR = set a data ref with value
 
 Control INI entries
 --------------------
 
 How X-Plane should react to a message sent from the box
 
 Format:
 	C:DEVICE:CMD IF TRUE:CMD IF FALSE:RESERVED
 where
	RESERVED - leave blank
 eg:
	the Arduino might send a message like: "TOGGLE0:1" or "TOGGLE0:0" when
	a toggle switch is clicked and you might want this to trigger the
	pump on/off command in X-Plane

 	C:TOGGLE0:sim/fuel/fuel_pump_1_on:sim/fuel/fuel_pump_1_off:
 
 
 Display INI entries
 -------------------
 
 Send data to the Arduino box if a dataref value changes
 	
 Format:
 	D:DEVICE:DataRef:INDEX:LOGIC:RESERVED:BOX_ID
 
 	LOGIC field can be blank if no logic needed
	RESERVED - leave blank
 where
	INDEX: index of wanted value in array if dataref is a vector.  Leave empty otherwise

 	LOGIC:
 		TEST?value if true,value if false
 		eg
		 	EQ 0?1,0
		 	if dataref value == 0 send a 1 else send a 0
 
		 	GT 10?1,0
		 	if dataref value > 0 send a 1 else send a 0
 
 		comparison operators:
 		EQ
 		GT
 		LT
 		GTE
 		LTE
 		CHG
 		EXACT

	BOX_ID: A name (eg BOX2) that specifies this action only applies to
		the DEVICE string coming from a particular BOX.  This means
		several boxes can have the same DEVICE name (eg LED0) however
		the configuration file can recognise and react differently to
		each one.
 	

 eg:
	you want to the Arduino to light LED0 if the gear annunciator warning
	in X-Plane is set:

 	D:LED0:sim/cockpit/warnings/annunciators/gear_unsafe:::BOX2

	You want the Arduino to light LED1 if the gear deployment ratio value in
	X-Plane is 0 and turn led off otherwise:

 	D:LED1:sim/flightmodel/2/gear/deploy_ratio:0:EQ 0?1,0:BOX2
``` 


Writing Code for an Arduino in the YAAXI system
===============================================

The process typically will some standard files (main.cpp, Makefile helper.mk,
simulatePlugin.sh etc).  These standard files should require no (or very minor)
changes between projects.

You write your customised device code in the file box.cpp in which you will
implement several functions declared in box.h (eg a method that sets up your
Arduino pins, a method that the system will repeatedly call where you should
read any sensor values, a method that sets your output devices if it receives a
message from X-Plane etc).

The functions you should implement (and what they do) are listed in the:
```
boxes/README.md
```
file.  Use the example boxes as a guide.


You will probably also need to set some pre-processor definitions in box.h to
match your network and may need to modify the Makefile to match your device (eg
if a Uno or Mega etc).


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



### Fishing for an X-Plane plugin on the network ###
```
	format: "XP Plugin Fish"
		try and get a response from the plugin
		(expecting an "Avduino Box Fish" message in return)
```

### sending box capabilities to X-Plane ###
```
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

```
This responses currently not actually used by the plugin however it is meant to allow
for future enhancements like a GUI configuration system that needs to see what
messages the Arduino box is capable of sending and what messages it reacts to.


### sending control information to X-Plane ###
```

	format: "NAME:Y"
		where NAME = control name (eg SWITCH2)
		      Y = value 
		eg "TWIST1:270" or "SWITCH2:1"
```



messages to box from X-Plane:
------------------------

The box reads UDP messages sent to it on port 8888

### X-Plane responding to a fishing message from a box ###
```
format: "XP Avduino Fish" 
	
	treat IP address this message comes from as the IP address of the X-Plane
	plugin (and respond with a description of the box)
```

### X-Plane sending data to a box ###
```
format: "NAME:Y"
	set device value in box
		where NAME = device name (eg LED1)
		      Y = value

	eg:
		LED2:1	 - means turn LED2 on
```

### all other messages: ###
```
	return unique box identifier
```

see simulatePlugin.sh and box.h in the "simplest box" for examples of communication protocol

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

Make sure the following ini entries are in the X-Plane plugin configuration file:
```
################# SIMPLEST TEST BOX
# typical X-Plane config values to add

# give BOX an index and enter its IP address
A:BOX0:192.168.0.189:

# Initial values to box
I:LED0:0:BOX0

# What to do with messages from box
C:SWITCH0:sim/ice/pitot_heat0_on:sim/ice/pitot_heat0_off:

# What to send to box
D:LED0:nav_lights_on::EXACT?,:BOX0
```

NB the configuration file lives in the YAAXI plugin directory.

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

