/*
Libraries used:

QMC5883L Compass
Arduino library for using QMC5583L series chip boards as a compass.
by MPrograms | https://github.com/mprograms/QMC5883LCompass

SolarPosition
Arduino Library to calculate the position of the sun relative to geographic coordinates
by KenWillmott | https://github.com/KenWillmott/SolarPosition

TinyGPS++ Library
Rui Santos
Complete Project Details https://randomnerdtutorials.com

*/
#include <Wire.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SolarPosition.h>
#include <QMC5883LCompass.h>
#include <Stepper.h>

// define RX and TX pins
static const int RXPin = 0, TXPin = 1;
// define default baud rate for the GPS module
static const uint32_t GPSBaud = 9600;

// the TinyGPS++ object
TinyGPSPlus gps;
// QMC5883LCompass object
QMC5883LCompass compass;

// the serial connection to the GPS device
SoftwareSerial ss(TXPin, RXPin);
// pointer to SolarPosition
SolarPosition* solarTracker = nullptr;  

// initialize variables for azimuth
float azimuthDiff = 0;
int currentAzimuth = 0;
float wantedAzimuth = 0;

// initialize variables for elevation
float currentElevation = 90.0; // set elevation angle to vertical position of the umbrella, 90 degrees
float wantedElevation = 90.0; // set elevation angle to vertical position of the umbrella, 90 degrees
float elevationDiff = 0;

const int ratedSpeed = 9; // 9 rpm from motor specs
const int NoLoadSpeed = 12; // 12 rpm from motor specs
// seconds to spin for 1 revolution
const float secondsPerRevolution = 60.0 / ratedSpeed;
// seconds to spin for 1 degree increment
const float secondsPerDegree = secondsPerRevolution / 360.0;

// number of steps per output rotation
const float stepsPerRevolution = 200.0; // Stepper motor specs
const float stepsPerDegree = stepsPerRevolution / 360.0; // 90 degrees is the full range

// Button connected to pin 7
const int buttonPin = 7; 

int x; // X sensor reading
int y; // Y sensor reading
int z; // Z sensor reading
int a; // azimuth, compass degree (0-359 degrees)
int b; // 6 point Azimuth bearing direction (0 - 15)
int counter = 0; // initialize counter for first run

// define stepper motor connections:
#define dirPin 2 
#define stepPin 3
#define stepsPerRevolution 200

// define DC motor (bottom motor) pins
int in1 = 10;
int in2 = 11;

char myArray[3]; //h old letters representing each direction,  16 point Azimuth bearing Names
// (N, NNE, NE, ENE, E, ESE, SE, SSE, S, SSW, SW, WSW, W, WNW, NW, NNW)

// declare global variable to set button state
bool resetInitiated = false;

void setup(){
  Serial.begin(9600);
  delay(800);
  // declare pins as output for the DC motor
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  // initialize pin 7 as input with pull-up resistor
  pinMode(buttonPin, INPUT_PULLUP); 

  // turn off the DC motor - Initial state
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  // declare pins as output for the Stepper motor
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  // set the spinning direction CW (clockwise)
	digitalWrite(dirPin, HIGH);

  // initialize the serial port:
  compass.init();
  // set values acquired from compass calibration (different for each use and location)
  compass.setCalibrationOffsets(-529.00, 297.00, -431.00);
  compass.setCalibrationScales(0.81, 0.77, 2.14);
  compass.setSmoothing(10, true);
  // set magnetic declination in Boston to be -8.40° (-8°24'),  NEGATIVE (WEST)
  // magnetic declination is the angle between magnetic north and true north changes with geographic location
  compass.setMagneticDeclination(-8, 24);
  ss.begin(GPSBaud);
}

void loop() {
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      if (gps.location.isUpdated()) {
       // obtain GPS data
       // get the latitude and longitude
       float latitude = gps.location.lat();
       float longitude = gps.location.lng();
       time_t currentTime = convertGPSTimeToTimeT(gps.date, gps.time);

       // delete the old object 
       delete solarTracker;
       // instantiate the solarTracker with known values
       solarTracker = new SolarPosition(latitude, longitude);
       SolarPosition_t solarPosition = solarTracker->getSolarPosition(currentTime);
      
       // print solar data from the GPS
       Serial.print("Solar Azimuth: ");
       Serial.print(solarPosition.azimuth);
       Serial.print(" degrees, Elevation: ");
       Serial.print(solarPosition.elevation);
       Serial.println(" degrees");

       compass.read(); // read compass values via I2C
        x = compass.getX();
        y = compass.getY();
        z = compass.getZ();
        a = compass.getAzimuth(); // calculated from x and y values
        b = compass.getBearing(a); // divide the azimuth value into a 0-15 representation
        compass.getDirection(myArray, a);

       // print azimuth and bearing values
       Serial.print(" Azimuth: ");
       Serial.print(a);
       Serial.print(" Bearing: ");
       Serial.println(b);
       delay(5000);

        // first run of code 
        if (counter == 0) {
          // set current azimuth to the azimuth value acquired from compass (initial orientation), degrees from north
          currentAzimuth = a;
          counter += 1; // increment counter
        // set current azimuth to wanted solar azimuth value after initialization and motor spin
        } else {
          currentAzimuth = wantedAzimuth;
        }
          
        // set wanted azimuth value to the real-time solar azimuth value from the GPS sensor output 
        wantedAzimuth = solarPosition.azimuth;
        // set wanted elevation value (in degrees) to the real-time solar elevation value from the GPS sensor output 
        wantedElevation = solarPosition.elevation;
        Serial.print("Wanted Azimuth: ");
        Serial.println(wantedAzimuth);
        Serial.print(" Wanted Elevation: ");
        Serial.println(wantedElevation);
        Serial.print(" Current Elevation: ");
        Serial.println(currentElevation);

          // initiate movement (motor spin) when real-time solar azimuth value is bigger or equal to 0
          // time period between sunrise and sunset, when there is light
          if (wantedAzimuth >=0) {
            // calculate the difference between current wanted values
            azimuthDiff = wantedAzimuth - currentAzimuth;
            elevationDiff = wantedElevation - currentElevation;
            Serial.print(" Azimuth Difference: ");
            Serial.println(azimuthDiff);
            Serial.print(" Elevation Difference: ");
            Serial.println(elevationDiff);
            // check if difference between current and solar azimuth is 5 degrees
            // average azimuth rate is 15°/hour, with 5° corresponding to 20 minutes, a reasonable difference to start the motor 
            if (abs(azimuthDiff) >= 5.0) {
              // move based on solar position and adjust corresponding seconds to spin
              // assuming rated speed is 9 rpm
              float desiredSeconds = secondsPerDegree*abs(azimuthDiff);
              // convert the time from seconds to milliseconds
              float secondsToSpin = desiredSeconds*1000;
              Serial.print(" Azimuth Difference bigger than 5 degrees: ");

              // azimuth difference is positive
              if (azimuthDiff > 0) {
                Serial.println(" Azimuth difference is positive");
                // turn on bottom motor
                // set the direction to CW (clockwise)
                digitalWrite(in1, HIGH);
                digitalWrite(in2, LOW);
                delay(secondsToSpin);
                // turn off bottom motor
                digitalWrite(in1, LOW);
                digitalWrite(in2, LOW);
                // update current azimuth to be the same as solar azimuth
                currentAzimuth = wantedAzimuth;
                Serial.println("Bottom Motor moved to new position");
                Serial.print("current azimuth: ");
                Serial.println(currentAzimuth);
                delay(5000);
              }
              // azimuth difference is negative
              else if (azimuthDiff < 0) {
                Serial.println(" Azimuth difference is negative");
                // turn on bottom motor
                // set the spinning direction CCW
                digitalWrite(in1, LOW);
                digitalWrite(in2, HIGH);
                delay(secondsToSpin);
                // turn off bottom motor
                digitalWrite(in1, LOW);
                digitalWrite(in2, LOW);
                // update current azimuth to be the same as solar azimuth value from GPS output 
                currentAzimuth = wantedAzimuth;
                Serial.println("Bottom Motor moved to new position");
                Serial.print("Current Azimuth: ");
                Serial.println(currentAzimuth);
                delay(5000);
              }
            }

            // check if difference between current and solar elevation is 5 degrees
            // sun's elevation angle could change by about 6 to 12 degrees per hour
            if (abs(elevationDiff) >= 5.0) { 
              // gearbox ratio, 20:1, pulley diameter ratio is roughly 2:1 (top to bottom)
              // set required steps to spin 40 times more to match 1:40 ratio
              float requiredSteps = 40*abs(elevationDiff)*stepsPerDegree;
              Serial.print(" Elevation Difference is bigger than 5 degrees: ");

              // solar elevation difference is positive
              if (elevationDiff >0) {
                Serial.println(" Elevation difference is positive");
                // spin the top motor and move necessary steps by sending pulses
                for (int i = 0; i < requiredSteps; i++) {
                  // set the spinning direction CW
	                digitalWrite(dirPin, HIGH);
                  digitalWrite(stepPin, HIGH);
                  delayMicroseconds(1000);  // Adjust speed 
                  digitalWrite(stepPin, LOW);
                  delayMicroseconds(1000);  // Adjust speed 
                }
                // update current elevation to be the same as solar elevation
                currentElevation = wantedElevation;
                Serial.println(" Top motor moved to new position");
                Serial.print("Current Elevation: ");
                Serial.println(currentElevation);
                delay(5000);
              }
              // solar elevation difference is negative
              else if (elevationDiff <0) {
                Serial.println("Elevation difference is negative");
                // spin the top motor and move necessary steps by sending pulses
                for (int i = 0; i < requiredSteps; i++) {
                  // set the spinning direction CCW
	                digitalWrite(dirPin, LOW);
                  digitalWrite(stepPin, HIGH);
                  delayMicroseconds(1000);  // Adjust speed 
                  digitalWrite(stepPin, LOW);
                  delayMicroseconds(1000);  // Adjust speed 
                }
                // update current elevation to be the same as solar elevation
                currentElevation = wantedElevation;
                Serial.println(" Top motor moved to new position");
                Serial.print("current elevation: ");
                Serial.println(currentElevation);
                delay(5000);
              }
            }  
          } 

        // 
        if (digitalRead(buttonPin) == LOW) { 
          resetInitiated = true;
          wantedElevation = 90.0; 
          Serial.print(" Elevation difference to initial position: ");
          Serial.println(elevationDiff);
          float requiredSteps = 40*abs(elevationDiff)*stepsPerDegree;
              if (elevationDiff > 0) {
                Serial.println(" Elevation difference is positive");
                // Move the motor
                for (int i = 0; i < requiredSteps; i++) {
                  // Set the spinning direction CCW
	                digitalWrite(dirPin, LOW);
                  digitalWrite(stepPin, HIGH);
                  delayMicroseconds(500);  // Adjust speed to be faster
                  digitalWrite(stepPin, LOW);
                  delayMicroseconds(500);  // Adjust speed to be faster
                }
                currentElevation = wantedElevation;
                Serial.println(" Top motor moved to initial position (90 degrees)");
                Serial.print("current elevation: ");
                Serial.println(currentElevation);
                delay(5000);
              }
        } else {
          resetInitiated = false;
        }
      } 
    }   
  }
}

 // Function to convert GPS date and time to time_t (Unix time)
time_t convertGPSTimeToTimeT(TinyGPSDate gpsDate, TinyGPSTime gpsTime) {
 // The time_t type is a standard C/C++ data type used to represent time
 // the number of seconds since the Unix epoch (00:00:00 UTC on 1 January 1970).
 tmElements_t tm;
 tm.Year = gpsDate.year() - 1970;
 tm.Month = gpsDate.month();
 tm.Day = gpsDate.day();
 tm.Hour = gpsTime.hour();
 tm.Minute = gpsTime.minute();
 tm.Second = gpsTime.second();
 return makeTime(tm);
}