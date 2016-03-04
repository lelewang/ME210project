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
#define lm_pwm 4 // used to be 12
#define rm_pwm 7 // used to be 13

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
#define TIMER1 120000//120000 // 2 min game
#define TIMER2 300 // drop time
#define TIMER3 1000 // reload time

#define IR_THRESH 600 // tape sensor

const int R_FWD = 52; // below 127 is forward (right wheel)
const int R_BWD = 202; // above 127 is backward (right wheel)
const int L_FWD = 197;
const int L_BWD = 57;


const int R_FWD_S = 67; // below 127 is forward (right wheel)
const int R_BWD_S = 187; // above 127 is backward (right wheel)
const int L_FWD_S = 187;
const int L_BWD_S = 67;

const byte mask_fl = B00100000;
const byte mask_fm = B00010000;
const byte mask_fr = B00001000;
const byte mask_bl = B00000100;
const byte mask_bm = B00000010;
const byte mask_br = B00000001;

byte tape_reading = B0;
int beacon_reading = 0;
int beacon_old = 0;

/* Global parameter */
static int state = STOP;
static long timer1_init = 0;
static long timer2_init = 0; 
static long timer3_init = 0;
static long timer_beacon_init = 0;
static int TIMER_BEACON = 0;

/* Switch parameters */
int reading;           // the current reading from the input pin
int prev;
long time = 0;
long debounce = 200;   // the debounce time, increase if the output flickers

/* Helper function */
bool checkTimer(long duration, long init);
void stopMotor(void);
void turnLeft(void);
void turnRight(void);
void goForward(void);
void turnLeftS(void);
void turnRightS(void);
void goForwardS(void);
void goBack(void);

void readBeacon(void);
void readTape();

/******* Main function ************/
void setup() {
  pinMode(enable,INPUT);
  pinMode(led,OUTPUT);

  pinMode(ir_fl,INPUT);
  pinMode(ir_fm,INPUT);
  pinMode(ir_fr,INPUT);
  
  pinMode(ir_bl,INPUT);
  pinMode(ir_bm,INPUT);
  pinMode(ir_br,INPUT);

  pinMode(bc_in,INPUT);

  pinMode(lm_dir,OUTPUT);
  pinMode(lm_pwm,OUTPUT);
  pinMode(rm_dir,OUTPUT);
  pinMode(rm_pwm,OUTPUT);

  pinMode(left, OUTPUT);
  pinMode(mid, OUTPUT);
  pinMode(right, OUTPUT);
  
  stopMotor();
  
  reading = LOW;
  prev = LOW;

  Serial.begin(9600);
  myservo_left.attach(left);
  myservo_mid.attach(mid);
  myservo_right.attach(right);
  myservo_left.writeMicroseconds(1040);
  myservo_mid.writeMicroseconds(2100);
  myservo_right.writeMicroseconds(2000);
}


void loop() {
  readTape();
  
  if (state != STOP && checkTimer(TIMER1, timer1_init)) {
    state = STOP;
    stopMotor();
    return;
     
  } else {
    switch(state) {
      
      case(STOP):
        reading = digitalRead(enable);
        if (reading == HIGH && prev == LOW && millis() - time > debounce) { 
          state = INIT;
          digitalWrite(led, HIGH);
          timer1_init = millis();
          turnRightS();
          time = millis();
        }
        prev = reading;
        break;
        
      case(INIT):
        readBeacon();
        if (beacon_reading == 1000) {
          state = BEACON;
        } else if ((beacon_reading==0) && (beacon_old==1)) {
          turnRightS();
          delay(500);
          state = BEACON;
        } else if (beacon_reading == 5000) {
          beacon_old = 1;
        }
        break;
        
      case(BEACON):
        if (tape_reading & mask_fr) {
          goForwardS();
          delay(400);
          turnRightS();
          delay(1300);
          goForward();
          state = LINE_FOLLOW;
          return;
        } else if (tape_reading & (mask_fr | mask_fm)) {
          goForwardS();
          delay(400);
          turnRightS();
          delay(400);
          state = LINE_FOLLOW;
          return;
        } else if (tape_reading & (mask_bl | mask_bm | mask_br)) {
          goBack();
          delay(1000);
          goForward();
        }
        
        if (!checkTimer(TIMER_BEACON, timer_beacon_init)) return;
        
        readBeacon();
        if (beacon_reading == 5000) {
          turnRight();
          TIMER_BEACON = 400;
          timer_beacon_init = millis();
        } else if (beacon_reading == 0) {
          goForward();
        } else if (beacon_reading == 1000) {
          turnLeft();
          TIMER_BEACON = 50;
          timer_beacon_init = millis();
        }
        break;
        
      case(LINE_FOLLOW):
        if ( (tape_reading & mask_fm) && ((tape_reading & mask_fl) || (tape_reading & mask_fr)) ) {
          //adjustPos();
          goForward();
          delay(120);
          stopMotor();
          
          myservo_left.writeMicroseconds(1700);
          myservo_mid.writeMicroseconds(1400);
          myservo_right.writeMicroseconds(1250);
          state = DROP;
          timer2_init = millis();
        } else if (tape_reading & mask_fm) {
          goForward();
        } else if (tape_reading & mask_fl) {
          turnLeft();
          delay(100);
          goForward();
        } else if (tape_reading & mask_fr) {
          turnRight();
          delay(100);
          goForward();
        } 
        break;
        
      case(DROP):
        if (checkTimer(TIMER2, timer2_init)) {
          goBack();
          state = LINE_BACK;
          myservo_left.writeMicroseconds(1030);
          myservo_mid.writeMicroseconds(2100);
          myservo_right.writeMicroseconds(2000);
        }
        break;
        
      case(LINE_BACK):
        if ( (tape_reading & mask_bm) && ((tape_reading & mask_bl) || (tape_reading & mask_br)) ) {
          delay(200);
          stopMotor();
          state = RELOAD;
          timer3_init = millis();
        } else if (tape_reading & mask_bm) {
          goBack();
        } else if (tape_reading & mask_bl) {
          turnLeft();
          delay(100);
          goBack();
        } else if (tape_reading & mask_br) {
          turnRight();
          delay(100);
          goBack();
        }
        break;
        
      case(RELOAD):
        if (checkTimer(TIMER3, timer3_init)) {
          goForward();
          state = LINE_FOLLOW;
        }
        break; 
    }
  }
}

bool checkTimer(long duration, long init) {
  return (millis()-init > duration);
}

void turnLeft(void) {
  // right motor forward
  digitalWrite(lm_pwm, HIGH);
  digitalWrite(rm_pwm, HIGH);
  
  analogWrite(lm_dir, L_BWD);
  analogWrite(rm_dir, R_FWD);
}
void turnRight(void) {
  digitalWrite(lm_pwm, HIGH);
  digitalWrite(rm_pwm, HIGH);
  
  analogWrite(lm_dir, L_FWD);
  analogWrite(rm_dir, R_BWD);
}

void goForward(void) {
  digitalWrite(lm_pwm, HIGH);
  digitalWrite(rm_pwm, HIGH);
 
  analogWrite(lm_dir, L_FWD);
  analogWrite(rm_dir, R_FWD);
}

void turnLeftS(void) {
  // right motor forward
  digitalWrite(lm_pwm, HIGH);
  digitalWrite(rm_pwm, HIGH);
  
  analogWrite(lm_dir, L_BWD_S);
  analogWrite(rm_dir, R_FWD_S);
}
void turnRightS(void) {
  digitalWrite(lm_pwm, HIGH);
  digitalWrite(rm_pwm, HIGH);
  
  analogWrite(lm_dir, L_FWD_S);
  analogWrite(rm_dir, R_BWD_S);
}

void goForwardS(void) {
  digitalWrite(lm_pwm, HIGH);
  digitalWrite(rm_pwm, HIGH);
 
  analogWrite(lm_dir, L_FWD_S);
  analogWrite(rm_dir, R_FWD_S);
}

void goBack(void) {
  digitalWrite(lm_pwm, HIGH);
  digitalWrite(rm_pwm, HIGH);
  
  analogWrite(lm_dir, L_BWD);
  analogWrite(rm_dir, R_BWD);
}
void stopMotor(void) {
  digitalWrite(lm_pwm,LOW);
  digitalWrite(rm_pwm,LOW);
  analogWrite(lm_dir, 127);
  analogWrite(rm_dir, 127);
}

void readBeacon(void) {
  unsigned long duration = 0;
  unsigned int ret = 0;
  unsigned int count1 = 0;
  unsigned int count2 = 0;

  const int count = 6;
  
  for(int i=0; i<count; ++i) {
    duration = pulseIn(bc_in, HIGH);
    if ((duration > 400) && (duration < 600)) // period = 1000, duty cycle = 500 for 1k
      count1 = count1 + 1;
    else if ((duration < 250) && (duration > 50)) // period = 200, duty cycle = 100 for 5k
      count2 = count2 + 1;
    else 
      break;
  }
  if (count1 == count)
    beacon_reading = 1000;
  else if (count2 == count)
    beacon_reading = 5000;
  else
    beacon_reading = 0;
}

void readTape() {
  tape_reading = 0;
  tape_reading = ( ((analogRead(ir_fl) > IR_THRESH) << 5) 
               |((analogRead(ir_fm) > IR_THRESH) << 4) 
               |((analogRead(ir_fr) > IR_THRESH) << 3) 
               |((analogRead(ir_bl) > IR_THRESH) << 2) 
               |((analogRead(ir_bm) > IR_THRESH) << 1) 
               |(analogRead(ir_br) > IR_THRESH) );
}

void adjustPos() {
  if ((tape_reading & mask_fl) && (tape_reading & mask_fr)) return;
  if (tape_reading & mask_bl) {
    turnRight();
    delay(50);
  } else if (tape_reading & mask_br) {
    turnLeft();
    delay(50);
  }
}

