int pin = 9;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  if (digitalRead(pin) == 0) {
    tone(2, 3000, 10);
    delay(10);
  }
}
