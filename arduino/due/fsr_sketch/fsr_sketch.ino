#define FSR1_PIN 6
#define FSR2_PIN 7

float fsr_1_force = 0;
float fsr_2_force = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  fsr_1_force = (float)(analogRead(FSR1_PIN));
  fsr_2_force = (float)(analogRead(FSR2_PIN));
  
  Serial.print("FSR 1 : ");
  Serial.print(fsr_1_force);
  Serial.print(" FSR 2 : ");
  Serial.println(fsr_2_force);
}
