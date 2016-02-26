
/* Input and output pin assignment */
int enable = 2; // overall control
int led = 1;

int ir_fl = A0;
int ir_fm = A1;
int ir_fr = A2;

int ir_bl = A3;
int ir_bm = A4;
int ir_br = A5;

int bc_in = 8;

int lm_dir = 10;
int rm_dir = 11;
int lm_pwm = 12;
int rm_pwm = 13;

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
#define TIMER1 30000
#define TIMER2 3000
#define TIMER3 3000
#define IR_THRESH 900

const int R_FWD = 80; // below 125 is forward (right wheel)
const int R_BWD = 170; // above 125 is backward (right wheel)
const int L_FWD = 170;
const int L_BWD = 80;

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
  
  reading = LOW;
  prev = LOW;
  Serial.begin(9600);
  stopMotor();
}

void loop() {
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
          turnRight();
          time = millis();
        }
        prev = reading;
        break;
      case(INIT):
        if (beaconFound() == 5000) {
          state = BEACON;
          Serial.println("BEACON");
        }
        break;
      case(BEACON):
        if (beaconFound() != 5000) {
          Serial.println("pre line search");
          turnRight();
          delay(600);
          goForward();
          state = LINE_SEARCH;
          Serial.println("LINE_SEARCH");
        }
        break;
      case(LINE_SEARCH): 
        if (checkTape(ir_fr)) {
          turnRight();
          delay(3000);
          goForward();
        } else if (checkTape(ir_fl)) {
          goForward();
          delay(500);
          turnRight();
          delay(500);
          state = LINE_FOLLOW;
          Serial.println("LINE_FOLLOW");
        }
        break;
      case(LINE_FOLLOW):
        if (checkTape(ir_fl) && checkTape(ir_fm) && checkTape(ir_fr)) {
          stopMotor();
          state = DROP;
          Serial.println("DROP");
          timer2_init = millis();
          /* TODO: dropping mechanism, e.g. start dropping timer */
        } else if (checkTape(ir_fm)) {
          goForward();
        } else if (checkTape(ir_fl)) {
          turnLeft();
        } else if (checkTape(ir_fr)) {
          turnRight();
        }
        break;
      case(DROP):
        if (checkTimer(TIMER2, timer2_init)) {
          /* TODO: retract wings */
          goBack();
          state = LINE_BACK;
          Serial.println("LINE_BACK");
        }
        break;
      case(LINE_BACK):
        if (checkTape(ir_bl) && checkTape(ir_bm) && checkTape(ir_br)) {
          delay(200);
          stopMotor();
          state = RELOAD;
          Serial.println("RELOAD");
          timer3_init = millis();
        } else if (checkTape(ir_bm)) {
          goBack();
        } else if (checkTape(ir_bl)) {
          turnLeft();
        } else if (checkTape(ir_br)) {
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
}

int beaconFound(void) {
  unsigned long duration = 0;
  unsigned int ret = 0;
  unsigned int count1 = 0;
  unsigned int count2 = 0;

  const int count = 5;
  
  for(int i=0; i<count; ++i) {
    duration = pulseIn(bc_in, HIGH);
    duration = pulseIn(bc_in, LOW) + duration;
    if ((duration > 800) && (duration < 1200)) // period = 1000 for 1k
      count1 = count1 + 1;
    else if ((duration < 300) && (duration > 100)) // period = 200 for 5k
      count2 = count2 + 1;
    else {
      break;
    }
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
//    Serial.print("IR value");
      Serial.println(pin);
//    Serial.println(val);
    return 1;
  }
  return 0;
}

