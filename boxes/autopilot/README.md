General Box description
=======================
This device is used to implement a generic autopilot that can be used in
typical general aviation aircraft in X-Plane.

It is built using an Arduino Mega (with ethernet shield).

Box is not complete yet.

**Images of prototype:**
![AP](ap-front.jpg?raw=true "Prototype autopilot box (front)")
![AP](ap-top.jpg?raw=true "Prototype autopilot box (top)")


The box is not meant to be an exact model of an actual real world autopilot
(mostly because X-Plane doesn't replicate a lot of GA autopilot functionality
exactly anyway) but it is roughly similar in function to a KAP140 or the Garmin
G1000 autopilot.

It also allows you to adjust the DI/HSI/RMI heading bug and OBS via knobs.

Autopilot mode annunciators and altitude selection are displayed on a 128x64
dot matrix LCD display, (ST7920 chipset) that is driven as a serial (SPI)
device.  The messages to display are triggered by messages sent from X-Plane.

The system has potential to add sound effects, possible enhancement (eg
altitude, trim warnings etc like the KAP140 issues)

The unit has 4 rotary encoders, one each for HDG and OBS and 2 for altitude
selection (one for thousands and one for hundreds of feet - these 2 use a dual
concentric encoder - same idea as the G1000 altitude selector).

It also has 7 SPST momentary action push button switches to change autopilot
modes and the rotary encoders push button switches are used to sync the heading
bug to current heading, OBS to current heading and altitude selection to
current altitude.

In addition there is a 2 position rocker switch to drive pitch up or pitch down
when in VS mode.

These switches generate messages that are sent to X-Plane and via the ini file
should set appropriate modes in the X-Plane autopilot.

Altitude capture is done via logic on the box as this functionality doesn't
seem to be able to be easily driven via a command in the X-Plane simulation.
The box recognises it is in VS mode and when approaching the assigned altitude
sends the ALT mode to X-Plane.


In addition, because there was panel real estate available on the AP facia on
my setup I used this to add gear and flap annunciator lights that are also
controlled by the same arduino device that controls the AP.  Obviously not part
of an autopilot but was convenient.


Typical ini values to add to the X-Plane Arduino plugin ini file
----------------------------------------------------------------

```
################# AUTOPILOT BOX
# let's call it BOX2
A:BOX2:192.168.0.192:
# Initial values to arduino box
I:AP_ON:0:BOX2
I:FD_ON:1:BOX2
# auunciator lights (not part of AP)
I:GEAR_TRANSIT:0:BOX3
I:GEAR_SAFE0:1:BOX3
I:GEAR_SAFE1:1:BOX3
I:GEAR_SAFE2:1:BOX3
I:FLAP_TRANSIT:0:BOX3
I:FLAP_APP:0:BOX3
I:FLAP_LAND:0:BOX3

# What to do with messages from arduino box
C:AP_TOGGLE:sim/autopilot/servos_toggle::
C:FD_TOGGLE:sim/autopilot/fdir_toggle::
C:HDG_TOGGLE:sim/autopilot/heading::
C:NAV_TOGGLE:sim/autopilot/NAV::
C:ALT_TOGGLE:sim/autopilot/altitude_hold::
C:APR_TOGGLE:sim/autopilot/approach::
C:VS_TOGGLE:sim/autopilot/vertical_speed_pre_sel::
C:AP_UP:sim/autopilot/vertical_speed_up::
C:AP_DN:sim/autopilot/vertical_speed_down::
DR:ALT_ASSIGNED:sim/cockpit/autopilot/altitude:::
C:ALT_SYNC_TOGGLE:sim/autopilot/altitude_sync:::
C:HDG_SYNC_TOGGLE:sim/autopilot/heading_sync:::
C:CRS_KNOB:sim/radios/obs1_up:sim/radios/obs1_down:
C:HDG_KNOB:sim/autopilot/heading_down:sim/autopilot/heading_up:
C:ALT_HU_KNOB:sim/autopilot/altitude_down:sim/autopilot/altitude_up:
#ALT_TH_KNOB
#OBS_SYNC_TOGGLE

# What to send to arduino box
D:AP_MODE:sim/cockpit2/annunciators/autopilot::EXACT:BOX2
D:FD_MODE:sim/cockpit2/annunciators/flight_director::EXACT:BOX2
D:HDG_MODE:sim/cockpit2/autopilot/heading_mode::EXACT:BOX2
D:ALT_MODE:sim/cockpit2/autopilot/altitude_mode::EQ 6?1,0:BOX2
D:NAV_MODE:sim/cockpit2/autopilot/nav_status::EQ 2?1,0:BOX2
D:VS_MODE:sim/cockpit2/autopilot/vvi_status::EQ 2?1,0:BOX2
#D:APR_MODE:sim/cockpit2/autopilot/nav_status::EQ 2?1,0:BOX2
D:ALT_SEL:sim/cockpit/autopilot/altitude::EXACT:BOX2
D:AP_VS:sim/cockpit/autopilot/vertical_velocity::EXACT:BOX2
D:ALTITUDE:sim/cockpit2/gauges/indicators/altitude_ft_pilot::EXACTIFDIFFGT 5?,:BOX2
# auunciator lights (not part of AP)
D:FLAP_APP:sim/flightmodel/controls/flaprat::EQ 0.400000?1,0:BOX0
D:FLAP_LAND:sim/flightmodel/controls/flaprat::EQ 1.000000?1,0:BOX0
D:FLAP_TRANSIT:sim/flightmodel/controls/flaprat::CHG?1,0:BOX0
D:GEAR_TRANSIT:sim/cockpit/warnings/annunciators/gear_unsafe::EXACT?,:BOX0
D:GEAR_SAFE0:sim/flightmodel2/gear/deploy_ratio:0:EQ 1?1,0:BOX0
D:GEAR_SAFE1:sim/flightmodel2/gear/deploy_ratio:0:EQ 1?1,0:BOX0
D:GEAR_SAFE2:sim/flightmodel2/gear/deploy_ratio:0:EQ 1?1,0:BOX0

```
