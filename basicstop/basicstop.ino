/*
 * Robotics with the BOE Shield - PhototransistorVoltage
 * Display voltage of phototransistor circuit output connected to A3 in
 * the serial monitor.
 */
#include <Servo.h>

Servo servoLeft;
Servo servoRight;
int rightWhisker = 6;
int leftWhisker = 8;
// Whisker sensors
int whiskerNone = 0;
int whiskerLeft = 1;
int whiskerRight = 2;
int whiskerBoth = 3;

int speedLeft=0, speedRight=0;

void setup()                                 // Built-in initialization block
{
  Serial.begin(9600);                 // Set data rate to 9600 bps
  tone(4, 3000, 1000);
  delay(1000);
  servoLeft.attach(13);
  servoRight.attach(12);
  pinMode(rightWhisker, INPUT);
  pinMode(leftWhisker, INPUT);
}

void loop()                                  // Main loop auto-repeats
{
  
   delay(10);
  
  Serial.println(volts(A3));   
  if(volts(A3)<0.1){
    speedLeft=-20;
    speedRight=-20; 
  } 
  else if(volts(A3)>0.30)
  {
    speedLeft=-20;
    speedRight=20;
  }
  
  int collision = checkWhiskers();
  if (collision > 0)
  {
    speedLeft = 0;
    speedRight = 0;
    // Serial.print(collision);
  }
  
  // Delay for 1 second ><
  maneuver(speedLeft,speedRight,20);

  
  
}
                                             
float volts(int adPin)                       // Measures volts at adPin
{                                            // Returns floating point voltage
 return float(analogRead(adPin)) * 5.0 / 1024.0;
}

int checkWhiskers()
{
  byte wLeft = digitalRead(leftWhisker);
  byte wRight = digitalRead(rightWhisker);
  if (wLeft == 1 && wRight == 1)
  {
    return whiskerNone;
  }
  else if (wLeft == 0 && wRight == 1)
  {
    return whiskerLeft;
  }
  else if (wLeft == 1 && wRight == 0)
  {
    return whiskerRight;
  }
  else if (wLeft == 0 && wRight == 0)
  {
    return whiskerBoth;
  }
}

void maneuver(int speedLeft, int speedRight, int msTime)
{
  servoLeft.writeMicroseconds(1500 + speedLeft);   // Set left servo speed
  servoRight.writeMicroseconds(1500 - speedRight); // Set right servo speed
  if(msTime==-1)                                   // if msTime = -1
  {                                  
    servoLeft.detach();                            // Stop servo signals
    servoRight.detach();   
  }
  delay(msTime);                                   // Delay for msTime
}
