int ir_l = A0;
int ir_m = A1;
int ir_r = A2;

int sensorValue = 0;

void setup() {
  pinMode(ir_l, INPUT);
  pinMode(ir_m, INPUT);
  pinMode(ir_r, INPUT);
  Serial.begin(9600);
}

void loop() {
  sensorValue = analogRead(ir_l);
  Serial.print(sensorValue);
  Serial.print(" ");
  sensorValue = analogRead(ir_m);
  Serial.print(sensorValue);
  Serial.print(" ");
  sensorValue = analogRead(ir_r);
  Serial.print(sensorValue);
  Serial.println();
  delay(200);
}
