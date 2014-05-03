#include <Wire.h>
#include <SabertoothSimplified.h>

struct Calibration {
  int brake_min;
  int brake_max;
  int gas_min;
  int gas_max;
  double rads_per_tick;
  int vibration_min;
  int vibration_max;
  int wheel_power_min;
  int wheel_power_max;
};

union PiDouble {
  byte b[8];
  double d;
};

#ifdef DEBUG
#define DEBUG_PRINT(msg) { Serial.print(msg); }
#else
#define DEBUG_PRINT(msg) {}
#endif

#define ENCODER_IS_REVERSED 1

#define ESTOP_PIN 25
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

#define GAS_PIN 0
#define BRAKE_PIN 1

#define ENCODER_A 52
#define ENCODER_B 50

#define SLAVE_ADDRESS 0x04

#define WHEEL_ANGLE_ID 0x00
#define GAS_POS_ID 0x01
#define BRAKE_POS_ID 0x02
#define GEAR_ID 0x03
#define VIBRATION_ID 0x04
#define KEY_TURNED_ID 0x05

#define WHEEL_MOTOR 2
#define VIBRATION_MOTOR 1

#define FSR1_PIN 6
#define FSR2_PIN 7

volatile bool keyswitch_on = false;
volatile int gearshift_state = UNKNOWN_GEAR;

volatile PiDouble gas;
volatile PiDouble brake;

struct Calibration cal;

volatile bool encoder_b_state;
volatile long encoder_ticks = 0;
volatile PiDouble wheel_angle;

volatile int received_byte_count = 0;
volatile PiDouble desired_wheel_angle;
volatile PiDouble desired_wheel_force;
volatile PiDouble desired_vibration;

volatile int next_data_to_send = WHEEL_ANGLE_ID;

double Kp = 50;
double Kf = 0.1;

double force_threshold = 120;

int fsr1_raw = 0; //right side relative to driver
int fsr2_raw = 0; //left side relative to driver

SabertoothSimplified ST;

void setup() {
  // set calibration values
  cal.brake_min = 190;
  cal.brake_max = 960;
  cal.gas_min = 417;
  cal.gas_max = 640;
  cal.rads_per_tick = 0.00770942982476;
  cal.vibration_min = 0;
  cal.vibration_max = 60;
  cal.wheel_power_min = -60;
  cal.wheel_power_max = 60;
  
  desired_wheel_angle.d = 0;
  desired_wheel_force.d = 0;
  desired_vibration.d = 0;
  
  wheel_angle.d = 0;
  
  SabertoothTXPinSerial.begin(9600);
  ST.motor(WHEEL_MOTOR, 0);
  ST.motor(VIBRATION_MOTOR, 0);
  
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
  
  // gas and brake setup
  gas.d = getGas();
  brake.d = getBrake();
  
  // Enocder setup
  pinMode(ENCODER_A, INPUT);
  pinMode(ENCODER_B, INPUT);
  attachInterrupt(ENCODER_A, handleEncoderChange, RISING);
  
  Wire.begin(SLAVE_ADDRESS);
  
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);

}

void loop() {
  keyswitch_on = digitalRead(KEYSWITCH_PIN);
  
  gearshift_state = getGear(gearshift_state);
  /*switch(gearshift_state) {
    case UNKNOWN_GEAR:
      DEBUG_PRINT("Unknown");
      break;
    case PARK_GEAR:
      DEBUG_PRINT("Park");
      break;
    case REVERSE_GEAR:
      DEBUG_PRINT("Reverse");
      break;
    case NEUTRAL_GEAR:
      DEBUG_PRINT("Neutral");
      break;
    case DRIVE_GEAR:
      DEBUG_PRINT("Drive");
      break;
    case DRIVE2_GEAR:
      DEBUG_PRINT("Drive 2");
      break;
  }
  DEBUG_PRINT("\n");*/
  
  gas.d = getGas();
  brake.d = getBrake();
  
  wheel_angle.d = encoder_ticks*cal.rads_per_tick;
  
  setVibration(desired_vibration.d);
  //setVibration(20);
  
  fsr1_raw = analogRead(FSR1_PIN);
  fsr2_raw = analogRead(FSR2_PIN);
  
  //if(millis() % 100 == 0)
  //{
    Serial.print("FSR1: ");
    Serial.print(fsr1_raw);
    Serial.print(" FSR2: ");
    Serial.println(fsr2_raw);
     /*Serial.print("Desired Wheel angle: ");
    Serial.println(desired_wheel_angle.d);*/
    Serial.print("Measured Wheel angle: ");
    Serial.println(wheel_angle.d);
    
   /* Serial.print("Brake :");
    Serial.println(brake.d);
    Serial.print("Gas :");
    Serial.println(gas.d);
     Serial.print("Wheel force: ");
      Serial.println(desired_wheel_force.d);
     Serial.print("vibration: ");
    Serial.println(desired_vibration.d); */
  //}
  if(fsr1_raw > force_threshold) //turning right -> assist right
  {
    setWheelPower(-Kf * fsr1_raw);
  }
  /*else if(fsr2_raw > fsr1_raw) //turning left -> assist left
  {
    setWheelPower(Kf * fsr2_raw);
  }*/
  else
  {
    /*if(wheel_angle.d > 0) //left turn -> go right
    {
      setWheelPower(-force_threshold);
    }
    else*/ if(wheel_angle.d < 0) //right turn -> go left
    {
      setWheelPower(Kp*-wheel_angle.d); 
    }
    else
    {
      setWheelPower(0); 
    }
  }
  
  //setWheelPower(Kp*(desired_wheel_angle.d - wheel_angle.d));
}

void setVibration(double magnitude) {
   magnitude = max(0, min(1, magnitude));
   int power = magnitude*(cal.vibration_max - cal.vibration_min) + cal.vibration_min;
   ST.motor(VIBRATION_MOTOR, 30);
}

void setWheelPower(double magnitude) {
   int power = (int)max(cal.wheel_power_min, min(cal.wheel_power_max, magnitude));
  //if(millis() % 100)
  //{
    Serial.print("magnitude: ");
    Serial.println(magnitude);
    Serial.print("power: ");
    Serial.println(power);
  //}*/
  ST.motor(WHEEL_MOTOR, power);
}

void activateEstop() {
  bool estop_active = true;
  do {
    Serial.println("Estop");
    estop_active = digitalRead(ESTOP_PIN);
    delayMicroseconds(500000);
  } while(estop_active);
  received_byte_count = 0;
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

float getGas() {
   float val = (float)(analogRead(GAS_PIN) - (float)cal.gas_min)/((float)(cal.gas_max - cal.gas_min)); 
   return max(min(val, 1), 0);
}

float getBrake() {
  float val = (float)(analogRead(BRAKE_PIN) - (float)cal.brake_min)/((float)(cal.brake_max - cal.brake_min));
   return max(min(val, 1), 0);
}

void handleEncoderChange() {
    encoder_b_state = digitalRead(ENCODER_B);
    // and adjust counter + if A leads B
    #ifdef ENCODER_IS_REVERSED
      encoder_ticks -= encoder_b_state ? -1 : +1;
    #else
      encoder_ticks += encoder_b_state ? -1 : +1;
    #endif
}

void receiveData(int byteCount) {
  if(byteCount < 25)
  {
    while(Wire.available())
      Wire.read();
  }
  while(Wire.available())
  {
    received_byte_count = received_byte_count % 24;
    if(received_byte_count == 0)
      Wire.read(); // skip the first value
      
    switch(received_byte_count / 8)
    {
     case 0:
      desired_wheel_angle.b[received_byte_count % 8] = Wire.read();
      break;
     case 1:
      desired_wheel_force.b[received_byte_count % 8] = Wire.read();
      break;
     case 2:
      desired_vibration.b[received_byte_count % 8] = Wire.read();
      break;
    }
    
    received_byte_count++;
  }

}

void sendData() {
  static int temp=0;
  switch(next_data_to_send) {
  case WHEEL_ANGLE_ID:
   Wire.write(WHEEL_ANGLE_ID);
   for(int i=0; i<8; i++)
   {
      Wire.write(wheel_angle.b[i] >> 4);
      Wire.write(wheel_angle.b[i] & 0x0F); 
   }
    break;
  case GAS_POS_ID:
   Wire.write(GAS_POS_ID);
   for(int i=0; i<8; i++)
   {
      Wire.write(gas.b[i] >> 4);
      Wire.write(gas.b[i] & 0x0F);
   }
  break;
  case BRAKE_POS_ID:
   Wire.write(BRAKE_POS_ID);
   for(int i=0; i<8; i++)
   {
      Wire.write(brake.b[i] >> 4);
      Wire.write(brake.b[i] & 0x0F); 
   }
    break; 
  case GEAR_ID:
    Wire.write(GEAR_ID);
    Wire.write(gearshift_state);
    break;
  case VIBRATION_ID:
    Wire.write(VIBRATION_ID);
    for(int i=0; i<8; i++)
   {
      Wire.write(desired_vibration.b[i] >> 4);
      Wire.write(desired_vibration.b[i] & 0x0F); 
   }
    break;
  case KEY_TURNED_ID:
    Wire.write(KEY_TURNED_ID);
    Wire.write(keyswitch_on);
    break;
  }
  next_data_to_send++;
  next_data_to_send = next_data_to_send % (KEY_TURNED_ID + 1);
}
