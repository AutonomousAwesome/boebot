#include <Servo.h>
    
Servo servoScanner;
Servo servoLeft;
Servo servoRight;

const int sonarPinUpper = 6;
const int sonarPinLower = 7;
const int sonarReadDelay = 15; // ms

bool sonarLastReadLower = false;
unsigned long sonarLastReadTime = 0;
unsigned long sonarDistanceUpper = 0; // cm
unsigned long sonarDistanceLower = 0; // cm

const unsigned long puckRange = 30; // cm
const unsigned long puckThreshold = 10; // cm
const int puckSideToCenterAngleEstimate = 15; // degrees
const unsigned long wallThreshold = 20; // cm

const int sweepAngleOffset = 8; // degrees
const int sweepAngleLimit = 60; // degrees
const int sweepAngleCenter = 6; // degrees

int sweepAngle = 0; // degrees
int sweepAngleDelta = 4; // degrees

bool foundWall = false;
int foundWallAngle = 0; // degrees

bool sweepJustOver = false;
bool sawThisSweep = false;
bool foundPuck = false;
int foundPuckAngle = 0; // degrees
unsigned long foundPuckDistance = 0; // cm

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
  
  // Test state machine
  updateSonars();
  
  checkForWall();
  
  if (foundWall) {
    state = 4;
  }
  
  int speedLeft = 0;
  int speedRight = 0;
  
  time++;
  
  if (state == 0) { // Sweep state, has to do this occasionally
    sweepForPuck();
    
    if (sweepJustOver) {
      if (foundPuck) {
        Serial.print("Found puck at angle: ");
        Serial.println(foundPuckAngle);
        
        if (abs(foundPuckAngle) < sweepAngleCenter) {
          
          if (foundPuckDistance < 20) {
            state = 3; // Eat!
            time = 0;
          } else {
            state = 2; // Nudge forward
            time = 0;
          }
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
  else if (state == 1) { // Nudge rotate towards a puck
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
  else if (state == 2) { // Nudge forward towards puck
    speedLeft = 30;
    speedRight = 30;
    
    if (time > 200) {
      state = 0;
      time = 0;
    }
  }
  else if (state == 3) { // Go forward and eat puck!
    speedLeft = 30;
    speedRight = 30;
    
    if (time > 500) {
      state = 0;
      time = 0;
    }
  }
  else if (state == 4) { // Found wall, turn around!
    speedLeft = 30;
    speedRight = -30;
    
    if (time > 300) {
      state = 0;
      time = 0;
    }
  }
    
  servoLeft.writeMicroseconds(1500 - speedLeft);
  servoRight.writeMicroseconds(1500 + speedRight);
  
  //Serial.print("\n");
  delay(10);
}

void updateSonars() {
  // Use micros in order to avoid delays that stall the whole state machine
  unsigned long currentTime = micros();
  
  if (currentTime - sonarLastReadTime >= sonarReadDelay) {
    if (sonarLastReadLower) {
      // Read upper
      sonarDistanceUpper = readSonar(sonarPinUpper);
    } else {
      // Read lower
      sonarDistanceLower = readSonar(sonarPinLower);
    }
    
    sonarLastReadLower = !sonarLastReadLower;
    sonarLastReadTime = currentTime;
  }
}

unsigned long readSonar(int pin) {
  // Start ranging
  digitalWrite(pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pin, LOW);
  
  // Wait for the result
  pinMode(pin, INPUT);
  unsigned long duration = pulseIn(pin, HIGH, (unsigned long)3000);//5772); // Read echo pulse
  //delay(sonarReadDelay); // Make sure that we wait to let the sound die out so that it doesn't fool any other sensor
  
  if (duration == 0) {
    duration = 3000;
  }
  
  unsigned long inches = duration / 148;
  unsigned long cm = inches * 2.54;
  
  //Serial.print(pin);
  //Serial.print(" cm: ");
  //Serial.print(cm);
  //Serial.print("\t");
  
  // Make sure that we reset the pin before leaving the function
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  
  return cm;
}

void checkForWall() {
  foundWall = false;
  
  if (sonarDistanceUpper < wallThreshold) {
    foundWall = true;
    foundWallAngle = sweepAngle;
  }
}

// Moves the scan servo and sweeps to find the puck. Only finds puck when
// sweeping to the right, in order to make it simple and robust.
void sweepForPuck() {
  bool seesPuck = (sonarDistanceLower + puckThreshold < sonarDistanceUpper) && (sonarDistanceLower < puckRange);
  
  if (!sawThisSweep) {
    if (seesPuck) {
      // Sweeping to the right
      foundPuckAngle = sweepAngle + puckSideToCenterAngleEstimate;
      foundPuckDistance = sonarDistanceLower;
      
      sawThisSweep = true;
      tone(2, 3000, 100);
    }
  }
  
  // Rotate sensor
  sweepAngle += sweepAngleDelta;
  
  // Reached end of angle limit?
  sweepJustOver = false;
  
  if (abs(sweepAngle) > sweepAngleLimit) {
    // Sweep ended
    foundPuck = sawThisSweep;
    
    // Sweeping right?
    if (sweepAngleDelta > 0) {
      sweepJustOver = true;
      sweepAngle = 0;
    }
    
    // Sweeping left?
    if (sweepAngleDelta < 0) {
      // Reset variables for next sweep to the right
      sawThisSweep = false;
      foundPuck = false;
      foundPuckAngle = 0;
    }
    
    // Switch scan direction
    sweepAngleDelta = -sweepAngleDelta;
  }
  
  int servoTiming = 1500 + ((long)(sweepAngleOffset + sweepAngle) * 820) / 90;
  servoScanner.writeMicroseconds(servoTiming);
  delay(sonarReadDelay);
}
