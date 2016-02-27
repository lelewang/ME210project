int enable = 2;

int lm_dir = 10;
int rm_dir = 11;
int lm_pwm = 12;
int rm_pwm = 13;

static int L_FWD = 0;
static int L_BWD = 0;
static int R_FWD = 0;
static int R_BWD = 0;

/* Switch parameters */
int reading;           // the current reading from the input pin
int prev;
long time = 0;
long debounce = 200;   // the debounce time, increase if the output flickers
static int state = 0;

/******* Main function ************/
void setup() {
  pinMode(enable,INPUT);
  pinMode(lm_dir,OUTPUT);
  pinMode(lm_pwm,OUTPUT);
  pinMode(rm_dir,OUTPUT);
  pinMode(rm_pwm,OUTPUT);
  
  Serial.begin(9600);
  state = 0;
  reading = LOW;
  prev = LOW;
  Serial.begin(9600);
  stopMotor();
}

void loop() {
  if (state == 0) {
    reading = digitalRead(enable);
    Serial.println(reading);
    if (reading == HIGH && prev == LOW && millis() - time > debounce) { 
      state = 1; 
      Serial.println("INIT");
      time = millis();
    }
    prev = reading;
  } 
  else {
    int thresh = 120;
    int dist = 50;
    for(int i=0; i<11; ++i) {
      Serial.println(i+thresh);
      R_FWD = thresh + i - dist;
      R_BWD = thresh + i + dist;
      L_FWD = thresh + i + dist;
      L_BWD = thresh + i - dist;
      goForward();
      delay(4000);
      goBack();
      delay(4000);
    }
  }
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


