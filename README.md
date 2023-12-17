# SmartUmbrella
Solar position tracking and adjustment Arduino code for the Mechanical Engineering Senior Capstone Project.

## Overview
A smart umbrella that autonomously adjusts its position based on the sun's position movement to provide increased sun protection and minimize time, effort and energy beachgoers spend for umbrella position adjustment.

## Tilting & Rotation Mechanisms
The umbrella is comprised of two parts and a pulley-enabled system. The top part tilts using a stepper motor, which spins based on the elevation (altitude) angle of the sun, the sun's height measured in degrees from the observer's local horizon. Tilting is in the range of 0-90 degrees, with 0° being sunrise and 90° when the sun is directly overhead. The bottom motor rotates based on the compass module and adjusts to the solar azimuth, the direction of the sun measured clockwise in degrees from north. The compass detects the initial orientation of the umbrella and further orients to the direction of the sun, within a 360-degree range of motion. Both motors rotate as the sun's azimuth and elevation change 5 degrees, which is a reasonable threshold value.

# Code
## Required Libraries
This code relies on the following libraries

QMC5883L Compass
Arduino library for using QMC5583L series chip boards as a compass\
by MPrograms | https://github.com/mprograms/QMC5883LCompass

SolarPosition
Arduino Library to calculate the position of the sun relative to geographic coordinates\
by KenWillmott | https://github.com/KenWillmott/SolarPosition

TinyGPS++ Library
by Rui Santos\
Complete Project Details https://randomnerdtutorials.com

## Modules
This code uses a NEO-6M GPS module with the Arduino to get GPS data and QMC5583L 3-Axis Digital Compass series chipboards as a compass.

## Board Hookup Reference
For different boards see [Arduino Wire Reference ](https://www.arduino.cc/reference/en/language/functions/communication/wire/). 

### QMC5883L hookup to Arduino
VCC  O ---- O +3.3v\
GND  O ---- O GND\
SCL  O ---- O SCL pin\
SDA  O ---- O SDA pin\
DRDY O ---- X NOT CONNECTED


Note: If Arduino module HMC5883L (GY-273) does not work with its library, check and make sure that it is actually HMC5883L, not QMC5883
[HMC5883L Datasheet] (https://www.best-microcontroller-projects.com/support-files/hmc5883l.pdf). 


QMC5883L
QMC5883L Datasheet.

How to know it’s a real HMC5883L:

If you try after wiring it with arduino with i2c scanner you gonna find that the address is “0x1E“
Or if you look on the chip itself it’s marked L883
How to know it’s a fake one:

If you try after wiring it with arduino with i2c scanner you gonna find that the address is “0x0D” as i found out or something else
Or if you look on the chip itself it’s marked DA5883

### NEO-6M GPS hookup to Arduino


