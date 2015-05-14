#include <Servo.h>
    
Servo servoScanner;
Servo servoLeft;
Servo servoRight;

const int pingPinHigh = 6;
const int pingPinLow = 7;
const unsigned long sonarThreshold = 10; // cm
const unsigned long sonarRange = 60; //cm
const int sonarReadDelay = 20; // ms

const int scanServoAngleOffset = 8;
const int scanServoAngleLimit = 60; // degrees
int scanServoAngle = 0; // degrees
int scanServoAngleDelta = 5; // degrees

bool sawThisSweep = false;
bool seesPuck = false;
bool sawPuckLastCheck = false;
int puckStartAngle = 0;
int puckEndAngle = 0;

bool foundPuck = false;
int foundPuckAngle = 0;
int foundPuckStraightAheadTime = 0;

void setup() {
  Serial.begin(9600);
  tone(2, 3000, 1000);
  servoScanner.attach(11);
  servoLeft.attach(13);
  servoRight.attach(12);
}

void loop() {
  scanForPuck();
  
  int speedLeft = 0;
  int speedRight = 0;
  
  if (foundPuck) {
    
    if (foundPuckStraightAheadTime > 4) {
      speedLeft += 50;
      speedRight += 50;
    } else {
      int dir = -(float)foundPuckAngle * 0.4f; // scale puck angle to make it more suitable for driving the motors
      speedLeft -= dir;
      speedRight += dir;
    }
  }
  
  servoLeft.writeMicroseconds(1500 - speedLeft);
  servoRight.writeMicroseconds(1500 + speedRight);
  
  //Serial.print("\n");
}

unsigned long checkSonar(int pingPin) {
  // Start ranging
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  
  // Wait for the result
  pinMode(pingPin, INPUT);
  unsigned long duration = pulseIn(pingPin, HIGH, (unsigned long)3000);//5772); // Read echo pulse
  delay(sonarReadDelay); // Make sure that we wait to let the sound die out so that it doesn't fool any other sensor
  
  if (duration == 0) {
    duration = 3000;
  }
  
  unsigned long inches = duration / 148;
  unsigned long cm = inches * 2.54;
  
  //Serial.print(pingPin);
  //Serial.print(" cm: ");
  //Serial.print(cm);
  //Serial.print("\t");
  
  // Make sure that we reset the pin before leaving the function
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  
  return cm;
}

/* Will scan for pucks and walls. 
* this function is communicating it's result by setting some global variables:
* ...
*/
void scanForPuck() {
  unsigned long lowDist = checkSonar(pingPinLow);
  unsigned long highDist = checkSonar(pingPinHigh);
  
  // Only look for a puck once during each sweep. Makes the check super robust!
  if (!sawThisSweep) {
    // Update puck observation
    sawPuckLastCheck = seesPuck;
    seesPuck = ((lowDist + sonarThreshold < highDist) && (lowDist < sonarRange)); // do we see a puck?
    
    if (seesPuck && !sawPuckLastCheck) {
      puckStartAngle = scanServoAngle;
    }
    else if (!seesPuck && sawPuckLastCheck) {
      puckEndAngle = scanServoAngle;
      sawThisSweep = true;
    }
  }
  
  // Rotate sensor
  scanServoAngle += scanServoAngleDelta;
  
  if (abs(scanServoAngle) > scanServoAngleLimit) {
    // Sweep ended
    foundPuck = sawThisSweep;
    
    if (foundPuck) {
      foundPuckAngle = (puckStartAngle + puckEndAngle) / 2;
      
      // Keep track of for how many sweeps the puck is straight ahead
      if (abs(foundPuckAngle) < 20) {
        foundPuckStraightAheadTime++;
        tone(2, 3000, 100);
      } else {
        foundPuckStraightAheadTime = 0;
      }
    }
    
    // Start new sweep and change direction
    sawThisSweep = false;
    seesPuck = false;
    sawPuckLastCheck = false;
    puckStartAngle = 0;
    puckEndAngle = 0;
    scanServoAngleDelta = -scanServoAngleDelta;
  }
  
  int servoTiming = 1500 + ((long)(scanServoAngleOffset + scanServoAngle) * 820) / 90;
  servoScanner.writeMicroseconds(servoTiming);
}
