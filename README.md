# SmartUmbrella
Solar position tracking and adjustment Arduino code for the Mechanical Engineering Senior Capstone Project.

## Usage
A smart umbrella that autonomously adjusts its position based on the sun's position movement to provide increased sun protection and minimize time, effort and energy beachgoers spend for umbrella position adjustment.

## Modules
This code uses a NEO-6M GPS module with the Arduino to get GPS data and QMC5583L series chipboards as a compass.

##Tilting & Rotation Mechanism
The top stepper motor tilts based on the elevation (altitude) angle of the sun, the sun's height measured in degrees from the observer's local horizon. The top tilting is in the range of 0-90 degrees, with 0 degrees being sunrise and 90 degrees when the sun is directly overhead. The bottom motor rotates based on a compass-enabled system and adjusts to the solar azimuth, the direction of the sun measured clockwise in degrees from north. The compass detects the initial orientation of the umbrella and further orients to the direction of the sun, within a 360 degrees range of motion. Both motors rotate as the sun's azimuth and elevation change 5 degrees, which is a reasonable threshold value.

## Board Hookup Reference

