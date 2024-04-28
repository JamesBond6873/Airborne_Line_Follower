#include "QTRSensorsN.h"

#define RF 18 // Right Motor Forward Pin
#define RB 19 // Right Motor Backward Pin
#define LF 20 // Left Motor Forward Pin
#define LB 21 // Left Motor Backward Pin


QTRSensors qtr;

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

// First Challenger Data

int blackThreshold = 1750;
int lineSize = 3;
float sensorDistance = 1;  // IR sensor distance from one to another
float half = 0.47625;

// Pid Vars
float eP; // Error Proportional
float kP; // Constant Proportional

float linePos = 0;
int LineTest[] = {};
int counter = 0;
float final = 0;

void setup()
{
  // configure the sensors
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

  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){14, 13, 12, 11, 10, 9, 8, 7}, SensorCount);
  qtr.setEmitterPin(15);

  Serial.begin(9600);
}


void loop()
{
  digitalWrite(25, HIGH);
  counter = 0;
  // read raw sensor values
  qtr.read(sensorValues);
  qtr.readLineBlack(sensorValues);

  // print the sensor values as numbers from 0 to 2500, where 0 means maximum
  // reflectance and 2500 means minimum reflectance
  for (uint8_t i = 0; i < SensorCount; i++)
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

  eP = -(4.5 - linePos);
  Serial.print(eP);
  Serial.print("\t");
  Serial.println();


  //digitalWrite(25, LOW);
  delay(250);

}
