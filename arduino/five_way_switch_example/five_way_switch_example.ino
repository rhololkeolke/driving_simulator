/* 
3-24-11
NPoole

This code is public domain.

To use this example code, connect the 5-way selector switch [COM-10541] 
in the following manner:

Switch                    Arduino
___                                  10k pull-downs
Pin3|D---------------------pin2---------\/\/\/------GND
Pin2|D---------------------pin3---------\/\/\/------GND
Pin1|D---------------------pin4---------\/\/\/------GND
COM |D---------------------VDD
    |

You only need to read three pins in order to tell the position of the switch (plus Common) because
there are actually 2 seperate wipers in the switch, each side (1,2,3,COM) is a mirror image of the other
and they're are not interconnected. As you can (hopefully) tell from the ASCII diagram above, you're 
going to wire each pin basically as if it were a button. (For more info on that, see the Arduino 'Button' 
Example). 

This Sketch will read the state of the three output pins on the selector and output a number on the serial 
terminal 1-5 corresponding to the position of the lever.
*/

//First we declare our pins

#define pin1 2
#define pin2 3
#define pin3 4

//These variables will hold our digital read values

int pinState1 = 0;
int pinState2 = 0;
int pinState3 = 0;

//This will keep the last number sent to the terminal 
//in this way we'll keep from cramming the terminal
//with a continuous stream of numbers.

int lastvalue = 0;

//And finally, this variable will hold the current switch position

int switchpos = 0;

void setup() {
  // And... Serial
  
  Serial.begin(9600);
  
  // Now we set up our pinModes

pinMode(pin1, INPUT);
pinMode(pin2, INPUT);
pinMode(pin3, INPUT);

}

void loop() {
  //First we read the pins:
  
  pinState1 = digitalRead(pin1);
  pinState2 = digitalRead(pin2);
  pinState3 = digitalRead(pin3);
    
  // Now we'll use a list of if then conditions to determine the switch position
  // because that's a nice transparent way of doing it. You could also 
  // use switch case or other methods. Basically, with each sweep we check to
  // see if each pin is high, if a pin is high, we check the surrounding pins.
  // with a little logic, we can deduce from that information which position the 
  // switch is in.

if(pinState1 == HIGH){
  
  if(pinState2 == HIGH){switchpos = 2;}
  else {switchpos = 1;}}
  
else if(pinState2 == HIGH){
  
  if(pinState1 == HIGH){switchpos = 2;}
  else if(pinState3 == HIGH){switchpos = 4;}
  else {switchpos = 3;}}
  
else if(pinState3 ==HIGH){
  
  if(pinState2 == HIGH){switchpos = 4;}
  else {switchpos = 5;}}
  
  //Now that we know which position our switch is in, let's print it to the terminal.
  //But first, we'll make sure the switch has moved
  
  if(switchpos != lastvalue){
    Serial.print("Switch Position:");
    Serial.println(switchpos);
    Serial.println(""); //make some room
    }
    
      //Now remember that for the next sweep.
  
  lastvalue = switchpos;
  
}
