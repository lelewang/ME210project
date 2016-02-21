/*
 * This is the test code for IR phototransistor
 * 
 * Input: voltage output of IR phototransistor circuits
 * Output: serial monitor print of duration and frequency
 * Note: looped about twice in a second
 */
int pin = 13;

unsigned long duration;

void setup()
{
  pinMode(pin, INPUT);
  Serial.begin(9600);
}

void loop()
{
  duration = pulseIn(pin, HIGH);
  Serial.print(duration);
  Serial.println(" is high (us)....");
  
  duration = pulseIn(pin, LOW);
  Serial.print(duration);
  Serial.println(" is low (us)....");
  
  delay(500);
}
