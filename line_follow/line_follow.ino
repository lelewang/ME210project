
/* Input and output pin assignment */
int ir_fl = A0;
int ir_fm = A1;
int ir_fr = A2;

int ir_bl = A3;
int ir_bm = A4;
int ir_br = A5;


int lm_dir = 10;
int rm_dir = 11;
int lm_pwm = 12;
int rm_pwm = 13;

/* State definition */
#define LINE_FOLLOW 4
#define DROP 5
#define LINE_BACK 6
#define RELOAD 7

/* Global constant */
#define TIMER1 120000
#define TIMER2 3000
#define TIMER3 3000
#define IR_THRESH 900

int R_FWD = 82; // below 125 is forward (right wheel)
int R_BWD = 172; // above 125 is backward (right wheel)
int L_FWD = 172;
int L_BWD = 82;

/* Global parameter */
static int state = LINE_FOLLOW;
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

int beaconFound(void);
int checkTape(int pin);

/******* Main function ************/
void setup() {

  pinMode(ir_fl,INPUT);
  pinMode(ir_fm,INPUT);
  pinMode(ir_fr,INPUT);
  
  pinMode(ir_bl,INPUT);
  pinMode(ir_bm,INPUT);
  pinMode(ir_br,INPUT);

  pinMode(lm_dir,OUTPUT);
  pinMode(lm_pwm,OUTPUT);
  pinMode(rm_dir,OUTPUT);
  pinMode(rm_pwm,OUTPUT);
  
  reading = LOW;
  Serial.begin(9600);
  goForward();
}

void loop() {
  if (Serial.available()) {
    int inChar = Serial.parseInt();
    R_FWD = inChar;
    R_BWD = 255 - inChar;
    L_BWD = R_FWD;
    L_FWD = R_BWD;
    Serial.println(R_FWD);
  }
  switch(state) {
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
      } else {
        goForward();
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

bool checkTimer(int duration, int init) {
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

int checkTape(int pin) {
  int val = 0;
  val = analogRead(pin);
  if (val > IR_THRESH) {
    Serial.print("IR value");
    Serial.print(pin);
    Serial.println(val);
    return 1;
  }
  return 0;
}

