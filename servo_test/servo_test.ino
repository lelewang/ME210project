/*
 * This code tests functionality of Servo library and position function of servo motor.
 * Inputs: N.A.
 * Outputs: 3 digital signals for servo motor enable.
 */
#include <Servo.h> 

#define left 9
#define mid 10
#define right 11

/* Servo motor */
Servo myservo_left;
Servo myservo_mid;
Servo myservo_right;

void setup() {
  pinMode(left, OUTPUT);
  pinMode(mid, OUTPUT);
  pinMode(right, OUTPUT);
  Serial.begin(9600);
  myservo_left.attach(left);
  myservo_mid.attach(mid);
  myservo_right.attach(right);
  myservo_left.writeMicroseconds(1040);
  myservo_mid.writeMicroseconds(2100);
  myservo_right.writeMicroseconds(2000);
}
void loop() {

  delay(2000);
  myservo_left.writeMicroseconds(1700);
  myservo_mid.writeMicroseconds(1400);
  myservo_right.writeMicroseconds(1300);
  
  delay(2000);
  myservo_left.writeMicroseconds(1030);
  myservo_mid.writeMicroseconds(2100);
  myservo_right.writeMicroseconds(2000);
}
