/*
 * Robotics with the BOE Shield - PhototransistorVoltage
 * Display voltage of phototransistor circuit output connected to A3 in
 * the serial monitor.
 */
#include <Servo.h>

Servo servoLeft;
Servo servoRight;

float heading = 0;

float posX = 0;
float posY = 0;
int state = 2;

long startTime = 0;
long loopPeriod = 10000; // in microseconds


void setup()                                 // Built-in initialization block
{
  Serial.begin(9600);                 // Set data rate to 9600 bps
  
//  delay(1000);
  servoLeft.attach(13);
  servoRight.attach(12);
}

void loop() { // Main loop auto-repeats

   while(startTime > micros()) ; // wait here until we get constant looptime
   startTime = micros() + loopPeriod;

Serial.println(heading);



  //make decision(s)
  switch (state) {
    case 2: // left turn
    if (heading > 360){
      state = 3;
      tone(4, 3000, 1000);
      }
    
        break;
    case 3: // stop
        break;
  }


      //take action
      int speedLeft = 0;
      int speedRight = 0;

      switch (state) {
        case 0: // forward
          speedLeft = 100;
          speedRight = 100;
          break;
        case 1: // right turn
          speedLeft = 30;
          speedRight = -30;
          break;
        case 2: // left turn
          speedLeft = -30;
          speedRight = 30;
          break;
        case 3: // stop & beep
          speedLeft = 0;
          speedRight = 0;
          break;
      }

      delay(loopPeriod); 
      
      odometry(speedRight, speedLeft, loopPeriod);
      maneuver(speedLeft,speedRight);
    }
    
void odometry(int speedRight, int speedLeft, long loopPeriod){
  heading += loopPeriod + (speedLeft - speedRight)*loopPeriod/(float)15000;
  //if( heading < 0) heading +=360;
  //if( heading >360) heading -=360;
  
  int speed = (speedLeft + speedRight)/2;
  posX += cos(heading)*speed*loopPeriod/(float)1000000;
  posY += sin(heading)*speed*loopPeriod/(float)1000000;
}

      void maneuver(int speedLeft, int speedRight)
      {
        servoLeft.writeMicroseconds(1500 + speedLeft);   // Set left servo speed
        servoRight.writeMicroseconds(1500 - speedRight); // Set right servo speed
        //if (msTime == -1)                                // if msTime = -1
        //{
        //  servoLeft.detach();                            // Stop servo signals
        //  servoRight.detach();
        //}
      
      }
