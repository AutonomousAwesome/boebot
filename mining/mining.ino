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
      
      int rotationConstant = 1;
      
      long startTime = 0;
      long loopPeriod = 10000;
      int speedLeft=0, speedRight=0;
      int currentState = 0;
      long lastTransitionTime = 0;
      
      int stoppingSignal = 0;
      int forwardSignal = 80;
      int backingSignal = 80;
      int turningSignal = 30;
      
      int sensorFront = 2;
      int sensorLeft = 4;
      int sensorRight = 3;
      
      float thresholdBlack = 3.5;
      float thresholdWhite = 2.5;      
      
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
        
         while(startTime > micros()) ; // wait here until we get constant looptime
         startTime = micros() + loopPeriod;
        
        int collision = checkWhiskers();
        if (collision > 0)
        {
          changeState(4); //awoid wall collision process
          speedLeft = stoppingSignal;
          speedRight = stoppingSignal;
          // Serial.print(collision);
        }
         
        //mining process
          if(currentState == 0){
          speedLeft = forwardSignal;
          speedRight = forwardSignal;
         // minSensor = max(volts(A1),volts(A2),volts(A3));min
          if(checkMine(thresholdBlack)){
            changeState(1);
            speedLeft=stoppingSignal;
            speedRight=stoppingSignal; 
          }
        }
        else if(currentState == 1)//bomb signal state
        {
          tone(4, 3000, 200);
          delay(200);
          tone(4, 3500, 200);
          delay(200);
          tone(4, 3000, 200);
          delay(200);
          tone(4, 2500, 200);
          delay(200);
          tone(4, 3000, 200);
          delay(200);
          changeState(2);
          speedLeft=-backingSignal;
          speedRight=-backingSignal; 
        }
        else if(currentState == 2)// back state
        {
            speedLeft=-backingSignal;
            speedRight=-backingSignal;
            if(checkClear(thresholdWhite)){
              changeState(3);
                //rotationConstant = 2*random(0,2)-1;
                speedLeft=-rotationConstant * turningSignal;
                speedRight=rotationConstant * turningSignal;
            }
        }
        else if(currentState==3) // turning state
         {
           speedLeft=-rotationConstant * turningSignal;
           speedRight=rotationConstant * turningSignal;
           
           long turningTime = random(1500000,2500000);
           if (micros() - lastTransitionTime > turningTime) {
             changeState(0);
           }
           if(checkMine(thresholdBlack)){
            changeState(1);
            speedLeft=stoppingSignal;
            speedRight=stoppingSignal; 
          }
         }
        else if(currentState ==4)
        {
          speedLeft=-backingSignal;
          speedRight=-backingSignal;
          if (collision == whiskerNone){
            currentState = 5;
          }
          else if(collision == whiskerLeft)
          {
            rotationConstant = 1;
          }
          else if(collision == whiskerRight)
          {
            rotationConstant = -1;
          }  
          else if(collision == whiskerBoth)
          {
            rotationConstant = 2*random(0,2)-1;
          }
        }
        else if(currentState ==5)
        {
          speedLeft=-rotationConstant * turningSignal;
           speedRight=rotationConstant * turningSignal;
           
           long turningTime = 1000000;
           if (micros() - lastTransitionTime > turningTime) {
             changeState(0);
           }
        }
          
        Serial.print("Front: ");
        Serial.print(volts(sensorFront));
        Serial.print("  Left:");
        Serial.print(volts(sensorLeft));
        Serial.print("  Right:");
        Serial.println(volts(sensorRight));
        
        // Delay for 1 second ><
        maneuver(speedLeft,speedRight);
      
        
      }
      
      void changeState(int newState){
        currentState = newState;
        lastTransitionTime = micros();
      }
      
      boolean checkMine(float threshold){
        float maxVolt = 0;
        float voltFront = volts(sensorFront);
        float voltLeft = volts(sensorLeft);
        float voltRight = volts(sensorRight);
        
        //check which is greatest
        if(voltLeft>maxVolt){
          maxVolt = voltLeft;
          rotationConstant = -1;
        }
        if(voltRight>maxVolt){
          maxVolt = voltRight;
          rotationConstant = 1;
        }
        if(voltFront>maxVolt){
          maxVolt = voltFront;
          rotationConstant = 2*random(0,2)-1;;
        }
        
        if(maxVolt>threshold){
          return true;
        }
        else{
          return false;
        }
      }
      
       boolean checkClear(float threshold){
        float maxVolt = 0;
        float voltFront = volts(sensorFront);
        float voltLeft = volts(sensorLeft);
        float voltRight = volts(sensorRight);
        
        //check which is greatest
        if(voltLeft>maxVolt){
          maxVolt = voltLeft;
        }
        if(voltRight>maxVolt){
          maxVolt = voltRight;
        }
        if(voltFront>maxVolt){
          maxVolt = voltFront;
        }
        
        if(threshold>maxVolt){
          return true;
        }
        else{
          return false;
        }
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
      
      void maneuver(int speedLeft, int speedRight)
      {
        servoLeft.writeMicroseconds(1500 + speedLeft);   // Set left servo speed
        servoRight.writeMicroseconds(1500 - speedRight); // Set right servo speed                               // Delay for msTime
      }
