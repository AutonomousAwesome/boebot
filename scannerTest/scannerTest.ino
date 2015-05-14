#include <Servo.h>
    
Servo servoScanner;
Servo servoLeft;
Servo servoRight;

const int pingPinHigh = 6;
const int pingPinLow = 7;

// Scan outputs
boolean foundPuck = false;
int foundPuckAngle = 0;
boolean foundWall = false;
int foundWallAngle = 0;
// TODO:
//int foundPuckDist = 0;
//int foundWallDist = 0;

//Scan internal variables
boolean seesPuck = false; 
int leftScanEdge = 0;
int rightScanEdge = 0;
boolean scanDirection = false; // true is right, false is left
int scanServoAngle = 0; // higher is more left
int dScanAngle = 25; // going left
int scanRange = 600;

void setup() {
  Serial.begin(9600);
  tone(2, 3000, 1000);
  servoScanner.attach(11);
  servoLeft.attach(13);
  servoRight.attach(12);
  servoLeft.writeMicroseconds(1500);   // Set left servo speed
  servoRight.writeMicroseconds(1500); // Set right servo speed
}

void loop() {
  scanForPuck(pingPinLow, pingPinHigh);
  //delay(25);
  //unsigned int cmHigh = checkSonar(pingPinHigh);
  //unsigned int cmLow = checkSonar(pingPinLow);
  //Serial.print("   foundPuck: ");
 //Serial.print(foundPuck);
//  Serial.print("   foundPuckAngle: ");
//  Serial.print(foundPuckAngle);
//  Serial.print("   seesPuck: ");
//  Serial.print(seesPuck);
//  Serial.print("   leftScanEdge: ");
//  Serial.print(leftScanEdge);
//  Serial.print("   rightScanEdge: ");
//  Serial.print(rightScanEdge);
//  Serial.print("   scanDirection: ");
//  Serial.print(scanDirection);
 //Serial.print("   scanServoAngle: ");
 // Serial.println(scanServoAngle);
 
 Serial.print("\n");
  
  //delay(100);		             // Short delay
}

unsigned long checkSonar(int pingPin) {
  digitalWrite(pingPin, HIGH);       // Start ranging
  delayMicroseconds(5);              // with 5 microsecond burst
  digitalWrite(pingPin, LOW);        // End ranging
  
  pinMode(pingPin, INPUT);
  unsigned long duration = pulseIn(pingPin, HIGH, (unsigned long)3000);//5772); // Read echo pulse
  unsigned long cm;
  
  if (duration == 0) {
    duration = 3000;
  }
  
  unsigned long inches = duration / 148;
  cm = inches * 2.54;
  
  
  pinMode(pingPin, OUTPUT); // Set pin to OUTPUT
  digitalWrite(pingPin, LOW); // Ensure pin is low
  
  Serial.print(pingPin);
  Serial.print(" cm: ");
  Serial.print(cm);
  Serial.print("\t");
  
  return cm;
}

/* Will scan for pucks and walls. 
* this function is communicating it's result by setting some global variables:
* boolean foundPuck, high = is found
* int foundPuckDist, the distance to the Puck in cm
* int foundPuckAngle, the angle in degree where the puck is, from -90 to + 90 deg.
* boolean foundWall, if we see any wall.
* int foundWallAngle, the angle to the closest found wall, from -90 to + 90 deg.
* int foundWallDist, the distance to the closest found wall in cm.
*/
void scanForPuck(int lowSensorPin, int highSensorPin){
  boolean sawPuck = seesPuck;   
  unsigned long lowDist = checkSonar(lowSensorPin);
  delay(50);
  unsigned long highDist = checkSonar(highSensorPin);
  delay(50);
  int threshold = 10;
  
  seesPuck = (((lowDist + threshold) < highDist) && (lowDist < 60));// do we see a puck?

  if(!foundPuck){
    if(seesPuck){//just found the puck(for the "first time")
      leftScanEdge = scanServoAngle;
      rightScanEdge = scanServoAngle;
      foundPuck= true;
    }
    if(scanServoAngle >= scanRange){ //too much left
      scanDirection= !scanDirection;
      dScanAngle= -abs(dScanAngle);
    }
    if(scanServoAngle <= -scanRange){ // too much right
      scanDirection= !scanDirection;
      dScanAngle= abs(dScanAngle);
    }
  }else{ // foundPuck
    if(seesPuck){ 
      if(sawPuck){ // foundPuck & see & saw
       if(scanServoAngle > leftScanEdge){
         leftScanEdge = scanServoAngle;
       }
       if(scanServoAngle < rightScanEdge){
         rightScanEdge = scanServoAngle;
       }
      }else{ // foundPuck & see & NOT saw
        if(scanDirection){ //we are going right
          leftScanEdge = scanServoAngle;
        }else{
          rightScanEdge = scanServoAngle;            
        }
      }  
    }else{ //don't see the puck
      if(sawPuck){// foundPuck & NOT see & saw
        scanDirection= !scanDirection;
        dScanAngle= -dScanAngle; 
      }else{// foundPuck & NOT see & NOT saw
        if(abs(scanServoAngle) >= scanRange){
          foundPuck = false;
            scanDirection= !scanDirection;
            dScanAngle= -dScanAngle;
        }
      }
      
    }
    
    if(abs(scanServoAngle) >= scanRange){
      scanDirection= !scanDirection;
      dScanAngle= -dScanAngle;
    }
  }
  
  
  foundPuckAngle =  (leftScanEdge + rightScanEdge)/2;
    
  //rotate sensor
  scanServoAngle += dScanAngle;
  servoScanner.writeMicroseconds(1500 + scanServoAngle); // Set right servo speed
}
