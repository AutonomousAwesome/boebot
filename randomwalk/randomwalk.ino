/*
 * Robotics with the BOE Shield - PhototransistorVoltage
 * Display voltage of phototransistor circuit output connected to A3 in
 * the serial monitor.
 */
#include <Servo.h>

Servo servoLeft;
Servo servoRight;

int state = 0; // 0 = forward, 1 = right turn, 2 = left turn, 3 = stop & beep, 4 = reverse

const int turnProbability = 10;
const int rightTurnProbability = 500;
const int stopTurnProbability = 20;

const int loopFrequency = 100;

void setup()                                 // Built-in initialization block
{
  Serial.begin(9600);                 // Set data rate to 9600 bps
  tone(4, 3000, 1000);
  delay(1000);
  servoLeft.attach(13);
  servoRight.attach(12);
}

void loop() { // Main loop auto-repeats
  boolean leftWisker = false;
  boolean rightWisker = false;
  boolean foundMine = false;

  //TODO read wiskers




  //make decision(s)
  switch (state) {
    case 0: // forward
      if (leftWisker) {
        state = 1;
      }
      if(rightWisker){
        state = 2;
      }
      if(foundMine){
        state = 3;
      }
      
      //change state to turning?
      if(random(1000) < turnProbability){
        if(random(1000) < rightTurnProbability){
          state = 1;
        }else{
          state = 2;
        }
      }
      break;
    case 1: // right turn
      
      //stop turning?
      if(random(1000) < stopTurnProbability){
        state = 0;
      }
      break;
    case 2: // left turn
      //stop turning?
      if(random(1000) < stopTurnProbability){
        state = 0;
      }
        break;
    case 3: // stop & beep
        break;
  }


      //take action
      int speedLeft = 0;
      int speedRight = 0;

      switch (state) {
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
        case 3: // stop & beep
          speedLeft = 0;
          speedRight = 0;
          break;
      }

      delay(1000/loopFrequency); 
      //  if(volts(A3)<0.2){
      //    speedLeft=0;
      //  speedRight=0;
      //  }
      //  else{
      //    speedLeft=200;
      //    speedRight=200;
      //  }
      //
      //
      //  // Delay for 1 second
      maneuver(speedLeft,speedRight,20);
    }

      float volts(int adPin)                       // Measures volts at adPin
      { // Returns floating point voltage
        return float(analogRead(adPin)) * 5.0 / 1024.0;
      }

      void maneuver(int speedLeft, int speedRight, int msTime)
      {
        servoLeft.writeMicroseconds(1500 + speedLeft);   // Set left servo speed
        servoRight.writeMicroseconds(1500 - speedRight); // Set right servo speed
        if (msTime == -1)                                // if msTime = -1
        {
          servoLeft.detach();                            // Stop servo signals
          servoRight.detach();
        }
        delay(msTime);                                   // Delay for msTime
      }
