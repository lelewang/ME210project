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
  Serial.write(duration);
  Serial.println(" is the period (ms)");
  Serial.write(1/duration);
  Serial.println(" is the frequency (kHz)");
  
  delay(500);
}
