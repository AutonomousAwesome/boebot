/*
 * Robotics with the BOE Shield - PhototransistorVoltage
 * Display voltage of phototransistor circuit output connected to A3 in
 * the serial monitor.
 */
#include <Servo.h>

Servo servoLeft;
Servo servoRight;

int state = 2; // 0 = search for puck, 1 = aproach puck, 2 = find beacon, 3 = aproach beacon, 4 = dump pucks

//Pins
//const int sonarSensorHigh = 9;
//const int sonarSensorLow = 8;
//const int sharpSensorLow = 4; // is 0
//const int sharpSensorHigh = 5; // is 1
const int beaconright = 4;
const int beaconleft = 5;
const int beaconrear = 3;

long lastTransitionTime = 0;

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

  int speedLeft = 0;
  int speedRight = 0;

  int beaconDir = beaconScan();
  Serial.print("beacon: ");
  Serial.println(beaconDir);

  if (beaconDir = 1){//forward
    tone(4, 3000, 1000);
  }else if(beaconDir = 2){//right
    tone(4, 3000, 1000);
  }else if(beaconDir = 3){//left
    tone(4, 3000, 1000);
  }else{ // not seen  
    //derp   
  }      


    //take action

    drive(speedLeft,speedRight);
}

int beaconScan(){
  int bRight = digitalRead(beaconright);
  int bLeft = digitalRead(beaconleft);
  int bRear = digitalRead(beaconrear); 
 if(bRight && bLeft){
   return 1;
 }else if (bRight){
   return 2;
 }else if (bLeft){
   return 3;
 }else
   return 0;
}

float volts(int adPin)                       // Measures volts at adPin
{ // Returns floating point voltage
    return float(analogRead(adPin)) * 5.0 / 1024.0;
}

void drive(int speedLeft, int speedRight){
    servoLeft.writeMicroseconds(1500 + speedLeft);   // Set left servo speed
    servoRight.writeMicroseconds(1500 - speedRight); // Set right servo speed
}

    
