#define S0 3
#define S1 4
#define S2 5
#define S3 6
#define sensorOut 2
#define buzzerPin 17

int redPin = 13;
int greenPin = 12;
int bluePin = 11;

int frequencyR;
int frequencyG;
int frequencyB;

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  pinMode(buzzerPin, OUTPUT);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  // Setting frequency-scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  
  Serial.begin(9600);
}

void loop() {
  checkColor();  
  delay(100);
}

void checkColor()
{
  // RED
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  frequencyR = pulseIn(sensorOut, LOW);
  Serial.print("R= ");//printing name
  Serial.print(frequencyR);//printing RED color frequency
  Serial.print("  ");
  delay(100);

  
  //GREEN
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  frequencyG = pulseIn(sensorOut, LOW);
  Serial.print("G= ");//printing name
  Serial.print(frequencyG);//printing Green color frequency
  Serial.print("  ");
  delay(100);

  
  //BLUE
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  frequencyB = pulseIn(sensorOut, LOW);
  Serial.print("B= ");//printing name
  Serial.print(frequencyB);//printing Blue color frequency
  Serial.println("  ");

  if(frequencyR < frequencyG - 35 && frequencyR < frequencyB - 35) {
    Serial.println("RED");
    setColor(255, 0, 0);
    makeSound();
    }
  else if(frequencyG < frequencyR - 10 && frequencyG < frequencyB - 10) {
    Serial.println("GREEN");
    setColor(0, 255, 0);
    makeSound();
    }
  else if(frequencyB < frequencyG - 35 && frequencyB < frequencyR - 35) {
    Serial.println("BLUE");
    setColor(0, 0, 255);
    makeSound();
    }
  setColor(0, 0, 0); // LED OFF
}

void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, 255 - redValue);
  analogWrite(greenPin, 255 - greenValue);
  analogWrite(bluePin, 255 - blueValue);
}

void makeSound() {
  int t0 = millis();
  while (millis() < (t0 + 5000)){
    tone(buzzerPin, 1000); // Send 1KHz sound signal...
    delay(50);        // ...for 1 sec
    noTone(buzzerPin);     // Stop sound...
    delay(100);        // ...for 1sec
  }
}