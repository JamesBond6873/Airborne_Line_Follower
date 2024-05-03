
#ifndef my_sys_state_h
#define my_sys_state_h
#endif

#include <Arduino.h>
#include "my_buffer.h"

// list of possible work modes
#define m0_Stop 0
#define m1_Calib 1
#define m2_LineFollow 2
#define m3_GoBackToLine 3
#define m4_WithinWalls 4
#define m5_ExploreToFindLine 5

typedef struct {
  unsigned long sensTime;
  uint16_t sensorQTR[8];
  int sensorRGB[3];
  float sensorSonar;
} tSensors;

typedef struct {
  unsigned long actTime = 0;
  int m1Speed;
  int m2Speed;
} tAct;

class MySysState {
public:
  MySysState() = default;
  ~MySysState();

  int mode = m0_Stop;
  bool detectColorsFlag = 1, beepOnColorFlag = 1, waitOnColorFlag = 1;

  int blackThreshold = 550;
  void setBlackThreshold(int newBlackThreshold) {
    blackThreshold = newBlackThreshold;
  };

  /*
    uint16_t sensorQTR[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    int sensorRGB[3] = { 0, 0, 0 };
    float sensorSonar = 0.0;
    unsigned long sensTime = 0;

    void sensorsDataStore(uint16_t qtrTimeValues[8],
                          int redValue, int greenValue, int blueValue,
                          float sonarDistance) {
      for (int i = 0; i < 8; i++) sensorQTR[i] = qtrTimeValues[i];
      sensorRGB[0] = redValue;
      sensorRGB[1] = greenValue;
      sensorRGB[2] = blueValue;
      sensorSonar = sonarDistance;
      sensTime = millis();
    };
*/

  tSensors sens;

  void sensorsDataStore(uint16_t qtrTimeValues[8],
                        int redValue, int greenValue, int blueValue,
                        float sonarDistance) {
    for (int i = 0; i < 8; i++) sens.sensorQTR[i] = qtrTimeValues[i];
    sens.sensorRGB[0] = redValue;
    sens.sensorRGB[1] = greenValue;
    sens.sensorRGB[2] = blueValue;
    sens.sensorSonar = sonarDistance;
    sens.sensTime = millis();
  };

  float KP = 65.0;  //Kp = 20.0
  float KI = 0.0;
  float KD = 170.0;

  /*
    int m1Speed = 0, m2Speed = 0;
    unsigned long actTime = 0;

    void speedsDataStore(int newM1Speed, int newM2Speed) {
      m1Speed = newM1Speed;
      m2Speed = newM2Speed;
      actTime = millis();
    };
    */

  tAct act;

  void speedsDataStore(int newM1Speed, int newM2Speed) {
    act.m1Speed = newM1Speed;
    act.m2Speed = newM2Speed;
    act.actTime = millis();
  };

  tCircularArray<tSensors> buff1;
  tCircularArray<tAct> buff2;
};
