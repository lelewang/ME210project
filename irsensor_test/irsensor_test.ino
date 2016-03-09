/* This file is used for IR tape sensor testing.
 * Input: three IR sensor signals from analog pins.
 * Output: three numerical reading printed on serial monitor.
 * 
 * Last Update: 02/28/2016
 */
int ir_l = A0;
int ir_m = A1;
int ir_r = A2;

#define lm_dir 5 // used to be 10 
#define rm_dir 6 // used to be 11
#define lm_pwm 4 // used to be 12
#define rm_pwm 7 // used to be 13

int sensorValue = 0;

void stopMotor(void);

void setup() {
  pinMode(ir_l, INPUT);
  pinMode(ir_m, INPUT);
  pinMode(ir_r, INPUT);
  
  pinMode(lm_dir,OUTPUT);
  pinMode(lm_pwm,OUTPUT);
  pinMode(rm_dir,OUTPUT);
  pinMode(rm_pwm,OUTPUT);
  Serial.begin(9600);
  stopMotor();
}

void loop() {
  sensorValue = analogRead(ir_l);
  Serial.print(sensorValue);
  Serial.print(" ");
  sensorValue = analogRead(ir_m);
  Serial.print(sensorValue);
  Serial.print(" ");
  sensorValue = analogRead(ir_r);
  Serial.print(sensorValue);
  Serial.println();
  delay(200);
}

void stopMotor(void) {
  digitalWrite(lm_pwm,LOW);
  digitalWrite(rm_pwm,LOW);
}
