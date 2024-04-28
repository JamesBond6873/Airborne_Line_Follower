#define S0 3
#define S1 4
#define S2 5
#define S3 6
#define sensorOut 2
#define buzzerPin 7

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
  makeSound(); // Make a sound
  
  delay(1000);
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

  if(frequencyR < frequencyG - 35 && frequencyR < frequencyB - 35) {Serial.println("RED");}
  if(frequencyG < frequencyR - 35 && frequencyG < frequencyB - 35) {Serial.println("GREEN");}
  if(frequencyB < frequencyG - 35 && frequencyB < frequencyR - 35) {Serial.println("BLUE");}

}

void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}

void makeSound() {
  tone(buzzerPin, 1000); // Play a tone of 1000Hz
  delay(100); // Wait for 100 milliseconds
  noTone(buzzerPin); // Stop the tone
}