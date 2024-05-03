/*

Work in progress to help centralize:
- configuration options
- sensor readings

Given the centralized data:
- get a desired working mode

In the following [WIP] means Work In Progress
(in other words, work yet to be done)

*/

#include "my_sys_state.h"
//#include "my_print.h" // [WIP]

//MySysState ss;
//MyPrint pr; // [WIP]


void setup() {
  Serial.begin(115200);

  // [WIP] set config options:
  // ss.setBlackThreshold( blackThreshold );
  // ss.setOptions( "l0 s150 KP70 KD130 KI0" );
}


//void loop() { ss_loop_demo_0(); }
//void loop() { ss_loop_demo_1(); }
void loop() { ss_loop_demo_2(); }


void loopX() {
  // [WIP] read all sensors, and send the data to "ss"
  // ss.sensorsDataStore( qtrTimeValues, colorPinValues, sonarDistance );

  // [WIP] compute new state given the stored sensor data and config info
  // ss.computeMode();

  // use sensor values in the current state
  switch (ss.mode) {
    case m0_Stop:
      // do nothing - this was easy to implement ;)
      break;

    case m1_Calib:
      // yet to implement
      break;

    case m2_LineFollow:
      //loop0(); // current code following the line
      break;

    case m3_GoBackToLine:
      // yet to implement
      break;

    case m4_WithinWalls:
      // yet to implement
      // should enter here as soon as the distance sensor says something as "5cm < measurement < 20cm"
      break;

    case m5_ExploreToFindLine:
      // yet to implement
      break;

    default:
      // should show an error...
      break;
  }
}
