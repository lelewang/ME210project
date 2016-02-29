#include <Servo.h> 

/* Input and output pin assignment */
#define enable 2 // overall control
#define led 1

#define ir_fl A0
#define ir_fm A1
#define ir_fr A2

#define ir_bl A3
#define ir_bm A4
#define ir_br A5

#define bc_in 8

#define lm_dir 5 // used to be 10
#define rm_dir 6 // used to be 11
#define lm_pwm 12
#define rm_pwm 13

#define left 9
#define mid 10
#define right 11

/* Servo motor */
Servo myservo_left;
Servo myservo_mid;
Servo myservo_right;

/* State definition */
#define STOP 0
#define INIT 1
#define BEACON 2
#define LINE_SEARCH 3
#define LINE_FOLLOW 4
#define DROP 5
#define LINE_BACK 6
#define RELOAD 7

/* Global constant */
#define TIMER1 120000 // 2 min game
#define TIMER2 2000 // drop time
#define TIMER3 1000 // reload time
#define IR_THRESH 850 // tape sensor

const int R_FWD = 77; // below 127 is forward (right wheel)
const int R_BWD = 177; // above 127 is backward (right wheel)
const int L_FWD = 177;
const int L_BWD = 77;


void stopMotor(void);
void goForward(void);

void setup() {
  pinMode(lm_dir,OUTPUT);
  pinMode(lm_pwm,OUTPUT);
  pinMode(rm_dir,OUTPUT);
  pinMode(rm_pwm,OUTPUT);
  
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
  goForward();
}
void loop() {

  delay(2000);
  stopMotor();
  myservo_left.writeMicroseconds(1700);
  myservo_mid.writeMicroseconds(1400);
  myservo_right.writeMicroseconds(1300);
  
  delay(2000);
  stopMotor();
  myservo_left.writeMicroseconds(1030);
  myservo_mid.writeMicroseconds(2100);
  myservo_right.writeMicroseconds(2000);
}

void stopMotor(void) {
  digitalWrite(lm_pwm,LOW);
  digitalWrite(rm_pwm,LOW);
}
void goForward(void) {
  digitalWrite(lm_pwm, HIGH);
  digitalWrite(rm_pwm, HIGH);
 
  analogWrite(lm_dir, L_FWD);
  analogWrite(rm_dir, R_FWD);
}
