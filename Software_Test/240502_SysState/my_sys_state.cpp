#include "my_sys_state.h"

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
