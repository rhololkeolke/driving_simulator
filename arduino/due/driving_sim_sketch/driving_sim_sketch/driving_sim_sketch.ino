#define ESTOP_PIN 22

void setup() {
  Serial.begin(9600);
  pinMode(ESTOP_PIN, INPUT);
  attachInterrupt(ESTOP_PIN, activateEstop, HIGH);
}

void loop() {
  Serial.println("running");
  delay(500);
}

void activateEstop() {
  bool estop_active = true;
  do {
    Serial.println("Estop");
    estop_active = digitalRead(ESTOP_PIN);
    delayMicroseconds(500000);
  } while(estop_active); 
}
