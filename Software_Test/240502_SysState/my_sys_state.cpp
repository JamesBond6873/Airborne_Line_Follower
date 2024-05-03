#include "my_sys_state.h"

void ss_loop_demo_1() {
  static int loopMode = 1;

  if (loopMode == 1 && millis() > 5000) {
    // do just once the exp, and do it after 5sec of reboot
    Serial.println("start exp");
    loopMode = 0;

    tCircularArray<int> arrayOfInt;
    for (int i = 0; i < 10; i++) {
      for (int j = 0; j < arrayOfInt.numberOfElements(); j++) {
        int x;
        arrayOfInt.get(j, x);
        Serial.print(x);
      }
      Serial.println();
      arrayOfInt.put(i);
    }

    Serial.println("\nend exp");
  }
}
