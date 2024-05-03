// Do a circular buffer

#ifndef my_buffer_h
#define my_buffer_h
#endif

#include <Arduino.h>

#define ArrayLength 200

template<typename tSens> class tCircularArray {
  tSens list[ArrayLength];
  uint16_t ind = 0;
  bool full = false;

  uint16_t numberOfElements() {
    if (full)
      return ArrayLength;
    else
      return ind;
  };

  void add(tSens x) {
    memcpy(&list[ind], &x, sizeof(tSens));
    ind += 1;
    if (ind >= ArrayLength) {
      ind = 0;
      full = true;
    }
  };

  void get(int n, tSens &y) {
    // for (int i=0; i<nElems; i++) get(i, sensVals);
    if (n < 0 || n >= ArrayLength) return;
    int ix = ind - n;
    if (ix < 0) ix += ArrayLength;
    memcpy(&y, &list[ix], sizeof(tSens));
  };
};
