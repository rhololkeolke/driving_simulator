#define ESTOP_PIN 22
#define KEYSWITCH_PIN 42

bool keyswitch_on = false;

void setup() {
  Serial.begin(9600);
  pinMode(ESTOP_PIN, INPUT);
  pinMode(KEYSWITCH_PIN, INPUT);
  attachInterrupt(ESTOP_PIN, activateEstop, HIGH);
}

void loop() {
  Serial.print("Keyswitch: ");
  keyswitch_on = digitalRead(KEYSWITCH_PIN);
  Serial.println(keyswitch_on);
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
