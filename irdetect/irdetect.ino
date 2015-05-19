#include <Servo.h>
Servo servoLeft;
Servo servoRight;

int irReceiverPinRight = 9;
int irReceiverPinLeft = 5;
int irReceiverPinBack = 3;
int ir1, ir2, ir3 = 100;

long lastTransitionTime = 0;
long listeningTime = 200000;
long wanderingTime = 7000000;
long turnAroundTime = 500000;
long turningTime = 50000;
long startTime = 0;
long loopPeriod = 1000;

boolean beaconSeenIr1 = false;
boolean beaconSeenIr2 = false;
boolean beaconSeenIr3 = false;
      
//take action
int speedLeft = 0;
int speedRight = 0;

int forwardSignal = -100;
int turningSignal = 50;

int state = 0;

void setup()
{
  Serial.begin(9600);
  tone(4, 3000, 1000);
  servoLeft.attach(13);
  servoRight.attach(12);
}

void loop()
{
  while(startTime > micros()) ; // wait here until we get constant looptime
  startTime = micros() + loopPeriod;
  
  /*
  Serial.print(ir1);
  Serial.print("\t");
  Serial.print(ir2);
  Serial.print("\t");
  Serial.println(ir3);*/
  // States
  switch(state) {
    
    case 0:
      //Listening state

        ir1 = digitalRead(irReceiverPinLeft);
        ir2 = digitalRead(irReceiverPinRight);
        ir3 = digitalRead(irReceiverPinBack);
        
        beaconSeenIr1 = beaconSeenIr1 || (ir1==0);
        beaconSeenIr2 = beaconSeenIr2 || (ir2==0);
        beaconSeenIr3 = beaconSeenIr3 || (ir3==0);
        
        if (micros() - lastTransitionTime > listeningTime) {
          changeState(1);      
        }
        break;
     case 1:
     // check sensors
     if(!beaconSeenIr1 && !beaconSeenIr2 && !beaconSeenIr3){
       //Didn't see anything, go to wandering state
       changeState(2);
     }
     else if(!beaconSeenIr1 && !beaconSeenIr2){
       //Saw something back, turn around
       beaconSeenIr3==false;
       changeState(3);
     }
     else{
       //Saw something front, left or right
       
       if(beaconSeenIr1 && beaconSeenIr2){
       //Saw something front, drive straight ahead
       speedLeft = forwardSignal;
       speedRight = forwardSignal;
       }
       else if(beaconSeenIr1){
       //Saw something left but not right, turn left
       speedLeft = turningSignal;
       speedRight = -turningSignal;
       }
       else if(beaconSeenIr2){
       //Saw something right but not left, turn right
       speedLeft = -turningSignal;
       speedRight = turningSignal;
       }
       
       //reset beaconSeen
       beaconSeenIr1 = false;
       beaconSeenIr2 = false;
       beaconSeenIr3 = false;
       
       changeState(0);
     }
     break;
     
     case 2:
     //Wandering state
     //TODO: some wandering
     speedLeft = 0;//-turningSignal;
     speedRight = 0;//turningSignal;
     if (micros() - lastTransitionTime > wanderingTime) {
          changeState(0);      
     }
     
     break;
     
     case 3:
     //Turnaround state
       speedLeft = turningSignal;
       speedRight = -turningSignal;
       if (micros() - lastTransitionTime > turnAroundTime) {
          changeState(0);      
       }
     break;
  }
  
  drive(speedLeft,speedRight);
}

void changeState(int newState){
        state = newState;
        lastTransitionTime = micros();
}

float volts(int adPin)                       // Measures volts at adPin
{ // Returns floating point voltage
    return float(analogRead(adPin)) * 5.0 / 1024.0;
}

void drive(int speedLeft, int speedRight){
    servoLeft.writeMicroseconds(1500 + speedLeft);   // Set left servo speed
    servoRight.writeMicroseconds(1500 - speedRight); // Set right servo speed
}
