const int EchoPin = 26;
const int TriggerPin = 27;


void setup() {
  Serial.begin(9600);
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);

}


void loop() {
  float cm = measureDistance(TriggerPin, EchoPin);
  Serial.print("Distancia medida: ");
  Serial.println(cm);
  delay(200);
}


float measureDistance(int TriggerPin, int EchoPin) 
{
  float duration, distanceCm;
  digitalWrite(TriggerPin, LOW);
  delayMicroseconds(4);
  digitalWrite(TriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);
  duration = pulseIn(EchoPin, HIGH, 4000); // Timeout = 1.500 ms -- Max distance = 25.5cm
  distanceCm = duration * 10 / 340/ 2;

  return distanceCm;
}
