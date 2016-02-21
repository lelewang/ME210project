int ir_in = 0;
int sensorValue = 0;

void setup() {
  pinMode(ir_in, INPUT);
  Serial.begin(9600);
}

void loop() {
  sensorValue = analogRead(ir_in);
  Serial.print(sensorValue);
  Serial.println();
  delay(200);
}
