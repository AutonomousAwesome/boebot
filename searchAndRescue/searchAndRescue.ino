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
    
    const int sonarSensor = 9;
    const int sharpSensorLow = 4;
    const int sharpSensorHigh = 5;
    const int beaconSensor = 7;
    
    const int noScan = 0;
    const int scanLeft = 1;
    const int scanRight = 2;
    int puckScanResults = 0;
    
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
    
    int scanForPuck(int lowSensorPin, int highSensorPin){
      int seesPuck            
      lowDist = 10*volts(lowSensorPin); //TODO better distance measuring
      highDist = 10*volts(highSensorPin);
      if((lowDist + threshold) > highDist){
        seesPuck = 1;
      }else{
        seesPuck = 0;
      }
        
      //rotate sensor
      return noScan //scanLeft, scanRight
    }
    
    float volts(int adPin)                       // Measures volts at adPin
    { // Returns floating point voltage
      return float(analogRead(adPin)) * 5.0 / 1024.0;
    }
    
    void drive(int speedLeft, int speedRight){
      servoLeft.writeMicroseconds(1500 + speedLeft);   // Set left servo speed
      servoRight.writeMicroseconds(1500 - speedRight); // Set right servo speed
    }
