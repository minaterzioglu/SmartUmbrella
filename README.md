# SmartUmbrella
Solar position tracking and adjustment Arduino code for the Mechanical Engineering Senior Capstone Project.

## Project Overview
A smart umbrella that autonomously adjusts its position based on the sun's position movement to provide increased sun protection and minimize the time, effort, and energy beachgoers spend for position adjustment.

## Tilting & Rotation Mechanisms
The umbrella is comprised of two parts and a pulley-enabled system. The top part tilts using a stepper motor, which spins based on the elevation (altitude) angle of the sun, the sun's height measured in degrees from the observer's local horizon. Tilting is in the range of 0-90 degrees, with 0° being sunrise and 90° when the sun is directly overhead. The bottom motor rotates based on the compass module and adjusts to the solar azimuth, the direction of the sun measured clockwise in degrees from north. The compass detects the initial orientation of the umbrella and further orients to the direction of the sun, within a 360-degree range of motion. Both motors rotate as the sun's azimuth and elevation change 5 degrees, which is a reasonable threshold value.

# Code
## Required Libraries
This code relies on the following libraries:

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
VCC  O ---- O +3.3V\
GND  O ---- O GND\
SCL  O ---- O SCL pin\
SDA  O ---- O SDA pin\
DRDY O ---- X NOT CONNECTED

Note: If Arduino module HMC5883L(GY-273) does not work with its library, check to make sure that it's actually HMC5883L, not QMC5883\
[HMC5883L Datasheet ](https://www.best-microcontroller-projects.com/support-files/hmc5883l.pdf)\
[QMC5883L Datasheet ](https://www.best-microcontroller-projects.com/support-files/qmc5883l-datasheet-1.0.pdf)\
[Helpful link to differentiate between 2 chips](https://www.best-microcontroller-projects.com/support-files/qmc5883l-datasheet-1.0.pdf](https://surtrtech.com/2018/02/01/interfacing-hmc8553l-qmc5883-digital-compass-with-arduino/)https://surtrtech.com/2018/02/01/interfacing-hmc8553l-qmc5883-digital-compass-with-arduino/)

### NEO-6M GPS hookup to Arduino
VCC  O ---- O +5V\
GND  O ---- O GND\
RX  O ---- O RX pin\
TX  O ---- O TX pin

### Positioning of QMC5883L & Calibration
- Place horizontal to the ground to get an accurate reading. Three-axis Magnetometers are sensitive to their orientation.
- Orient the x-axis on the compass in the direction of tilt (top motor, 90°)\
  When the X-axis is aligned with the geographic north, the heading reading is approximately 0°, pointing to the north  direction
- Calibrate before using and use the calibration data. QMC5883LCompass library includes a calibration function and utility sketch to calibrate the chip.
- Adjust the magnetometer reading for the magnetic declination offset. The magnetometer measures magnetic north which is affected by the Earth's magnetic field. Magnetic declination is the angle between magnetic north and true north which changes with geographic location. Solar position algorithms measure solar azimuth and elevation relative to true north, not magnetic north. Get magnetic declination from [this API](https://www.ngdc.noaa.gov/geomag/calculators/magcalc.shtml#declination)
  
Note: When QMC5883L is not level, the output bearing will be incorrect. Using an accelerometer will compensate for the tilt in the magnetometer. [More information at this link ](https://www.best-microcontroller-projects.com/hmc5883l.html). 
