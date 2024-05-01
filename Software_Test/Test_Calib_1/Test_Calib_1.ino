#include "QTRSensorsN.h"
#include <Arduino.h>
 
// create an object for your type of sensor (RC or Analog)
// in this example we have three sensors on analog inputs 0 - 2 (digital pins 14 - 16)
QTRSensors qtr;

#define RF 18 // Right Motor Forward Pin
#define RB 19 // Right Motor Backward Pin
#define LF 20 // Left Motor Forward Pin
#define LB 21 // Left Motor Backward Pin

int M1 = 80; //Kp = 80
int M2 = 80; //Kp = 80

float KP = 65.0; //Kp = 20.0
float KI = 0;
float KD = 170;

bool debug = false;

// unsigned long calib_ti, calib_tf; // time for calibration
unsigned long t0; // control sampling rate (period ini)
unsigned long t1; // control sampling rate (period end)
long timeInterval = 10; // 10ms per loop = 100Hz


// -----------------------------------------------------------------

#if 0
float calculatePosition(uint16_t sensorValues[]) 
{
  int blackThreshold = 450;
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
#endif

#if 1
float calculatePosition(uint16_t sensorValues[]) 
{
  int blackThreshold = 550;
  float sensorDistance = 1.00;
  static float linePos = 0.0;
  int LineTest[] = {};
  int counter = 0;
  int acc = 0;
  float final = 0;

  for (uint8_t i = 0; i < 8; i++)
  {
    if (sensorValues[i] >= blackThreshold) {
      counter ++; acc += i + 1;
      Serial.print("||||");
      Serial.print("\t");
    }
    else {
      Serial.print(sensorValues[i]);
      Serial.print("\t");
    }
  }
  if (counter > 0) {
    linePos = (float) acc / counter;
  }
  Serial.print(linePos);
  Serial.print("\t");
  return linePos;
}
#endif

float getError(float &errorAcc) {
  uint16_t sensors[8];

  qtr.readCalibrated(sensors);
  int position = calculatePosition(sensors);

  int error = position - 4.5;

  errorAcc += error;

  /*if (abs(error) >= 3.5) {
    error = lastError;
    counter ++;
    if (counter >= 10) {
      analogWrite(LF, 0);
      analogWrite(RF, 0); 
      delay(5000);
      counter = 0;    
    }
    analogWrite(LF, 0);
    analogWrite(RF, 0); 
    delay(250);
  }


  if(error <= -4.0) {
    analogWrite(LF, 0);
    analogWrite(RF, 0); 
    delay(5000);
  }*/

  return error;
}

// -----------------------------------------------------------------

float lastError = 0; // Last Error
float errorAcc = 0; // Acumulated Error
int counter = 0;
int m1Speed;
int m2Speed;


void calcSpeed(float error, float lastError, float errorAcc, int &m1Speed, int &m2Speed) {
  // set the motor speed based on proportional and derivative PID terms
  // KP is the a floating-point proportional constant (maybe start with a value around 0.1)
  // KD is the floating-point derivative constant (maybe start with a value around 5)
  // note that when doing PID, it's very important you get your signs right, or else the
  // control loop will be unstable
  int motorSpeed = KP * error + KD * (error - lastError) + KI * errorAcc;

 
  // M1 and M2 are base motor speeds.  That is to say, they are the speeds the motors
  // should spin at if you are perfectly on the line with no error.  If your motors are
  // well matched, M1 and M2 will be equal.  When you start testing your PID loop, it
  // might help to start with small values for M1 and M2.  You can then increase the speed
  // as you fine-tune your PID constants KP and KD.
  m1Speed = M1 + motorSpeed;
  m2Speed = M2 - motorSpeed;
 
  // it might help to keep the speeds positive (this is optional)
  // note that you might want to add a similiar line to keep the speeds from exceeding
  // any maximum allowed value
  if (m1Speed < -255) m1Speed = -255;
  if (m2Speed < -255) m2Speed = -255;
  if (m1Speed > 255) m1Speed = 255;
  if (m2Speed > 255) m2Speed = 255;
}


void setSpeed(int m1Speed, int m2Speed, float speedReducer, bool debug) {
  if (debug == false)
  {
    if(m1Speed > 0) {analogWrite(LB, 0); analogWrite(LF, m1Speed * speedReducer);}
    else {analogWrite(LF, 0); analogWrite(LB, abs(m1Speed) * speedReducer);}

    if(m2Speed > 0) {analogWrite(RB, 0); analogWrite(RF, m2Speed * speedReducer);}
    else {analogWrite(RF, 0); analogWrite(RB, abs(m1Speed) * speedReducer);}
  }
}


// -----------------------------------------------------------------

void calibr()
{
  // float w; // to define speed
  int i;
  for (i = 0; i < 250; i++)  // make the calibration take about 5 seconds
  {
    qtr.calibrate();
    delay(20);
  }

}


void setup()
{
  Serial.begin(9600);

  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){14, 13, 12, 11, 10, 9, 8, 7}, 8);
  qtr.setEmitterPin(15);

  pinMode(25, OUTPUT);
  pinMode(RF, OUTPUT); pinMode(RB, OUTPUT);
  pinMode(LF, OUTPUT); pinMode(LB, OUTPUT);

  // Set Stop motions
  digitalWrite(RF, LOW); digitalWrite(RB, LOW);
  digitalWrite(LF, LOW); digitalWrite(LB, LOW);
  digitalWrite(25, HIGH);

  // optional: wait for some input from the user, such as  a button press

  // then start calibration phase and move the sensors over both
  // reflectance extremes they will encounter in your application:
  //int i;

  // Blink led for 1sec before Calibration
  delay(1000);
  digitalWrite(25, HIGH);
  delay(1000);
  digitalWrite(25, LOW);

  Serial.println("Starting");
  calibr();

    // Set Stop motions
  digitalWrite(RF, LOW);
  digitalWrite(RB, LOW);
  digitalWrite(LF, LOW);
  digitalWrite(LB, LOW);
  digitalWrite(25, HIGH);

  // Blink led multiple times after Calibration
  for (int i = 0; i < 10; i++)
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


void loop()
{
  t1 = t0 + timeInterval;
  float speedReducer = 1;

  int error = getError(errorAcc);
  if (abs(error) >= 2.0) {
    speedReducer = 0.7;
  }
  calcSpeed(error, lastError, errorAcc, m1Speed, m2Speed);
  setSpeed(m1Speed, m2Speed, speedReducer, debug);
  lastError = error;

  // set motor speeds using the two motor speed variables above
  Serial.print("Speeds = \t"); Serial.print(m1Speed); Serial.print("\t"); Serial.print(m2Speed); Serial.println();

  while(millis() <= t1) {
    delay(0.1);
  }
  t0 = t1;
}
