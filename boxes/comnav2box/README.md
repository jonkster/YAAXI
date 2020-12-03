General Box description
=======================
This device adds a simple COM/NAV radio control head.

It is built using an Arduino Mega (with ethernet shield).

It uses two 2x16 Dot Matrix LCD displays, 2 rotary encoders (one with a push
switch) and a separate push switch.

The system controls 2 COM radios and 2 NAV radios in X Plane.

The rotary encoders adjust the frequency of the currently highlighted COM or NAV radio.
Pushing the rotary encoder cycles through each radio highlighting each in turn.
The idea is similar to the Garmin G1000 where pushing the frequency knob flips
between COM1 and COM2 (or NAV1/NAV2) - in this case the one knob it cycles
between COM1->COM2->NAV1->NAV2.

The push switch swaps between the standby and active frequency in the currently
highlighted radio.

![Front Panel](comnav2box.jpg?raw=true "COM-NAV box front")
![Top View](comnav2box-2.jpg?raw=true "COM-NAV box top view")
