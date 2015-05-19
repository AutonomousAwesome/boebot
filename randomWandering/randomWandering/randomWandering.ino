/*
 * Robotics with the BOE Shield - PhototransistorVoltage
 * Display voltage of phototransistor circuit output connected to A3 in
 * the serial monitor.
 */
#include <Servo.h>

Servo servoLeft;
Servo servoRight;

int wanderState = 0; // 0 = forward, 1 = right turn, 2 = left turn

const int turnProbability = 5;
const int rightTurnProbability = 500;
const int stopTurnProbability = 40;

int speedLeft = 0;
int speedRight = 0;

long startTime = 0;
long loopPeriod = 10000; // period in micro seconds

void setup()                                 // Built-in initialization block
{
  Serial.begin(9600);                 // Set data rate to 9600 bps
  tone(4, 3000, 1000);
  delay(1000);
  servoLeft.attach(13);
  servoRight.attach(12);
}

void loop() { // Main loop auto-repeats
  while(startTime > micros()) ; // wait here until we get constant looptime
  startTime = micros() + loopPeriod;
  wander(); 
  drive(speedLeft,speedRight);
}

//will set speed left and speed right, and set the wander state
void wander(){
  switch (wanderState) {
    case 0: // forward
      speedLeft = 200;
      speedRight = 200;
      if(random(1000) < turnProbability){//change wanderState to turning?
        if(random(1000) < rightTurnProbability){
          wanderState = 1;
          speedLeft = 100;
          speedRight = -100;
        }else{
          wanderState = 2;
          speedLeft = -100;
          speedRight = 100;
        }
      }
      break;
    case 1: // right turn
      speedLeft = 100;
      speedRight = -100;
      //stop turning?
      if(random(1000) < stopTurnProbability){
        wanderState = 0;
        speedLeft = 200;
        speedRight = 200;
      }
      break;
    case 2: // left turn
      speedLeft = -100;
      speedRight = 100;
      //stop turning?
      if(random(1000) < stopTurnProbability){
        wanderState = 0;
        speedLeft = 200;
        speedRight = 200;
      }
      break;
  }
}


void drive(int speedLeft, int speedRight){
    servoLeft.writeMicroseconds(1500 - speedLeft);   // Set left servo speed
    servoRight.writeMicroseconds(1500 + speedRight); // Set right servo speed
}
