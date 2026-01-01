# Barometer Pedal

Replaces a Roland DP-2 Foot-Pedal for an E-Piano with an electronic switch, driven by air-pressure applied with the mouth.
Intended for handicapped people playing e-Piano but can not use a foot-switch, lile the Roland DP-2.

## Hardware

### Simple version 

* Arduino Pro Micro
* Air-Pressure Sensor NXP : MPXV 7007 DP
* One 5V Arduino relay

### Flexible Version

* Arduino Pro Micro
* Air-Pressure Sensor NXP : MPXV 7007 DP
* Two 5V Arduino relay ( to allow reacting on over- and under-pressure)
* Two potentiometers ( to allow changing the needed pressure-level per channel )
* Two switches ( to allow opening - like the Roland DP-2 - closing a circuit )


## Software 

Simple Arduino-Sketch, reading the pressure sensor, comparing it against the upper-switch-level and switching a relay and the same for the lower switching level.

