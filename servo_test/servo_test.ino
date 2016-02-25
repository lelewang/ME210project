const int pinNum = 10;
int onTime;
int offTime;
  
void setup() {
  pinMode(pinNum, OUTPUT);
  Serial.begin(9600);
  onTime = 1000;
  offTime = 20000-onTime;
}
void loop() {
  if (Serial.available()) {
    int inChar = Serial.read();
    if (inChar == '1') {
      onTime = 1000;
    } else if (inChar == '2') {
      onTime = 1500;
    } else if (inChar == '3') {
      onTime = 2000;
    }
    offTime = 20000 - onTime;
    Serial.println(onTime);
    for (int i=0; i<1000; ++i) {
      digitalWrite(pinNum,HIGH);
      delayMicroseconds(onTime);
      digitalWrite(pinNum,LOW);
      delayMicroseconds(offTime);
    }
  }
}

