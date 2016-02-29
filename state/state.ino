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
#define TIMER1 120000 // 2 min game
#define TIMER2 2000 // drop time
#define TIMER3 1000 // reload time
#define IR_THRESH 800 // tape sensor

const int R_FWD = 77; // below 127 is forward (right wheel)
const int R_BWD = 177; // above 127 is backward (right wheel)
const int L_FWD = 180;
const int L_BWD = 77;

/* Global parameter */
static int state = STOP;
static long timer1_init = 0;
static long timer2_init = 0; 
static long timer3_init = 0;

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
void goBack(void);

int beaconFound(void);
int checkTape(int pin);

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
  
  reading = LOW;
  prev = LOW;
  Serial.begin(9600);
  stopMotor();

  Serial.begin(9600);
  myservo_left.attach(left);
  myservo_mid.attach(mid);
  myservo_right.attach(right);
  myservo_left.writeMicroseconds(1040);
  myservo_mid.writeMicroseconds(2100);
  myservo_right.writeMicroseconds(2000);
}

void loop() {
  int val = 0;
  int fl_val = checkTape(ir_fl);
  int fm_val = checkTape(ir_fm);
  int fr_val = checkTape(ir_fr);
  int bl_val = checkTape(ir_bl);
  int bm_val = checkTape(ir_bm);
  int br_val = checkTape(ir_br);
  
  if (state != STOP && checkTimer(TIMER1, timer1_init)) {
    Serial.println("final...");
    state = STOP;
    stopMotor();
    /* TODO: turn off everything */
    return; 
  } else {
    switch(state) {
      case(STOP):
        Serial.println("STOP");
        reading = digitalRead(enable);
        Serial.print(reading);
        if (reading == HIGH && prev == LOW && millis() - time > debounce) { 
          state = INIT;
          Serial.println("INIT");
          digitalWrite(led, HIGH);
          timer1_init = millis();
          //turnLeft();
          turnRight();
          time = millis();
        }
        prev = reading;
        break;
      case(INIT):
        if (beaconFound() == 1000) {
          state = BEACON;
          Serial.println("BEACON");
        }
        break;
      case(BEACON):
        if (fr_val) {
          goForward();
          delay(500);
          turnRight();
          delay(2000);
          goForward();
          state = LINE_FOLLOW;
          Serial.println("LINE_FOLLOW");
          return;
        } else if (fl_val || fm_val) {
          goForward();
          delay(500);
          turnRight();
          delay(500);
          state = LINE_FOLLOW;
          Serial.println("LINE_FOLLOW");
          return;
        } else if (bm_val || bl_val || br_val) {
          goBack();
          delay(2000);
          goForward();
        }
        val = beaconFound();
        if (val == 5000) {
          Serial.println("saw 5k");
          turnRight();
          delay(500);
//          goForward();
//          delay(200);
        } else if (val == 0) {
          goForward();
          Serial.println("saw nothing");
        } else if (val == 1000) {
          Serial.println("saw 1k");
          turnLeft();
          delay(100);
        }
        break;
      case(LINE_FOLLOW):
        if ((fl_val && fm_val) ||  (fm_val && fr_val)) {
          stopMotor();
          myservo_left.writeMicroseconds(1700);
          myservo_mid.writeMicroseconds(1400);
          myservo_right.writeMicroseconds(1300);
          stopMotor();
          state = DROP;
          Serial.println("DROP");
          timer2_init = millis();
        } else if (fm_val) {
          goForward();
        } else if (fl_val) {
          turnLeft();
        } else if (fr_val) {
          turnRight();
        }
        break;
      case(DROP):
        if (checkTimer(TIMER2, timer2_init)) {
          myservo_left.writeMicroseconds(1030);
          myservo_mid.writeMicroseconds(2100);
          myservo_right.writeMicroseconds(2000);
          goBack();
          state = LINE_BACK;
          Serial.println("LINE_BACK");
        }
        break;
      case(LINE_BACK):
        if ((bl_val && bm_val) ||  (bm_val && br_val)) {
          delay(200);
          stopMotor();
          state = RELOAD;
          Serial.println("RELOAD");
          timer3_init = millis();
        } else if (bm_val) {
          goBack();
        } else if (bl_val) {
          turnLeft();
        } else if (br_val) {
          turnRight();
        }
        break;
      case(RELOAD):
        if (checkTimer(TIMER3, timer3_init)) {
          goForward();
          state = LINE_FOLLOW;
          Serial.println("LINE_FOLLOW");
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

int beaconFound(void) {
  unsigned long duration = 0;
  unsigned int ret = 0;
  unsigned int count1 = 0;
  unsigned int count2 = 0;

  const int count = 3;
  
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
    ret = 1000;
  else if (count2 == count)
    ret = 5000;
  else
    ret = 0;
  return ret;
}

int checkTape(int pin) {
  int val = 0;
  val = analogRead(pin);
  if (val > IR_THRESH) {
      Serial.println(pin);
    return 1;
  }
  return 0;
}

