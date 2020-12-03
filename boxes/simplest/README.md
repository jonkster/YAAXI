General Box description
=======================
This device is a very simple example mainly used to ensure the development
environment is working and communication with X-Plane is OK.

It is built using an Arduino Uno (with ethernet shield).

It needs a led connected to pin 8 and a switch to pin 9.

The system will light the led if the nav lights are turned on in X-Plane and
will flick the X-Plane pitot heat switch if the physical switch on the Arduino
is turned on (well to be exact grounded)

![Simplest Panel](simplest.jpg?raw=true "Simplest Arduino setup")


Typical ini values to add to the X-Plane Arduino plugin ini file
----------------------------------------------------------------

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
