int irReceiverPin1 = 4;
int irReceiverPin2 = 4;
int irReceiverPin3 = 4;
int ir1, ir, ir3;

void setup()
{
  Serial.begin(9600);
  tone(4, 3000, 1000);
}

void loop()
{
  
  delay(100);
  ir1 = analogRead(irReceiverPin1);
  ir2 = analogRead(irReceiverPin1);
  ir3 = analogRead(irReceiverPin1);
  Serial.print(ir1);
  Serial.print("\t");
  Serial.print(ir2);
  Serial.print("\t");
  Serial.println(ir3);
  // States
  switch(state) {
    
    case 0:
      //Listening state
      if timeInState 
    
  }

}
