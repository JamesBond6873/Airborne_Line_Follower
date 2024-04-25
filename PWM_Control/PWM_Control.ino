#define RB 3
#define LB 1
#define RF 2
#define LF 0

void setup() {
  pinMode(RB, OUTPUT);
  pinMode(LB, OUTPUT);
  pinMode(RF, OUTPUT);
  pinMode(LF, OUTPUT);
}

void loop() {
  analogWrite(RB,105);
  analogWrite(LB, 255); 
  delay(10);
}