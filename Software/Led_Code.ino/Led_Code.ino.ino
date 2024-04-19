int redPin= 1;
int greenPin = 2;
int bluePin = 3;

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void loop() {
  setColor(255, 0, 0); // Red Color
  delay(1000);
  
  setColor(0, 255, 0); // Green Color
  delay(1000);
  
  setColor(0, 0, 255); // Blue Color
  delay(1000);
  
  setColor(255, 255, 0); // Yellow Color
  delay(1000);

  setColor(0, 255, 255); // Cyan Color
  delay(1000);
  
  setColor(255, 0, 255); // Magenta Color
  delay(1000);
  
  setColor(255, 165, 0); // Orange Color
  delay(1000);
  
  setColor(128, 0, 128); // Purple Color
  delay(1000);
  
  setColor(255, 255, 255); // White Color
  delay(1000);
}

void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}