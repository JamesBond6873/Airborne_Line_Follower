#include "QTRSensorsN.h"

#define RB 3
#define LB 1
#define RF 2
#define LF 0

int line[] = {};

QTRSensors qtr;

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];



void setup()
{
  pinMode(RB, OUTPUT);
  pinMode(LB, OUTPUT);
  pinMode(RF, OUTPUT);
  pinMode(LF, OUTPUT);

  // configure the Built In LED
  pinMode(25, OUTPUT);


  // Line Sensor
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){14, 13, 12, 11, 10, 9, 8, 7}, SensorCount);
  qtr.setEmitterPin(15);

  Serial.begin(9600);
}


void loop()
{
  digitalWrite(25, HIGH);

  // read raw sensor values
  qtr.read(sensorValues);
  // print the sensor values as numbers from 0 to 2500, where 0 means maximum
  // reflectance and 2500 means minimum reflectance
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
  Serial.println();

  //digitalWrite(25, LOW);
  delay(250);
  analogWrite(RB,105);
  analogWrite(LB, 255); 
  delay(10);
}

