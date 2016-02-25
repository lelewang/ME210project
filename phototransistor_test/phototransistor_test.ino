/*
 * This is the test code for IR phototransistor
 * 
 * Input: voltage output of IR phototransistor circuits
 * Output: serial monitor print of duration and frequency
 * Note: looped about twice in a second
 */
int pin = 13;

unsigned long duration1;
unsigned long duration2;
void setup()
{
  pinMode(pin, INPUT);
  Serial.begin(9600);
}

void loop()
{
  duration1 = pulseIn(pin, HIGH);
  Serial.print(duration1);
  Serial.println(" is high (us)....");
  
  duration2 = pulseIn(pin, LOW);
  Serial.print(duration2);
  Serial.println(" is low (us)....");
  Serial.print(duration1 + duration2);
  Serial.println();
  
  delay(1000);
}
