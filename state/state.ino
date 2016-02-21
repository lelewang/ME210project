
/* Input and output pin assignment */
int enable = 0; // overall control
int led = 1;

int ir_fl = A0;
int ir_fm = A1;
int ir_fr = A2;

int ir_bl = A3;
int ir_bm = A4;
int ir_br = A5;

int bc_in = 13;

int lm_dir = 12;
int lm_pwm = 11;
int rm_pwm = 9;
int rm_dir = 8;

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
#define TIMER1 120000
#define TIMER2 3000
#define TIMER3 3000
#define IR_THRESH 800

/* Global parameter */
static int state = STOP;
static int timer1_init = 0;
static int timer2_init = 0; 
static int timer3_init = 0;

/* Switch parameters */
int reading;           // the current reading from the input pin
long time = 0;
long debounce = 200;   // the debounce time, increase if the output flickers

/* Helper function */
bool checkTimer(int duration, int init);
void stopMotor(void);
void turnLeft(void);
void turnRight(void);
void goForward(void);
void goBack(void);
void reverseTurnLeft(void);
void reverseTurnRight(void);

int beaconFound(void);
int checkTape(int pin);

/******* Main function ************/
void setup() {
  reading = LOW;
}

void loop() {
  if (state != STOP && checkTimer(TIMER1, timer1_init)) {
    state = STOP;
    /* TODO: turn off everything */
    return; 
  } else {
    switch(state) {
      case(STOP):
        reading = digitalRead(enable);
        if (reading == HIGH && millis() - time > debounce) { 
          state = INIT;
          digitalWrite(led, HIGH);
          timer1_init = millis();
          turnLeft();
          time = millis();
        }
      case(INIT):
        if (beaconFound() == 1000) {
          state = BEACON;
        }
        break;
      case(BEACON):
        if (beaconFound() == 0) {
          goForward();
          state = LINE_SEARCH;
        }
        break;
      case(LINE_SEARCH): 
        if (checkTape(ir_fr)) {
          reverseTurnRight();
          delay(2000);
          goForward();
        } else if (checkTape(ir_fl)) {
          goForward();
          state = LINE_FOLLOW;
        }
        break;
      case(LINE_FOLLOW):
        if (checkTape(ir_fl) && checkTape(ir_fm) && checkTape(ir_fr)) {
          stopMotor();
          state = DROP;
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
        }
        break;
      case(LINE_BACK):
        if (checkTape(ir_bl) && checkTape(ir_bm) && checkTape(ir_br)) {
          delay(200);
          stopMotor();
          state = RELOAD;
          timer3_init = millis();
        } else if (checkTape(ir_bm)) {
          goBack();
        } else if (checkTape(ir_bl)) {
          reverseTurnLeft();
        } else if (checkTape(ir_br)) {
          reverseTurnRight();
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

bool checkTimer(int duration, int init) {
  return (millis()-init > duration);
}

void turnLeft(void) {
  // right motor forward
}
void turnRight(void) {
  
}
void reverseTurnRight(void) {
  // right motor reverse
}
void reverseTurnLeft(void) {
  
}

void goForward(void) {
  
}
void goBack(void) {
  
}
void stopMotor(void) {
  
}

int beaconFound(void) {
  unsigned long duration = 0;
  unsigned int ret = 0;
  
  for(int i=0; i<5; ++i) {
    duration = pulseIn(bc_in, HIGH);
    duration = pulseIn(bc_in, LOW) + duration;
  }
  duration = duration / 5; 

  if (duration > 700)
    ret = 1000;
  else if ((duration < 300) && (duration > 100)) 
    ret = 5000;
  else
    ret = 0;
  return ret;
}

int checkTape(int pin) {
  int val = 0;
  val = analogRead(pin);
  if (pin > IR_THRESH) 
    return 1;
  return 0;
}

