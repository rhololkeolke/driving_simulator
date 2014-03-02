/*
  This is example code for using the rotary encoder from adafruit. 
  https://www.adafruit.com/products/377
  
  To use hook up the middle pin to the arduino's GND.
  Hook up the left pin to pin 50 and the right pin to pin 52 on the due.
*/


#define PIN_ENCODER_A 50
#define PIN_ENCODER_B 52

static uint8_t enc_prev_pos = 0;
static uint8_t enc_flags = 0;

bool bit_is_set(int var, int bit)
{
  if(var & (1 << bit))
   return true;
  else
   return false; 
}

void setup() {
  Serial.begin(115200);
  
  pinMode(PIN_ENCODER_A, INPUT);
  pinMode(PIN_ENCODER_B, INPUT);
  digitalWrite(PIN_ENCODER_A, HIGH);
  digitalWrite(PIN_ENCODER_B, HIGH);
  
  // initial reading of encoder
  if(digitalRead(PIN_ENCODER_A) == LOW)
    enc_prev_pos |= (1 << 0);
  if(digitalRead(PIN_ENCODER_B) == LOW)
    enc_prev_pos |= (1 << 1);

}

void loop() {

  int8_t enc_action = 0; // 1 or -1 if moved, sign is direction
  
  uint8_t enc_cur_pos = 0;
  
  if(digitalRead(PIN_ENCODER_A) == LOW)
    enc_cur_pos |= (1 << 0);
  if(digitalRead(PIN_ENCODER_B) == LOW)
    enc_cur_pos |= (1 << 1);
    
// if any rotation at all
  if (enc_cur_pos != enc_prev_pos)
  {
    if (enc_prev_pos == 0x00)
    {
      // this is the first edge
      if (enc_cur_pos == 0x01) {
        enc_flags |= (1 << 0);
      }
      else if (enc_cur_pos == 0x02) {
        enc_flags |= (1 << 1);
      }
    }
 
    if (enc_cur_pos == 0x03)
    {
      // this is when the encoder is in the middle of a "step"
      enc_flags |= (1 << 4);
    }
    else if (enc_cur_pos == 0x00)
    {
      // this is the final edge
      if (enc_prev_pos == 0x02) {
        enc_flags |= (1 << 2);
      }
      else if (enc_prev_pos == 0x01) {
        enc_flags |= (1 << 3);
      }
 
      // check the first and last edge
      // or maybe one edge is missing, if missing then require the middle state
      // this will reject bounces and false movements
      if (bit_is_set(enc_flags, 0) && (bit_is_set(enc_flags, 2) || bit_is_set(enc_flags, 4))) {
        enc_action = 1;
      }
      else if (bit_is_set(enc_flags, 2) && (bit_is_set(enc_flags, 0) || bit_is_set(enc_flags, 4))) {
        enc_action = 1;
      }
      else if (bit_is_set(enc_flags, 1) && (bit_is_set(enc_flags, 3) || bit_is_set(enc_flags, 4))) {
        enc_action = -1;
      }
      else if (bit_is_set(enc_flags, 3) && (bit_is_set(enc_flags, 1) || bit_is_set(enc_flags, 4))) {
        enc_action = -1;
      }
 
      enc_flags = 0; // reset for next time
    }
  }
 
  enc_prev_pos = enc_cur_pos;
  
  if(enc_action != 0)
  {
    Serial.print("enc_action: ");
    Serial.println(enc_action);
    Serial.println("");
  }
}
