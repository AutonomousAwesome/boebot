#include <Servo.h>

Servo servoSweeper;
Servo servoLeft;
Servo servoRight;

int state = 0; // 0 = search for puck, 1 = aproach puck, 2 = find beacon, 3 = aproach beacon, 4 = dump pucks
int pucks = 0; // count of pucks in the hold

// Timing
long startTime = 0;
long lastTransitionTime = 0;
long loopPeriod = 1000; // period in micro seconds

// Pins
const int sonarSensorHigh = 9;
const int sonarSensorLow = 8;

//---------- Beacon finding -----------
int irReceiverPinRight = 9;
int irReceiverPinLeft = 5;
int irReceiverPinBack = 3;
int ir1, ir2, ir3 = 100;

long listeningTime = 200000;
long wanderingTime = 2000000;
long turnAroundTime = 500000;
long turningTime = 50000;
long driveToSafeZoneTime = 2000000;
long lastBeaconTransitionTime = 0;

boolean beaconSeenIr1 = false;
boolean beaconSeenIr2 = false;
boolean beaconSeenIr3 = false;
      
// Take action
int speedLeft = 0;
int speedRight = 0;

int forwardSignal = 100;
int turningSignal = 50;
int stoppingSignal = 0;

int beaconState = 0;

int sensorLeft = A5;
int sensorRight = A3;

float thresholdBlack = 3.0;//3.8;
float thresholdWhite = 2.0;//3.0; 
//-------------------------------------

//---------- Puck finding -----------
int puckState = 0;
unsigned long puckStateTransitionTime = 0;
unsigned long puckTimeSinceStateChange = 0;

const int sonarPinUpper = 6;
const int sonarPinLower = 7;
const int sonarReadDelay = 20; // ms

bool sonarLastReadLower = false;
unsigned long sonarLastReadTime = 0;
unsigned long sonarDistanceUpper = 0; // cm
unsigned long sonarDistanceLower = 0; // cm

const unsigned long puckRange = 40; // cm
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
//-----------------------------------

void setup() {
  Serial.begin(9600);
  tone(2, 3000, 1000);
  delay(1000);
  servoSweeper.attach(11);
  servoLeft.attach(13);
  servoRight.attach(12);
}

void loop() {
  while(startTime > micros()) ; // wait here until we get constant looptime
  startTime = micros() + loopPeriod;
  
  speedLeft = 0;
  speedRight = 0;
  
  // Read global sensors
  updateSonars();
  
  // Always look for wall
  checkForWall();
  
  //if (foundWall) {
  //  changeState(1);
  //}
  
  // Make decision(s)
  switch (state) {
    case 0: // Puck finding
    loopPeriod = 10000;
    
    puckTimeSinceStateChange = micros() - puckStateTransitionTime;
    
    if (puckState == 0) { // Sweep, has to do this occasionally
      sweepForPuck();
      
      if (sweepJustOver) {
        if (foundPuck) {
          Serial.print("Found puck at angle: ");
          Serial.println(foundPuckAngle);
          
          // Puck straight ahead?
          if (abs(foundPuckAngle) < sweepAngleCenter) {
            if (foundPuckDistance < 20) {
              // Eat puck!
              changePuckState(4);
            } else {
              // Nudge forward
              changePuckState(3);
            }
          } else {
            if (foundPuckDistance <= 8) {
              // Eat puck!
              changePuckState(4);
            } else {
              // Nudge rotate
              changePuckState(2);
            }
          }
        } else {
          // Found no puck, wander
          changePuckState(1);
        }
      }
    }
    else if (puckState == 1) { // Wander
      speedLeft = 0;
      speedRight = 0;
      
      if (puckTimeSinceStateChange > 2000000) {
        changePuckState(0);
      }
    }
    else if (puckState == 2) { // Nudge rotate towards a puck
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
      
      if (puckTimeSinceStateChange > 200000) {
        changePuckState(0);
      }
    }
    else if (puckState == 3) { // Nudge forward towards puck
      speedLeft = 30;
      speedRight = 30;
      
      if (puckTimeSinceStateChange > 1000000) {
        changePuckState(0);
      }
    }
    else if (puckState == 4) { // Go forward and eat puck!
      speedLeft = 30;
      speedRight = 30;
      
      if (puckTimeSinceStateChange > 3000000) {
        pucks++;
        
        if (pucks < 2) {
          changePuckState(0);
        } else {
          // Find beacon
          changePuckState(0);
          changeState(2);
          changeBeaconState(0);
        }
      }
    }
    
    break;
    case 1: // ---
    break;
    case 2: // find beacon
    
    if(checkSafeZone(thresholdBlack)){
      changeState(3);
      changeBeaconState(0);
      speedLeft=stoppingSignal;
      speedRight=stoppingSignal; 
    } else {
      // substates
      switch(beaconState) {
        
        case 0:
        //Listening state        
        ir1 = digitalRead(irReceiverPinLeft);
        ir2 = digitalRead(irReceiverPinRight);
        ir3 = digitalRead(irReceiverPinBack);
        
        beaconSeenIr1 = beaconSeenIr1 || (ir1==0);
        beaconSeenIr2 = beaconSeenIr2 || (ir2==0);
        beaconSeenIr3 = beaconSeenIr3 || (ir3==0);
        
        if (micros() - lastBeaconTransitionTime > listeningTime) {
          changeBeaconState(1);      
        }
        break;
        case 1:
        // check sensors
        if(!beaconSeenIr1 && !beaconSeenIr2 && !beaconSeenIr3){
          //Didn't see anything, go to wandering state
          changeBeaconState(2);
        }
        else if(!beaconSeenIr1 && !beaconSeenIr2){
          //Saw something back, turn around
          beaconSeenIr3==false;
          changeBeaconState(3);
        }
        else {
          //Saw something front, left or right
          if(beaconSeenIr1 && beaconSeenIr2){
            //Saw something front, drive straight ahead
            speedLeft = forwardSignal;
            speedRight = forwardSignal;
          }
          else if(beaconSeenIr1){
            //Saw something left but not right, turn left
            speedLeft = -turningSignal;
            speedRight = turningSignal;
          }
          else if(beaconSeenIr2){
            //Saw something right but not left, turn right
            speedLeft = turningSignal;
            speedRight = -turningSignal;
          }
          
          //reset beaconSeen
          beaconSeenIr1 = false;
          beaconSeenIr2 = false;
          beaconSeenIr3 = false;
          
          changeBeaconState(0);
        }
        break;
        case 2:
        //Wandering state
        //TODO: some wandering
        speedLeft = 0;//-turningSignal;
        speedRight = 0;//turningSignal;
        
        if (micros() - lastBeaconTransitionTime > wanderingTime) {
          changeBeaconState(0);      
        }
        break;
        case 3:
        //Turnaround state
        speedLeft = -turningSignal;
        speedRight = turningSignal;
        
        if (micros() - lastBeaconTransitionTime > turnAroundTime) {
          changeBeaconState(0);      
        }
        break;
      }
    }
    break;
    case 3: // drive into the safe zone
    if (micros() - lastTransitionTime > driveToSafeZoneTime) {
      changeState(4);      
    }
    
    speedLeft = forwardSignal;
    speedRight = forwardSignal;
    break;
    case 4: // dump pucks
    if(checkClear(thresholdWhite)){
      changeState(5);
      speedLeft=stoppingSignal;
      speedRight=stoppingSignal; 
    }
    speedLeft = -forwardSignal;
    speedRight = -forwardSignal;
    break;
    
    case 5: //Turn around
    speedLeft = -turningSignal;
    speedRight = turningSignal;
    if (micros() - lastTransitionTime > turnAroundTime) {
      pucks = 0;
      changeBeaconState(0);      
    }
    break;
  }
  
  // Drive the robot!
  drive(speedLeft,speedRight);
}

float volts(int adPin) { // Returns floating point voltage
    return float(analogRead(adPin)) * 5.0 / 1024.0;
}

void drive(int speedLeft, int speedRight){
    servoLeft.writeMicroseconds(1500 - speedLeft);   // Set left servo speed
    servoRight.writeMicroseconds(1500 + speedRight); // Set right servo speed
}

void changeState(int newState){
    state = newState;
    lastTransitionTime = micros();
}

//---- Beacon functions ------
void changeBeaconState(int newState){
    beaconState = newState;
    lastBeaconTransitionTime = micros();
}

boolean checkSafeZone(float threshold){
  float voltLeft = volts(sensorLeft);
  float voltRight = volts(sensorRight);
  
  if(voltRight>threshold && voltLeft>threshold){
    return true;
  }
  else{
    return false;
  }
}

boolean checkClear(float threshold){
  float maxVolt = 0;
  float voltLeft = volts(sensorLeft);
  float voltRight = volts(sensorRight);
  
  //check which is greatest
  if(voltLeft>maxVolt){
    maxVolt = voltLeft;
  }
  if(voltRight>maxVolt){
    maxVolt = voltRight;
  }
          
  if(threshold>maxVolt){
    return true;
  }
  else{
    return false;
  }
}
//------------------------------

// ---------- Puck finding functions -------------
void changePuckState(int newState) {
  //if (puckState != newState) {
    puckState = newState;
    puckStateTransitionTime = micros();
  //}
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
      Serial.print("\n");
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
  
  Serial.print(pin);
  Serial.print(" cm: ");
  Serial.print(cm);
  Serial.print("\t");
  
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
  servoSweeper.writeMicroseconds(servoTiming);
  delay(sonarReadDelay);
}
// -----------------------------------------------
