int value;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(2, OUTPUT);
  pinMode(3, INPUT);

  digitalWrite(2, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  value = digitalRead(3);
  Serial.println(value);
  delay(100);
}
