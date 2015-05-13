    /*
     * Robotics with the BOE Shield - PhototransistorVoltage
     * Display voltage of phototransistor circuit output connected to A3 in
     * the serial monitor.
     */
    #include <Servo.h>
    
    Servo servoLeft;
    Servo servoRight;
    
    int state = 0; // 0 = search for puck, 1 = aproach puck, 2 = find beacon, 3 = aproach beacon, 4 = dump pucks
    int pucks = 0; // count of pucks in the hold
    //const int turnProbability = 10;
    //const int rightTurnProbability = 500;
    //const int stopTurnProbability = 20;
    
    //Pins
    const int sonarSensor = 9;
    const int sharpSensorLow = 4; // is 0
    const int sharpSensorHigh = 5; // is 1
    const int beaconSensor = 7;
    
    const int noScan = 0;
    const int scanLeft = 1;
    const int scanRight = 2;
    int puckScanResults = 0;
    
    //Scan outputs
    boolean foundPuck = false;
    int foundPuckDist = 0;
    int foundPuckAngle = 0;
    boolean foundWall = false;
    int foundWallAngle = 0;
    int foundWallDist = 0;
    
    //Scan internal variables
    boolean seesPuck = false 
    int leftScanEdge = 0;
    int rightScanEdge = 0;
    boolean scanDirection = false; // true is right, false is left
    int scanServoAngle = 0;
    int dScanAngle = 1;
    
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
    
      //make decision(s)
      switch (state) {
        case 0: // search for pucks
          puckScanResults = scanForPuck(sharpSensorLow, sharpSensorHigh); //scanForPuck returns no, left or right
          if(puckScanResults > noScan){
            //found something, switch state
            state = 1;
          }
        break;
        case 1: // approach puck
        
        break;
        case 2: // find beacon
    
        break;
        case 3: // approach beacon
        break;
      }
    
    
      //take action
      int speedLeft = 0;
      int speedRight = 0;
    
      switch (state) {
        case 0: // forward
              //turn randomly and drive a bit
        break;
        case 1: // right turn
            
        break;
        case 2: // left turn
     
        break;
        case 3: // stop & beep
       
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
      drive(speedLeft,speedRight);
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
      int lowDist = checkSonar(lowSensorPin);  //10*volts(lowSensorPin); //TODO better distance measuring
      int highDist = checkSonar(highSensorPin);  //10*volts(highSensorPin);
      int threshold = 5;
      
      if(lowDist > (highDist + threshold)){ // do we see a puck?
        seesPuck = true;
      }
      if(!foundPuck){
        if(seesPuck){//just found the puck(for the "first time")
          leftScanEdge = scanServoAngle;
          rightScanEdge = scanServoAngle;
          foundPuck= true;
          if(abs(scanServoAngle) >= scanRange){
           changeScanDirection(); 
          scanServoAngle += dScanAngle;
        }else{
          scanServoAngle += dScanAngle;
        }
      }
      if(seesPuck && !sawPuck){ 

      }
      
      if(!seesPuck && foundPuck)
        
      //rotate sensor
      return noScan //scanLeft, scanRight
    }
    
    //helper function for scanForPuck
    void changeScanDirection(){
      if(foundPuck && !seesPuck){
        foundPuck = !foundPuck; //we have reached the edge of the scan and lost track of the puck
      }
      
      scanDirection= !scanDirection;
      dScanAngle= -dScanAngle;
    }
      
    float volts(int adPin)                       // Measures volts at adPin
    { // Returns floating point voltage
      return float(analogRead(adPin)) * 5.0 / 1024.0;
    }
    
    void drive(int speedLeft, int speedRight){
      servoLeft.writeMicroseconds(1500 + speedLeft);   // Set left servo speed
      servoRight.writeMicroseconds(1500 - speedRight); // Set right servo speed
    }

unsigned int checkSonar(int pingPin){
  pinMode(pingPin, OUTPUT);          // Set pin to OUTPUT
  digitalWrite(pingPin, LOW);        // Ensure pin is low
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);       // Start ranging
  delayMicroseconds(5);              //   with 5 microsecond burst
  digitalWrite(pingPin, LOW);        // End ranging
  pinMode(pingPin, INPUT);           // Set pin to INPUT
  duration = pulseIn(pingPin, HIGH); // Read echo pulse
  inches = duration / 74 / 2 ;        // Convert to inches
  cm = inches * 2.54;                    //Convert to centimeters
  return cm;
}
