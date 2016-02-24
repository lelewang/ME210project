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
 
const int sensorPin = A0;
const int speedPin = 9; 
const int dirPin = 10;

int sensorValue = 0; //read from analog input
int outputValue = 0; //convert to duty ratio for pwm output
int pwmPeriod = 1023;

void setup() {
  // configure Pin 13 to be digital output (customized pwm)
  pinMode(speedPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(sensorPin, INPUT);
  digitalWrite(dirPin, HIGH); // direction initialization
  // open the serial port:
  Serial.begin(9600);
}

void loop() {
  static bool dir = true;
  static int motorSpeed = 0;

  if (Serial.available()) {
    char inChar = Serial.read();
    if (dir == true) {
      dir = false;
      digitalWrite(dirPin, LOW);
    } else {
      dir = true;
       digitalWrite(dirPin, HIGH);
    }
  }
  
  //sensorValue = analogRead(sensorPin);
  outputValue = 1000;
  //outputValue = map(sensorValue, 0, 1023, 0, 255);
  analogWrite(speedPin, outputValue);
}
