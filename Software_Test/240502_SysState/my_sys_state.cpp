/*
System state implementation of longer functions
*/

#include "my_sys_state.h"

// the SysState object is declared here:
MySysState ss;

// ----------------------------------------------------------------------------------

String MySysState::sens2str() {
  String str = "t=" + String(sens.sensTime) + " Q=";
  for (int i = 0; i < 8; i++) str = str + " " + String(sens.sensorQTR[i]);
  str = str + " RGB=";
  for (int i = 0; i < 3; i++) str = str + " " + String(sens.sensorRGB[i]);
  str = str + " Dist=" + String(sens.sensorSonar, 2);
  return str;
}

String MySysState::act2str() {
  String str = "t=" + String(act.actTime) + " S= ";
  str = str + String(act.m1Speed) + " " + String(act.m2Speed);
  return str;
}


// ----------------------------------------------------------------------------------
// check ss is available
void ss_loop_demo_0() {
  static int loopMode = 1;

  if (loopMode == 1 && millis() > 5000) {
    // do just once the exp, and do it after 5sec of reboot
    loopMode = 0;
    Serial.print("Hello, loopMode=");
    Serial.println(loopMode);
  }
}


// ----------------------------------------------------------------------------------
// test saving and reading integer values
void ss_loop_demo_1() {
  static int loopMode = 1;

  if (loopMode == 1 && millis() > 5000) {
    // do just once the exp, and do it after 5sec of reboot
    Serial.println("start exp");
    loopMode = 0;

    // make a buffer of integers with length 5
    tCircularArray<int, 5> arrayOfInt;

    for (int i = 0; i < 10; i++) {
      // see what is in the buffer (should start empty)
      for (int j = 0; j < arrayOfInt.numberOfElements(); j++) {
        int x;
        arrayOfInt.get(j, x);
        Serial.print(x);
      }
      Serial.println();

      // put some info into the buffer
      arrayOfInt.put(i);
    }

    Serial.println("\nend exp");
  }
}


// ----------------------------------------------------------------------------------
// test saving and reading tSensor tAct values

void ss_loop_demo_2() {
  static int loopMode = 1;

  // do just once the exp, and do it after 5sec of reboot
  if (!loopMode || millis() < 5000) { return; }

  Serial.println("start exp");
  loopMode = 0;  // disable more runs

  // tSensor s1; tAct a1; // put patterns into s1 and a1, and push them to the buffer

  // ss is a global var containing: ss.sens, ss.act, ss.buff1, ss.buff2
  uint16_t qv[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  int rgb[3] = { 0, 0, 0 };
  float sonar = 0.0;
  int m1 = 0, m2 = 0;


  for (int i = 0; i < 10; i++) {
    Serial.print("--- i=");
    Serial.println(i);

    // see what is in the buffers (they should start empty)
    int nElems = ss.buff1.numberOfElements();
    if (nElems != ss.buff2.numberOfElements()) {
      Serial.print("diff nElems");
      if (nElems > ss.buff2.numberOfElements()) nElems = ss.buff2.numberOfElements();
    }

    for (int j = 0; j < nElems; j++) {
      ss.buff1.get(j, ss.sens);
      ss.buff2.get(j, ss.act);
      ss.showSensAct();
    }

    // put some info into the buffers
    qv[3] += 1;
    qv[4] += 2;
    rgb[1] += 1;
    rgb[2] += 2;
    rgb[3] += 3;
    sonar += 1.0;
    ss.sensorsDataStore(qv, rgb[0], rgb[1], rgb[2], sonar);
    m1 += 1;
    m2 += 2;
    ss.speedsDataStore(m1, m2);
  }

  Serial.println("\nend exp");
}
