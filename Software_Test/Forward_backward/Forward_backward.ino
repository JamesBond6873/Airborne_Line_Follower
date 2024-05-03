#include <Arduino.h>
 
#define RF 18 // Right Motor Forward Pin
#define RB 19 // Right Motor Backward Pin
#define LF 20 // Left Motor Forward Pin
#define LB 21 // Left Motor Backward Pin


int m1Speed = 80; //Kp = 80
int m2Speed = 80; //Kp = 80

bool debug = false;

// unsigned long calib_ti, calib_tf; // time for calibration
unsigned long t0; // control sampling rate (period ini)
unsigned long t1; // control sampling rate (period end)
long timeInterval = 10; // 10ms per loop = 100Hz


// -----------------------------------------------------------------

void setSpeed(int m1Speed, int m2Speed, float speedReducer, bool debug) 
{
  if (debug == false)
  {
    if(m1Speed > 0) {analogWrite(LB, 0); analogWrite(LF, m1Speed * speedReducer);}
    else {analogWrite(LF, 0); analogWrite(LB, abs(m1Speed) * speedReducer);}

    if(m2Speed > 0) {analogWrite(RB, 0); analogWrite(RF, m2Speed * speedReducer);}
    else {analogWrite(RF, 0); analogWrite(RB, abs(m1Speed) * speedReducer);}
  }
}


// -----------------------------------------------------------------


void setup()
{
  Serial.begin(9600);


  pinMode(25, OUTPUT);
  pinMode(RF, OUTPUT); pinMode(RB, OUTPUT);
  pinMode(LF, OUTPUT); pinMode(LB, OUTPUT);

  // Set Stop motions
  digitalWrite(RF, LOW); digitalWrite(RB, LOW);
  digitalWrite(LF, LOW); digitalWrite(LB, LOW);
  digitalWrite(25, HIGH);

}

// Color Sensor flag to avoid multiple color readings for the same mark
int lastUsed = 50;

void loop()
{
  m1Speed = 80;
  m2Speed = 80;
  for (int i = 0; i < 2; i++) {
    setSpeed(m1Speed, m2Speed, 1, debug);
    delay(2000);

    setSpeed(-m1Speed, -m2Speed, 1, debug);
    delay(2000);
  }

  m1Speed = 120;
  m2Speed = 120;
  for (int i = 0; i < 2; i++) {
    setSpeed(m1Speed, m2Speed, 1, debug);
    delay(2000);

    setSpeed(-m1Speed, -m2Speed, 1, debug);
    delay(2000);
  }


  m1Speed = 180;
  m2Speed = 180;
  for (int i = 0; i < 2; i++) {
    setSpeed(m1Speed, m2Speed, 1, debug);
    delay(2000);

    setSpeed(-m1Speed, -m2Speed, 1, debug);
    delay(2000);
  }

  m1Speed = 220;
  m2Speed = 220;
  for (int i = 0; i < 2; i++) {
    setSpeed(m1Speed, m2Speed, 1, debug);
    delay(2000);

    setSpeed(-m1Speed, -m2Speed, 1, debug);
    delay(2000);
  }

  // set motor speeds using the two motor speed variables above
  Serial.print("Speeds = \t"); Serial.print(m1Speed); Serial.print("\t"); Serial.print(m2Speed); Serial.println();
}
