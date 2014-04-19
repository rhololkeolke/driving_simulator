#define ESTOP_PIN 22
#define KEYSWITCH_PIN 42

#define DRIVE2_PIN 51
#define DRIVE_PIN 49
#define NEUTRAL_PIN 47
#define REVERSE_PIN 45
#define PARK_PIN 43

#define DRIVE2_GEAR 4
#define DRIVE_GEAR 3
#define NEUTRAL_GEAR 2
#define REVERSE_GEAR 1
#define PARK_GEAR 0
#define UNKNOWN_GEAR -1

bool keyswitch_on = false;
int gearshift_state = UNKNOWN_GEAR;

void setup() {
  Serial.begin(9600);
  
  // estop setup
  pinMode(ESTOP_PIN, INPUT);
  attachInterrupt(ESTOP_PIN, activateEstop, HIGH);

  // keyswitch setup
  pinMode(KEYSWITCH_PIN, INPUT);
  keyswitch_on = digitalRead(KEYSWITCH_PIN);
  
  // Gear shift setup
  pinMode(DRIVE2_PIN, INPUT);
  pinMode(DRIVE_PIN, INPUT);
  pinMode(NEUTRAL_PIN, INPUT);
  pinMode(REVERSE_PIN, INPUT);
  pinMode(PARK_PIN, INPUT);
  
  gearshift_state = getGear(gearshift_state);
}

void loop() {
  Serial.print("Keyswitch: ");
  keyswitch_on = digitalRead(KEYSWITCH_PIN);
  Serial.println(keyswitch_on);
  Serial.print("Gear: ");
  gearshift_state = getGear(gearshift_state);
  switch(gearshift_state) {
    case UNKNOWN_GEAR:
      Serial.println("Unknown");
      break;
    case PARK_GEAR:
      Serial.println("Park");
      break;
    case REVERSE_GEAR:
      Serial.println("Reverse");
      break;
    case NEUTRAL_GEAR:
      Serial.println("Neutral");
      break;
    case DRIVE_GEAR:
      Serial.println("Drive");
      break;
    case DRIVE2_GEAR:
      Serial.println("Drive 2");
      break;
  }
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

int getGear(int last_state) {
   if(digitalRead(DRIVE2_PIN))
     return DRIVE2_GEAR;
   else if(digitalRead(DRIVE_PIN))
     return DRIVE_GEAR;
   else if(digitalRead(NEUTRAL_PIN))
     return NEUTRAL_GEAR;
   else if(digitalRead(PARK_PIN))
     return PARK_GEAR;
   else if(digitalRead(REVERSE_PIN))
   {
     if(last_state == NEUTRAL_GEAR || last_state == PARK_GEAR)
       return REVERSE_GEAR;
     return last_state;
   }
   
   return last_state;
}
