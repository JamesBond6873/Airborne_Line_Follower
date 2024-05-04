#include "QTRSensorsN.h"
#include <Arduino.h>
#include "my_sys_state.h"

// create an object for your type of sensor (RC or Analog)
// in this example we have three sensors on analog inputs 0 - 2 (digital pins 14 - 16)
QTRSensors qtr;
uint16_t sensors[8];

#define RF 18  // Right Motor Forward Pin
#define RB 19  // Right Motor Backward Pin
#define LF 20  // Left Motor Forward Pin
#define LB 21  // Left Motor Backward Pin

#define S0 3
#define S1 4
#define S2 1
#define S3 0
#define sensorOut 2
#define buzzerPin 5

int level = 3;

int redPin = 6;
int greenPin = 17;
int bluePin = 16;

int frequencyR;
int frequencyG;
int frequencyB;

const int EchoPin = 26;
const int TriggerPin = 27;
float wallDistance;

int M1 = 180;  //Kp = 80
int M2 = 180;  //Kp = 80

int goBackTime = 100;    // time to go back after color detection (ms)
int goBackSpeed = -125;  // speed to go back after color detection (negative values only)

float KP = 100.0;  //Kp = 20.0
float KI = 0;
float KD = 230.0;

bool debug = false;
bool insideLevel3 = false;
float linePosLast = 0.0;

// unsigned long calib_ti, calib_tf; // time for calibration
bool t0undefined = true;
unsigned long t0;        // control sampling rate (period ini)
unsigned long t1;        // control sampling rate (period end)
long timeInterval = 10;  // 10ms per loop = 100Hz

int counterLevel3 = 0;  // Counter for 3rd level


// -----------------------------------------------------------------
// working macros, do really send data through the Serial
#if 0
#define myserialprint(str) Serial.print(str)
#define myserialprintln(str) Serial.println(str)
#else
// empty macros, cut serial print and save from its time delays
#define myserialprint(str)
#define myserialprintln(str)
#endif


// -----------------------------------------------------------------
void checkColor(int &lastUsed) {
  if (lastUsed < 25) {
    return;
  }
  // RED
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  frequencyR = pulseIn(sensorOut, LOW, 1500);  // Timeout 1500 added
  //Serial.print("R= ");                         //printing name
  //Serial.print(frequencyR);                    //printing RED color frequency
  //Serial.print("\t");
  delayMicroseconds(50);

  //GREEN
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  frequencyG = pulseIn(sensorOut, LOW, 1500);  // Timeout 1500 added
  //Serial.print("G= ");                         //printing name
  //Serial.print(frequencyG);                    //printing Green color frequency
  //Serial.print("\t");
  delayMicroseconds(50);

  //BLUE
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  frequencyB = pulseIn(sensorOut, LOW, 1500);  // Timeout 1500 added
  //Serial.print("B= ");                         //printing name
  //Serial.print(frequencyB);                    //printing Blue color frequency
  //Serial.print("\t");

  if (frequencyR < frequencyG - 35 && frequencyR < frequencyB - 35) {  //Important Change: Red detection OFF
    //Serial.print("RED");
    //Serial.print("\t");
    lastUsed = 0;
    setSpeed(0, 0, 1, debug);
    delay(0.05);
    setSpeed(goBackSpeed, goBackSpeed, 1, debug);
    delay(goBackTime);
    setSpeed(0, 0, 1, debug);
    setColor(255, 0, 0);
    makeSound();
  } else if (frequencyG < frequencyR - 20 && frequencyG < frequencyB - 20) {
    //Serial.print("GREEN");
    //Serial.print("\t");
    lastUsed = 0;
    setSpeed(0, 0, 1, debug);
    delay(0.05);
    setSpeed(goBackSpeed, goBackSpeed, 1, debug);
    delay(goBackTime);
    setSpeed(0, 0, 1, debug);
    setColor(0, 255, 0);
    makeSound();
  } else if (frequencyB < frequencyG - 40 && frequencyB < frequencyR - 40) {
    //Serial.print("BLUE");
    //Serial.print("\t");
    lastUsed = 0;
    setSpeed(0, 0, 1, debug);
    delay(0.05);
    setSpeed(goBackSpeed, goBackSpeed, 1, debug);
    delay(goBackTime);
    setSpeed(0, 0, 1, debug);
    setColor(0, 0, 255);
    makeSound();
  } else {
    //Serial.print("\t");
  }
  setColor(0, 0, 0);  // LED OFF
  return;
}


void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, 255 - redValue);
  analogWrite(greenPin, 255 - greenValue);
  analogWrite(bluePin, 255 - blueValue);
}

void makeSound() {
  int time0 = millis();
  /*while (millis() < (time0 + 5000)){
    tone(buzzerPin, 1000); // Send 1KHz sound signal...
    delay(50);        // ...for 1 sec
    noTone(buzzerPin);     // Stop sound...
    digitalWrite(buzzerPin, LOW);
    delay(100);        // ...for 1sec
  }*/

  while (millis() < (time0 + 5000 - goBackTime)) {
    //tone(buzzerPin, 2000); // Send 1KHz sound signal...
    digitalWrite(buzzerPin, HIGH);
    delay(500);         // ...for 1 sec
    noTone(buzzerPin);  // Stop sound...
    digitalWrite(buzzerPin, LOW);
    delay(100);  // ...for 1sec
  }
}

float measureDistance(int TriggerPin, int EchoPin) {
  float duration, distanceCm;
  digitalWrite(TriggerPin, LOW);
  delayMicroseconds(4);
  digitalWrite(TriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);
  duration = pulseIn(EchoPin, HIGH, 4000);  // Timeout = 1.500 ms -- Max distance = 25.5cm
  distanceCm = duration * 10 / 340 / 2;

  return distanceCm;
}

float calcPosUltrassonicSensor(float wallDistance) {
  float centerDistance = wallDistance + 3;
  float linePos = (4.5 * centerDistance) / 15.0;

  return linePos;
}


float calculatePosition(uint16_t sensorValues[]) {
  int blackThreshold = 550;
  float sensorDistance = 1.00;
  static float linePos = 0.0;
  int LineTest[] = {};
  int counter = 0;
  int acc = 0;
  float final = 0;

  for (uint8_t i = 0; i < 8; i++) {
    // try to get line imaging
    if (sensorValues[i] >= blackThreshold) {
      counter++;
      acc += i + 1;
      //Serial.print("||||"); Serial.print("\t");
      myserialprint("*" + String(sensorValues[i]) + "* ");
    } else {
      //Serial.print(sensorValues[i]); Serial.print("\t");
      myserialprint(String(sensorValues[i]) + " ");
    }
  }
  myserialprint("Cnt=" + String(counter) + " ");

  // given the line imaging or the wall distance, find a pose to be
  if (counter > 0) {
    // if the lane is found, than forget checking more inputs
    insideLevel3 = false;
    linePos = (float)acc / counter;
    counterLevel3 = 0;
  } else if (level == 3 && counterLevel3 >= 25) {
    // ok, line NOT available, are there walls to survive?
    insideLevel3 = true;
    //float wallDistance = measureDistance(TriggerPin, EchoPin);
    wallDistance = measureDistance(TriggerPin, EchoPin);
    //Serial.print("Distance:"); Serial.print(wallDistance);
    myserialprint("Dist=" + String(wallDistance) + " ");
    if (wallDistance < 25) {
      int linePos2 = calcPosUltrassonicSensor(wallDistance);
      if (linePos2 > 8) {
        myserialprint("Error, Error, Error");
      } else {
        linePos = linePos2;
      }
      //Serial.print("U=");
    }
  }
  //Serial.print(linePos); Serial.print("\t");
  myserialprint("LinesPos=" + String(linePos) + " ");

  return linePos;
}


float getError(float &errorAcc) {
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

float lastError = 0;  // Last Error
float errorAcc = 0;   // Acumulated Error
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
  if (debug == false) {
    if (m1Speed > 0) {
      analogWrite(LB, 0);
      analogWrite(LF, m1Speed * speedReducer);
    } else {
      analogWrite(LF, 0);
      analogWrite(LB, abs(m1Speed) * speedReducer);
    }

    if (m2Speed > 0) {
      analogWrite(RB, 0);
      analogWrite(RF, m2Speed * speedReducer);
    } else {
      analogWrite(RF, 0);
      analogWrite(RB, abs(m1Speed) * speedReducer);
    }
  }
}


// -----------------------------------------------------------------

void calibr() {
  // float w; // to define speed
  int i;
  for (i = 0; i < 250; i++)  // make the calibration take about 5 seconds
  {
    qtr.calibrate();
    delay(20);
  }
}


// -----------------------------------------------------------------
void show_digital_pins_wait_on_GP7() {
  for (int i = 0; i <= 22; i++) { pinMode(i, INPUT); }
  while (!digitalRead(7)) {
    // wait here
    for (int i = 0; i <= 22; i++) {
      if (digitalRead(i)) Serial.print("1 ");
      else Serial.print("0 ");
    }
    Serial.println("\twait short GP7 to 3.3v");
    delay(1000);
  }
}


void wait_on_pin(int pinNumber, bool pinState) {
  pinMode(pinNumber, INPUT);
  while (digitalRead(pinNumber) != pinState) {
    Serial.println("wait on pin " + String(pinNumber) + " to be " + String(pinState));
    delay(1000);
  }
}


void setup() {

  Serial.begin(115200);

  //Serial.println("Start1 ini..."); pinMode(7, INPUT); show_digital_pins_wait_on_GP7(); Serial.println("Start1 concluded.");
  wait_on_pin(7, true);

  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){ 14, 13, 12, 11, 10, 9, 8, 7 }, 8);
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

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  pinMode(buzzerPin, OUTPUT);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);


  //Ultrassonic Sensor
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);

  // Setting frequency-scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  setColor(255, 0, 0);

  // Blink led for 1sec before Calibration
  delay(1000);
  digitalWrite(25, HIGH);
  delay(1000);
  digitalWrite(25, LOW);

  setColor(255, 255, 0);

  Serial.println("Starting");
  calibr();

  // Set Stop motions
  digitalWrite(RF, LOW);
  digitalWrite(RB, LOW);
  digitalWrite(LF, LOW);
  digitalWrite(LB, LOW);
  digitalWrite(25, HIGH);

  setColor(0, 255, 0);

  // Blink led multiple times after Calibration
  for (int i = 0; i < 10; i++) {
    digitalWrite(25, HIGH);
    delay(60);
    digitalWrite(25, LOW);
    delay(40);
  }

  setColor(0, 0, 0);
  // optional: signal that the calibration phase is now over and wait for further
  // input from the user, such as a button press

  //t0 = millis(); // to replace by the flag t0undefined
}


// -----------------------------------------------------------------
int lastUsed = 50;  // Color Sensor flag to avoid multiple color readings for the same mark

void myloop1() {

  // define the end-time of this function (so that it runs in similar times forever)
  if (t0undefined) {
    t0 = millis();
    t0undefined = false;
  }
  t1 = t0 + timeInterval;
  float speedReducer = 1;

  lastUsed += 1;
  counterLevel3 += 1;

  checkColor(lastUsed);

  int error = getError(errorAcc);
  ss.sensorsDataStore(sensors, frequencyR, frequencyG, frequencyB, wallDistance);
  if (abs(error) >= 2.0) { speedReducer = 0.7; }
  if (insideLevel3 == true) { speedReducer = 0.5; }

  calcSpeed(error, lastError, errorAcc, m1Speed, m2Speed);
  setSpeed(m1Speed, m2Speed, speedReducer, debug);
  lastError = error;

  // set motor speeds using the two motor speed variables above
  // Serial.print("Speeds = \t"); Serial.print(m1Speed); Serial.print("\t"); Serial.print(m2Speed); Serial.println();
  myserialprintln(" Speed=(" + String(m1Speed) + "," + String(m2Speed) + ")");

  // sensors & actuator debug
  ss.speedsDataStore(m1Speed, m2Speed);

  while (millis() <= t1) {
    delay(0.1);
  }
  t0 = t1;
}


void myloop2() {
  // show the buffers
  Serial.println("myloop2");
  for (int j = 0; j < ss.buff1.numberOfElements(); j++) {
    ss.buff1.get(j, ss.sens);
    ss.buff2.get(j, ss.act);
    ss.showSensAct();
  }
  delay(500);
}


void myloop3() {
  Serial.println("myloop3");
  delay(10000);
}


int loopMode = 1;
int loopCount = 0;

void loop() {
  wait_on_pin(7, false);

  //while (1) {};
  //if (millis() < 10000) {
  //if (loopCount++ < 100) {
  if (loopCount++ < 1000) {
    // acquire data into the buffers
    myserialprint("loopCnt=" + String(loopCount) + " ");
    myloop1();
  } else if (loopMode == 1) {
    // run once loop, buffers show
    loopMode = 2;
    myloop2();
  } else {
    // slow myloop
    myloop3();
  }
}
