General Box description
=======================
This device is used to implement a GNS430/530 type GPS.

It is built using an Arduino Mega (with ethernet shield).

It uses the X-Plane GNS430 floating window that gets displayed on a small
screen. 


![Simplest Panel](gnx530-front.jpg?raw=true "Prototype GNS530 box (front)")
![Simplest Panel](gnx530-top.jpg?raw=true "Prototype GNS530 box (top)")

It is actually a fairly simple device, the arduino just sends info on button
presses and the pair of rotary encoders but doesn't respond to anything from
X-Plane.  This version doesn't simulate the COM and NAV controls that are on a
real GNS430/530, this could be done but would require adding another pair of
encoders and an extra button.

There are 2 leds that currently are just used to indicate diagnostic info on
the network initialisation, they could respond to stuff from X-Plane if wanted.

The screen is connected as another monitor.  It is a 7inch LCD 1024x600 pixel
screen typically used to interface to Raspberry PIs etc.  It operates
completely independently of the arduino - this is all done by X-Plane using
standard X-Plane functionality.  You need to click on the GNS430/530 in X-Plane
to make it floating and drag it to the small screen when starting.


Typical ini values to add to the X-Plane Arduino plugin ini file
----------------------------------------------------------------

```
############# NEW GNS 430/530 BOX
# let's call it BOX1
A:BOX1:192.168.0.191:

# Initial values to box
# at the moment - nothing!

# What to do with messages from box
C:CDI:sim/GPS/g430n1_cdi::
C:OBS:sim/GPS/g430n1_obs::
C:MSG:sim/GPS/g430n1_msg::
C:FPL:sim/GPS/g430n1_fpl::
C:PROC:sim/GPS/g430n1_proc::
C:ENTER:sim/GPS/g430n1_ent::
C:CLR:sim/GPS/g430n1_clr::
C:MENU:sim/GPS/g430n1_menu::
C:DTO:sim/GPS/g430n1_direct::
C:CRSR:sim/GPS/g430n1_cursor::
C:RANGEIN:sim/GPS/g430n1_zoom_in::
C:RANGEOUT:sim/GPS/g430n1_zoom_out::
C:OUTER_NAV_KNOB:sim/GPS/g430n1_chapter_up:sim/GPS/g430n1_chapter_dn:
C:INNER_NAV_KNOB:sim/GPS/g430n1_page_up:sim/GPS/g430n1_page_dn:

# What to send to box 
# at the moment - nothing!

```
