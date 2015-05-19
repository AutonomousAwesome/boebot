/*
 * Robotics with the BOE Shield - PhototransistorVoltage
 * Display voltage of phototransistor circuit output connected to A3 in
 * the serial monitor.
 */
#include <Servo.h>

Servo servoLeft;
Servo servoRight;

int wanderState = 0; // 0 = forward, 1 = right turn, 2 = left turn

const int turnProbability = 10;
const int rightTurnProbability = 500;
const int stopTurnProbability = 20;

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
  
  //make decision(s)
  switch (wanderState) {
    case 0: // forward
      //change wanderState to turning?
      if(random(1000) < turnProbability){
        if(random(1000) < rightTurnProbability){
          wanderState = 1;
        }else{
          wanderState = 2;
        }
      }
      break;
    case 1: // right turn
      
      //stop turning?
      if(random(1000) < stopTurnProbability){
        wanderState = 0;
      }
      break;
    case 2: // left turn
      //stop turning?
      if(random(1000) < stopTurnProbability){
        wanderState = 0;
      }
        break;
    case 3: // stop & beep
        break;
  }


  //take action
  int speedLeft = 0;
  int speedRight = 0;
wanderState = 1;
      switch (wanderState) {
        case 0: // forward
          speedLeft = 200;
          speedRight = 200;
          break;
        case 1: // right turn
          speedLeft = 100;
          speedRight = -100;
          break;
        case 2: // left turn
          speedLeft = -100;
          speedRight = 100;
          break;
      }

      drive(speedLeft,speedRight);
}

void drive(int speedLeft, int speedRight){
    servoLeft.writeMicroseconds(1500 - speedLeft);   // Set left servo speed
    servoRight.writeMicroseconds(1500 + speedRight); // Set right servo speed
}
