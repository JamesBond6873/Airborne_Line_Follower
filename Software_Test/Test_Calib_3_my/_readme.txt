
Study whether the PD (PID) has a nicely kept sampling period

The .ino has in "myloop1()" calls to:
  ss.sensorsDataStore(sensors, frequencyR, frequencyG, frequencyB, wallDistance);
  ss.speedsDataStore(m1Speed, m2Speed);
which are saved to a buffer with (max) 1000 entries of sensors and speeds

In the end the serial prints can be copied to the clipboard and ploted to check the time steps.

