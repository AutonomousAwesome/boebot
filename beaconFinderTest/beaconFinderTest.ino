/*
 * Robotics with the BOE Shield - PhototransistorVoltage
 * Display voltage of phototransistor circuit output connected to A3 in
 * the serial monitor.
 */
#include <Servo.h>

Servo servoLeft;
Servo servoRight;

const int beaconright = 4;
const int beaconleft = 5;
const int beaconrear = 3;


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
//returns if the beacon is seen and the relative angle in which the beacon is, 0 is forward, positive is left
void beaconScan(){
  int bRight = digitalRead(beaconright);
  int bLeft = digitalRead(beaconleft);
  int bRear = digitalRead(beaconrear); 
  if(!bRight && !bLeft && !bRear){ // sees nothing
    seesBeacon = false;
    beaconAngle = 0;
    return  
  }else if (bRight && !bLeft && !bRear){ // only right
    seesBeacon = true;
    beaconAngle = -120;
    return;
  }else if (!bRight && bLeft && !bRear){ //only left
    seesBeacon = true;
    beaconAngle = 120;
    return ;
  }else if (!bRight && !bLeft && bRear){ // only rear
    seesBeacon = true;
    beaconAngle = 180;
    return ;
  }else if (bRight && bLeft){ // left & right
    seesBeacon = true;
    beaconAngle = 0;
    return ;
  }else if (bRight && !bLeft && bRear){ // right & rear
    seesBeacon = true;
    beaconAngle = -150;
    return ;
  }else if (!bRight && bLeft && bRear){ // rear & left
    seesBeacon = true;
    beaconAngle = 150;
    return ;
}

void drive(int speedLeft, int speedRight){
    servoLeft.writeMicroseconds(1500 + speedLeft);   // Set left servo speed
    servoRight.writeMicroseconds(1500 - speedRight); // Set right servo speed
}

    
