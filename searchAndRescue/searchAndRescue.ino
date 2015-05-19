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

//----------Variables for beacon finding-----------

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

float thresholdBlack = 3.8;
float thresholdWhite = 3.0; 

//-------------------------------------------------
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
  
  // Make decision(s)
  switch (state) {
    case 0: // search for pucks
    break;
    case 1:
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
        
        if (micros() - lastTransitionTime > listeningTime) {
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
        
        if (micros() - lastTransitionTime > wanderingTime) {
          changeBeaconState(0);      
        }
        break;
        case 3:
        //Turnaround state
        speedLeft = -turningSignal;
        speedRight = turningSignal;
        
        if (micros() - lastTransitionTime > turnAroundTime) {
          changeBeaconState(0);      
        }
        break;
      }
    }
    break;
    case 3: // drive into the safe zone
    if (micros() - lastTransitionTime > driveToSafeZoneTime) {
      changeBeaconState(4);      
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
