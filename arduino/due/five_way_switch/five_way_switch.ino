/*
Code for detecting the position of a 5-way selector switch from Sparkfun (COM-10541)

Switch               Arduino
-------              --------         10k pull-downs
Pin3|----------------pin22------------\/\/\/\/-------GND
Pin2|----------------pin24------------\/\/\/\/-------GND
Pin1|----------------pin26------------\/\/\/\/-------GND
COM |----------------3.3V
*/

// Change these lines to change the input pins
#define pin1 22
#define pin2 24
#define pin3 26

// these variables save the input values
int pin_state1 = 0;
int pin_state2 = 0;
int pin_state3 = 0;

// remember the last value
// that way we can print only on change
int last_value = 0;

int switch_pos = 0;

void setup() {
  
  Serial.begin(9600);
  
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
  pinMode(pin3, INPUT);

}

void loop() {
  pin_state1 = digitalRead(pin1);
  pin_state2 = digitalRead(pin2);
  pin_state3 = digitalRead(pin3);
  
  if(pin_state1 == HIGH)
  {
    if(pin_state2 == HIGH)
     switch_pos = 2;
    else
     switch_pos = 1; 
  }
  else if(pin_state2 == HIGH)
  {
    if(pin_state1 == HIGH)
     switch_pos = 2;
    else if(pin_state3 == HIGH)
     switch_pos = 4;
    else
     switch_pos = 3; 
  }
  else if(pin_state3 == HIGH)
  {
    if(pin_state2 == HIGH)
      switch_pos = 4;
    else
      switch_pos = 5;
  }
  
  if(switch_pos != last_value)
  {
     Serial.print("Switch Position: ");
     Serial.println(switch_pos);
     Serial.println(""); 
  }
  
  last_value = switch_pos;
}
