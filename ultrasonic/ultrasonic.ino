const int pingPinHigh = 6;
const int pingPinLow = 7;
unsigned int duration, inches, cm, cmHigh, cmLow;

void setup() {
  Serial.begin(9600);
  tone(4, 3000, 1000);
}

void loop() {
  cmHigh = checkSonar(pingPinHigh);
  delay(100);
  cmLow = checkSonar(pingPinLow);
  Serial.print("High: ");
  Serial.print(cmHigh);
  Serial.print("\t");
  Serial.print("Low: ");
  Serial.println(cmLow);
  delay(200);		             // Short delay
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
