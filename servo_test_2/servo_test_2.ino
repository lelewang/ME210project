const int left = 3;
const int mid = 5;
const int right = 6;
#include <Servo.h> 

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
  if (Serial.available()) {
    int inChar = Serial.read();
    if (inChar == '1') {
      myservo_left.writeMicroseconds(1850);
      myservo_mid.writeMicroseconds(1400);
      myservo_right.writeMicroseconds(1250);
    } else if (inChar == '2') {
      myservo_left.writeMicroseconds(1030);
      myservo_mid.writeMicroseconds(2100);
      myservo_right.writeMicroseconds(2000);
    } 
    Serial.println(inChar);
    }
}


