/******** ME210 Lab3 Part4 *********
 * Functionality:
 * 1. DC motor speed control by potentiometer
 * 2. DC motor direction reversed when a key is pressed
 * 
 * Input: voltage across potentiometer
 * Output: PWM analog signal for speed control
 *         digital logic for direction control
 *         
 * Setup: potentiometer -- arduino -- L293 H-bridge -- DC motor
 * *********************************/
 
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
