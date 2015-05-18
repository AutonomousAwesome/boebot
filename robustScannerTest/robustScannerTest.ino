#include <Servo.h>
    
Servo servoScanner;
Servo servoLeft;
Servo servoRight;

const int pingPinHigh = 6;
const int pingPinLow = 7;
const int sonarReadDelay = 5; // ms

const unsigned long puckRange = 40; // cm
const unsigned long puckThreshold = 10; // cm
const int puckSideToCenterAngleEstimate = 4; // degrees

const int scanAngleOffset = 8;
const int scanAngleLimit = 60; // degrees
const int scanAngleCenter = 15;
int scanAngle = 0; // degrees
int scanAngleDelta = 5; // degrees

bool sawThisSweep = false;
bool foundPuck = false;
int foundPuckAngle = 0;
int foundPuckStraightAheadTime = 0;

void setup() {
  Serial.begin(9600);
  tone(2, 3000, 500);
  servoScanner.attach(11);
  //servoLeft.attach(13);
  //servoRight.attach(12);
}

void loop() {
  sweep();
  
  int speedLeft = 0;
  int speedRight = 0;
  //servoLeft.writeMicroseconds(1500 - speedLeft);
  //servoRight.writeMicroseconds(1500 + speedRight);
  
  //Serial.print("\n");
}

// Moves the scan servo and sweeps to find the puck. Only finds puck when
// sweeping to the right, in order to make it simple and robust.
void sweep() {
  unsigned long highDistance = checkSonar(pingPinHigh);
  unsigned long lowDistance = checkSonar(pingPinLow);
  
  bool seesPuck = (lowDistance + puckThreshold < highDistance) && (lowDistance < puckRange);
  
  if (!sawThisSweep) {
    if (seesPuck) {
      foundPuckAngle = scanAngle;
      
      if (scanAngleDelta > 0) {
        // Sweeping to the right
        foundPuckAngle += puckSideToCenterAngleEstimate;
      } else {
        // Sweeping to the left
        foundPuckAngle -= puckSideToCenterAngleEstimate;
      }
      
      sawThisSweep = true;
      //tone(2, 3000, 100);
    }
  }
  
  // Rotate sensor
  scanAngle += scanAngleDelta;
  
  // Reached end of angle limit?
  if (abs(scanAngle) > scanAngleLimit) {
    // Sweep ended
    foundPuck = sawThisSweep;
    
    // Sweeping right?
    if (scanAngleDelta > 0) {
      // Count how many sweeps to the right the found puck has been straight ahead
      if (foundPuck) {
        if (abs(foundPuckAngle) < scanAngleCenter) {
          foundPuckStraightAheadTime++;
          tone(2, 3000, 100);      
        } else {
          foundPuckStraightAheadTime = 0;
        }
      }
    }
    
    // Sweeping left?
    if (scanAngleDelta < 0) {
      // Reset variables for next sweep to the right
      sawThisSweep = false;
      foundPuck = false;
      foundPuckAngle = 0;
      foundPuckStraightAheadTime = 0;
    }
    
    // Switch scan direction
    scanAngleDelta = -scanAngleDelta;
  }
  
  int servoTiming = 1500 + ((long)(scanAngleOffset + scanAngle) * 820) / 90;
  servoScanner.writeMicroseconds(servoTiming);
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
  
  Serial.print(pingPin);
  Serial.print(" cm: ");
  Serial.print(cm);
  Serial.print("\t");
  
  // Make sure that we reset the pin before leaving the function
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  
  return cm;
}
