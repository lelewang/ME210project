/* This code is used for motor test. 
 *  It applies PWM on the direction instead of enable pin of the H bridge.
 *  Inputs: N.A.
 *  Outputs: 4 digital signals for enable and direction of H bridge to drive two DC motors
 */
 
int lm_dir = 10;
int rm_dir = 11;
int lm_pwm = 12;
int rm_pwm = 13;

int sensorValue = 0; //read from analog input
int outputValue = 0; //convert to duty ratio for pwm output
int pwmPeriod = 1023;

void setup() {
  // configure Pin 13 to be digital output (customized pwm)
  pinMode(lm_dir,OUTPUT);
  pinMode(lm_pwm,OUTPUT);
  pinMode(rm_dir,OUTPUT);
  pinMode(rm_pwm,OUTPUT);
  
  // open the serial port:
  Serial.begin(9600);
}

void loop() {
  static bool dir = true;
  static int motorSpeed = 0;

  //sensorValue = analogRead(sensorPin);
  digitalWrite(lm_pwm, HIGH);
  digitalWrite(rm_pwm, HIGH);
  //analogWrite(dirPin, 10);
  int i = 80;
  while(true) {
    //outputValue = map(sensorValue, 0, 1023, 0, 255);
    //outputValue = map(sensorValue, 0, 1023, 0, 255);
    for (i=80; i<170; ++i) {
      Serial.println(i);
      analogWrite(lm_dir, i);
      analogWrite(rm_dir, 255-i);
      delay(100);
    }
    for (i = 170; i > 80; --i) {
      Serial.println(i);
      analogWrite(lm_dir, i);
      analogWrite(rm_dir, 255-i);
      delay(100);
    }
  }
}
