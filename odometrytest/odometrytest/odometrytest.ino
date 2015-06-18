/*
 * Robotics with the BOE Shield - PhototransistorVoltage
 * Display voltage of phototransistor circuit output connected to A3 in
 * the serial monitor.
 */
#include <Servo.h>

Servo servoLeft;
Servo servoRight;
//odometry
float heading = 0;
float posX = 0;
float posY = 0;

int state = 2;

long startTime = 0;
long loopPeriod = 10000; // in microseconds
float loopPeriod_s = loopPeriod / 1000000;

int speedLeft = 0;
int speedRight = 0;

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
  odometry(speedRight, speedLeft);
   
  Serial.println(heading);
   
  switch (state) {
    case 2: // left turn
      speedLeft = -30;
      speedRight = 30;
      if (heading > 180){
        state = 3;
        speedLeft = 0;
        speedRight = 0;
        tone(4, 3000, 1000);
      }
      break;
    case 3: // stop
      speedLeft = 0;
      speedRight = 0;
      break;
  }
  driveToHeading(goalHeading);
}
    
void odometry(int speedRight, int speedLeft){
  heading += (speedRight - speedLeft)*(loopPeriod/(float)1000000)*(0.82);
  
  while( heading < 0){
    heading +=360;
  }
  while( heading >360){
    heading -=360;
  }
  
  int speed = 0.1*(speedLeft + speedRight)/2;
  posX += cos(heading*3.14/180)*speed*loopPeriod_s;
  posY += sin(heading*3.14/180)*speed*loopPeriod_s;
}

void driveToHeading(int goalHeading){
  int headingDiff = goalHeading - heading;
    while( headingDiff < -180){
    headingDiff +=360;
  }
  while( headingDiff > 180){
    headingDiff -=360;
  }
  
  //headingDiff in range -180 to 180
  if(headingDiff > 30){ // left turn
    speedLeft = -30;
    speedRight = 30;
    return;
  }
  if(headingDiff < -30){ //right turn
    speedLeft = 30;
    speedRight = -30;
    return;
  }
  
  speedLeft = 30  - headingDiff*3;
  speedRight = 30 + headingDiff*3;
}
    
void drive(){
  servoLeft.writeMicroseconds(1500 - speedLeft);   // Set left servo speed
  servoRight.writeMicroseconds(1500 + speedRight); // Set right servo speed
}
