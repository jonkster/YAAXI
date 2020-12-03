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

Typical ini values to add to the X-Plane Arduino plugin ini file
----------------------------------------------------------------
```
################# COM/NAV RADIO BOX
# typical X-Plane config values to add

# give BOX an index and enter its IP address
A:BOX4:192.168.0.190:

# Initial values to send to Arduino
I:C1U:12455:BOX4
I:C1S:12465:BOX4
I:C2U:12475:BOX4
I:C2S:12485:BOX4
I:N1U:10905:BOX4
I:N1S:10915:BOX4
I:N2U:10925:BOX4
I:N2S:10935:BOX4

# What to do with messages from Arduino
C:FLIP_COM1:sim/radios/com1_standy_flip::
C:FLIP_COM2:sim/radios/com2_standy_flip::
C:FLIP_NAV1:sim/radios/nav1_standy_flip::
C:FLIP_NAV2:sim/radios/nav2_standy_flip::
C:MHZ_KNOB_COM1:sim/radios/stby_com1_coarse_down:sim/radios/stby_com1_coarse_up:
C:KHZ_KNOB_COM1:sim/radios/stby_com1_fine_down:sim/radios/stby_com1_fine_up:
C:MHZ_KNOB_COM2:sim/radios/stby_com2_coarse_down:sim/radios/stby_com2_coarse_up:
C:KHZ_KNOB_COM2:sim/radios/stby_com2_fine_down:sim/radios/stby_com2_fine_up:
C:MHZ_KNOB_NAV1:sim/radios/stby_nav1_coarse_down:sim/radios/stby_nav1_coarse_up:
C:KHZ_KNOB_NAV1:sim/radios/stby_nav1_fine_down:sim/radios/stby_nav1_fine_up:
C:MHZ_KNOB_NAV2:sim/radios/stby_nav2_coarse_down:sim/radios/stby_nav2_coarse_up:
C:KHZ_KNOB_NAV2:sim/radios/stby_nav2_fine_down:sim/radios/stby_nav2_fine_up:
#
# What to send to Arduino
D:C1U:sim/cockpit/radios/com1_freq_hz::EXACT?,:BOX4
D:C1S:sim/cockpit/radios/com1_stdby_freq_hz::EXACT?,:BOX4
D:C2U:sim/cockpit/radios/com2_freq_hz::EXACT?,:BOX4
D:C2S:sim/cockpit/radios/com2_stdby_freq_hz::EXACT?,:BOX4
D:N1U:sim/cockpit/radios/nav1_freq_hz::EXACT?,:BOX4
D:N1S:sim/cockpit/radios/nav1_stdby_freq_hz::EXACT?,:BOX4
D:N2U:sim/cockpit/radios/nav2_freq_hz::EXACT?,:BOX4
D:N2S:sim/cockpit/radios/nav2_stdby_freq_hz::EXACT?,:BOX4
```
