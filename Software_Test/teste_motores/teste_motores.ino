#define RF 0 // Right Motor Forward Pin
#define RB 1 // Right Motor Backward Pin
#define LF 2 // Left Motor Forward Pin
#define LB 3 // Left Motor Backward Pin

#define Led 25

void setup() {
  pinMode(RF, OUTPUT);
  pinMode(RB, OUTPUT);
  pinMode(LF, OUTPUT);
  pinMode(LB, OUTPUT);

  pinMode(Led, OUTPUT);

  // Set Stop motions
  digitalWrite(RF, LOW);
  digitalWrite(RB, LOW);
  digitalWrite(LF, LOW);
  digitalWrite(LB, LOW);
}

void loop() {
  digitalWrite(Led, HIGH);
  digitalWrite(LB, LOW);
  digitalWrite(RB, LOW);
  digitalWrite(LB, LOW);
  digitalWrite(RF, HIGH);
  digitalWrite(LF, HIGH);
  delay(1000);

  digitalWrite(Led, LOW);
  digitalWrite(RF, LOW);
  digitalWrite(LF, LOW);
  digitalWrite(RB, HIGH);
  digitalWrite(LB, HIGH);
  delay(1000);
}