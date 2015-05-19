#include <Servo.h>
    
Servo servoScanner;
Servo servoLeft;
Servo servoRight;

const int pingPinHigh = 6;
const int pingPinLow = 7;
const int sonarReadDelay = 15; // ms

const unsigned long puckRange = 30; // cm
const unsigned long puckThreshold = 10; // cm
const int puckSideToCenterAngleEstimate = 15; // degrees

const int scanAngleOffset = 8;
const int scanAngleLimit = 60; // degrees
const int scanAngleCenter = 6;
int scanAngle = 0; // degrees
int scanAngleDelta = 4; // degrees

bool sawThisSweep = false;
bool sweepJustOver = false;
bool foundPuck = false;
int foundPuckAngle = 0;

void setup() {
  Serial.begin(9600);
  tone(2, 3000, 500);
  servoScanner.attach(11);
  servoLeft.attach(13);
  servoRight.attach(12);
}

int state = 0;
unsigned long time = 0;

void loop() {
  
  int speedLeft = 0;
  int speedRight = 0;
  
  time++;
  
  if (state == 0) {
    sweep();
    
    if (sweepJustOver) {
      if (foundPuck) {
        Serial.print("Found puck at angle: ");
        Serial.println(foundPuckAngle);
        if (abs(foundPuckAngle) < scanAngleCenter) {
          state = 2;
          time = 0;
        } else {
          state = 1;
          time = 0;
        }
      } else {
        state = 2;
        time = 0;
      }
    }
  }
  else if (state == 1) {
    // Rotate
    if (foundPuckAngle > 0) {
      // Rotate right
      speedLeft = 30;
      speedRight = -30;
    }
    else {
      // Rotate left
      speedLeft = -30;
      speedRight = 30;
    }
    
    if (time > 20) {
      state = 0;
      time = 0;
    }
  }
  else if (state == 2) {
    // Go forward
    speedLeft = 30;
    speedRight = 30;
    
    if (time > 200) {
      state = 0;
      time = 0;
    }
  }
    
  servoLeft.writeMicroseconds(1500 - speedLeft);
  servoRight.writeMicroseconds(1500 + speedRight);
  
  //Serial.print("\n");
  delay(10);
}

// Moves the scan servo and sweeps to find the puck. Only finds puck when
// sweeping to the right, in order to make it simple and robust.
void sweep() {
  unsigned long highDistance = checkSonar(pingPinHigh);
  unsigned long lowDistance = checkSonar(pingPinLow);
  
  bool seesPuck = (lowDistance + puckThreshold < highDistance) && (lowDistance < puckRange);
  
  if (!sawThisSweep) {
    if (seesPuck) {
      // Sweeping to the right
      foundPuckAngle = scanAngle + puckSideToCenterAngleEstimate;
      
      sawThisSweep = true;
      tone(2, 3000, 100);
    }
  }
  
  // Rotate sensor
  scanAngle += scanAngleDelta;
  
  // Reached end of angle limit?
  sweepJustOver = false;
  
  if (abs(scanAngle) > scanAngleLimit) {
    // Sweep ended
    foundPuck = sawThisSweep;
    
    // Sweeping right?
    if (scanAngleDelta > 0) {
      sweepJustOver = true;
      scanAngle = 0;
    }
    
    // Sweeping left?
    if (scanAngleDelta < 0) {
      // Reset variables for next sweep to the right
      sawThisSweep = false;
      foundPuck = false;
      foundPuckAngle = 0;
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
  
  //Serial.print(pingPin);
  //Serial.print(" cm: ");
  //Serial.print(cm);
  //Serial.print("\t");
  
  // Make sure that we reset the pin before leaving the function
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  
  return cm;
}
