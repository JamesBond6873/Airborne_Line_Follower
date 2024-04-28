#include "QTRSensorsN.h"
#include <Arduino.h>
 
// create an object for your type of sensor (RC or Analog)
// in this example we have three sensors on analog inputs 0 - 2 (digital pins 14 - 16)
QTRSensors qtr;

#define RF 18 // Right Motor Forward Pin
#define RB 19 // Right Motor Backward Pin
#define LF 20 // Left Motor Forward Pin
#define LB 21 // Left Motor Backward Pin

int M1 = 130; //Kp = 80
int M2 = 130; //Kp = 80

float KP = 38.0; //Kp = 20.0
float KI = 0.0;
float KD = 4;

bool debug = true;

unsigned long t0;
unsigned long t1;
long timeInterval = 10; // 10ms per loop = 100Hz



// QTRSensorsA qtr((char[]) {0, 1, 2}, 3);
 
void setup()
{
  Serial.begin(9600);

  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){14, 13, 12, 11, 10, 9, 8, 7}, 8);
  qtr.setEmitterPin(15);

  pinMode(25, OUTPUT);
  pinMode(RF, OUTPUT);
  pinMode(RB, OUTPUT);
  pinMode(LF, OUTPUT);
  pinMode(LB, OUTPUT);

  // Set Stop motions
  digitalWrite(RF, LOW);
  digitalWrite(RB, LOW);
  digitalWrite(LF, LOW);
  digitalWrite(LB, LOW);
  digitalWrite(25, HIGH);


  // optional: wait for some input from the user, such as  a button press
 
  // then start calibration phase and move the sensors over both
  // reflectance extremes they will encounter in your application:
  int i;
  

  // Blink led for 1sec before Calibration
  delay(1000);
  digitalWrite(25, HIGH);
  delay(1000);
  digitalWrite(25, LOW);

  Serial.println("Starting");


  for (i = 0; i < 250; i++)  // make the calibration take about 5 seconds
  {
    qtr.calibrate();
    delay(20);
  }

    // Set Stop motions
  digitalWrite(RF, LOW);
  digitalWrite(RB, LOW);
  digitalWrite(LF, LOW);
  digitalWrite(LB, LOW);
  digitalWrite(25, HIGH);

  // Blink led multiple times after Calibration
  for (i = 0; i < 10; i++)
  {
    digitalWrite(25, HIGH);
    delay(60);
    digitalWrite(25, LOW);
    delay(40);
  }
  
  // optional: signal that the calibration phase is now over and wait for further
  // input from the user, such as a button press

  t0 = millis();
}


float calculatePosition(uint16_t sensorValues[]) 
{
  int blackThreshold = 500;
  float sensorDistance = 1.00;
  float linePos = 0;
  int LineTest[] = {};
  int counter = 0;  
  float final = 0;

  for (uint8_t i = 0; i < 8; i++)
  {
    if (sensorValues[i] >= blackThreshold) {
      counter ++;
      final = (i + 1) * sensorDistance;
      Serial.print("||||");
      Serial.print("\t");
    }
    else {
      Serial.print(sensorValues[i]);
      Serial.print('\t');
    }
  }
  Serial.print(final);
  Serial.print("\t");
  Serial.print(counter);
  Serial.print("\t");
  
  linePos = final - (sensorDistance / 2) * (counter - 1);
  Serial.print(linePos);
  Serial.print("\t");

  //eP = -(4.5 - linePos);
  //Serial.print(eP);
  //Serial.print("\t");
  //Serial.println();
  
  return linePos;
}



#if 1
int lastError = 0;
int counter = 0;

void loop()
{
  t1 = t0 + timeInterval;


  uint16_t sensors[8];
  // get calibrated sensor values returned in the sensors array, along with the line
  // position, which will range from 0 to 2000, with 1000 corresponding to the line over
  // the middle sensor
  //qtr.readLineBlack(sensors);
  qtr.readCalibrated(sensors);
  int position = calculatePosition(sensors);
 
  // compute our "error" from the line position.  We will make it so that the error is zero
  // when the middle sensor is over the line, because this is our goal.  Error will range
  // from -1000 to +1000.  If we have sensor 0 on the left and sensor 2 on the right,  
  // a reading of -1000 means that we see the line on the left and a reading of +1000 
  // means we see the line on the right.
  int error = position - 4.0;
  if (error <= -4.0) {
    error = lastError;
    counter ++;
    /*if (counter >= 10) {
      analogWrite(LF, 0);
      analogWrite(RF, 0); 
      delay(5000);
      counter = 0;    
    }
    analogWrite(LF, 0);
    analogWrite(RF, 0); 
    delay(250);*/
  }

/*
  if(error <= -4.0) {
    analogWrite(LF, 0);
    analogWrite(RF, 0); 
    delay(5000);
  }*/
 
  // set the motor speed based on proportional and derivative PID terms
  // KP is the a floating-point proportional constant (maybe start with a value around 0.1)
  // KD is the floating-point derivative constant (maybe start with a value around 5)
  // note that when doing PID, it's very important you get your signs right, or else the
  // control loop will be unstable
  int motorSpeed = KP * error + KD * (error - lastError);
  lastError = error;
 
  // M1 and M2 are base motor speeds.  That is to say, they are the speeds the motors
  // should spin at if you are perfectly on the line with no error.  If your motors are
  // well matched, M1 and M2 will be equal.  When you start testing your PID loop, it
  // might help to start with small values for M1 and M2.  You can then increase the speed
  // as you fine-tune your PID constants KP and KD.
  int m1Speed = M1 + motorSpeed;
  int m2Speed = M2 - motorSpeed;
 
  // it might help to keep the speeds positive (this is optional)
  // note that you might want to add a similiar line to keep the speeds from exceeding
  // any maximum allowed value
  if (m1Speed < 0) m1Speed = 0;
  if (m2Speed < 0) m2Speed = 0;
  if (m1Speed > 255) m1Speed = 255;
  if (m2Speed > 255) m2Speed = 255;
 
  // set motor speeds using the two motor speed variables above
  Serial.print("Speeds=");
  Serial.print(m1Speed);
  Serial.print("\t");
  Serial.print(m2Speed);
  Serial.println();

  if (debug == false)
  {
    analogWrite(LF, m1Speed);
    analogWrite(RF, m2Speed); 
  }
  delay(10);

    while(millis() <= t1) {
    delay(0.1);
  }
  t0 = t1;
}

#endif

