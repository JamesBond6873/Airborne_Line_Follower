/*
System state header file
*/

#ifndef my_sys_state_h
#define my_sys_state_h
#include <Arduino.h>

// ------ main types:

typedef struct {
  unsigned long sensTime;
  uint16_t sensorQTR[8];
  int sensorRGB[3];
  float sensorSonar;
} tSensor;

typedef struct {
  unsigned long actTime = 0;
  int m1Speed;
  int m2Speed;
} tAct;

// ------ circular arrays:

template<typename tSens, int maxLen> class tCircularArray {
  tSens list[maxLen];
  uint16_t ind = 0;
  bool full = false;

public:

  uint16_t numberOfElements() {
    if (full)
      return maxLen;
    else
      return ind;
  };

  void put(tSens x) {
    memcpy(&list[ind], &x, sizeof(tSens));
    ind += 1;
    if (ind >= maxLen) {
      ind = 0;
      full = true;
    }
  };

  void get(int n, tSens &y) {
    // for (int i=0; i<nElems; i++) get(i, sensVals);
    if (n < 0 || n >= maxLen) return;
    int ix = ind - n - 1;
    if (ix < 0) ix += maxLen;
    memcpy(&y, &list[ix], sizeof(tSens));
  };
};


// ------ System state, centralize IO data:

// list of possible work modes
#define m0_Stop 0
#define m1_Calib 1
#define m2_LineFollow 2
#define m3_GoBackToLine 3
#define m4_WithinWalls 4
#define m5_ExploreToFindLine 5

// length of data saving (circular) buffers
#define BUFF_LEN 200

// the main class doing the job
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

  // sensors data
  tCircularArray<tSensor, BUFF_LEN> buff1;
  tSensor sens;

  void sensorsDataStore(uint16_t qtrTimeValues[8],
                        int redValue, int greenValue, int blueValue,
                        float sonarDistance) {
    for (int i = 0; i < 8; i++) sens.sensorQTR[i] = qtrTimeValues[i];
    sens.sensorRGB[0] = redValue;
    sens.sensorRGB[1] = greenValue;
    sens.sensorRGB[2] = blueValue;
    sens.sensorSonar = sonarDistance;
    sens.sensTime = millis();
    buff1.put(sens);
  };
  String sens2str();

  // control params
  float KP = 65.0;  //Kp = 20.0
  float KI = 0.0;
  float KD = 170.0;

  // actuation data
  tCircularArray<tAct, BUFF_LEN> buff2;
  tAct act;

  void speedsDataStore(int newM1Speed, int newM2Speed) {
    act.m1Speed = newM1Speed;
    act.m2Speed = newM2Speed;
    act.actTime = millis();
    buff2.put(act);
  };
  String act2str();
};

// declare one object to use everywhere
extern MySysState ss;

// testing functions implemented in the .cpp :
void ss_loop_demo_0();
void ss_loop_demo_1();
void ss_loop_demo_2();

#endif  // my_sys_state_h
