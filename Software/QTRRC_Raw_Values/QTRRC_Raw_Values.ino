#include "QTRSensorsN.h"

// This example is designed for use with eight RC QTR sensors. These
// reflectance sensors should be connected to digital pins 3 to 10. The
// sensors' emitter control pin (CTRL or LEDON) can optionally be connected to
// digital pin 2, or you can leave it disconnected and remove the call to
// setEmitterPin().
//
// The main loop of the example reads the raw sensor values (uncalibrated). You
// can test this by taping a piece of 3/4" black electrical tape to a piece of
// white paper and sliding the sensor across it. It prints the sensor values to
// the serial monitor as numbers from 0 (maximum reflectance) to 2500 (minimum
// reflectance; this is the default RC timeout, which can be changed with
// setTimeout()).

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
