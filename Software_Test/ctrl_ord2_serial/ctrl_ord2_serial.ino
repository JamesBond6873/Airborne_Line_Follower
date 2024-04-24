/*
  Analog input, analog output, serial output
  
  Note: choose monitor to terminate lines e.g. with "newline",
  as otherwise string reading needs to terminate by timeouts (multiples of 10ms)

  Oct2022 (WeMos D1 R2 ver) J. Gaspar
  Nov2023 (set data buffer entries, WeMos D1 R1 ver) J. Gaspar
*/

// --------------------------------------------------

// Main buffer for ADC/DAC data (see later buffer for USB comms)
// #define S_BUFF_SZ 500
#define S_BUFF_SZ 1000
int sensorValuesArray[S_BUFF_SZ];
unsigned long timeArray[S_BUFF_SZ];

// buffer to receive commands from the serial interf
#define BUFSZ 100
char buf[BUFSZ];
// buffer to help preparing strings to send through the serial interf
#define BUFSZ2 100
char buf2[BUFSZ2];

// variables for an interrupt service routine
volatile int ISR_iniFlag, ISR_wasON, ISR_isON, ISR_cnt, ISR_endFlag;
volatile int buffInd= 0;

unsigned long t0, t1;
int n;

// The D0..D8 are used to give names
int myDigitalPin( int pinId )
{
	switch (pinId)
	{
		case 0: return D0;
		case 1: return D1;
		case 2: return D2;
		case 3: return D3;
		case 4: return D4;
		case 5: return D5;
		case 6: return D6;
		case 7: return D7;
		case 8: return D8;
    // case 9: return D9;   // D9 undeclared
    // case 10: return D10; // D10 undeclared
	}
	return pinId; //D0; // make a default
}


void serial_print_data_n() {
  //Serial.print("nLines=100 ti[us]="); // going to send 100 lines
  Serial.print("nLines=");
  Serial.print(S_BUFF_SZ/2); // going to send S_BUFF_SZ/2 lines
  Serial.print(" ti[us]=");
  Serial.print(t0);
  Serial.println(" (n v1 v2 t1 t2):");

  for (n=0; n<S_BUFF_SZ; n+=2) {
    Serial.print(n>>1); // n=0,2,4,6,... divided by two displays 0,1,2,3,...
    Serial.print(" ");
    Serial.print(sensorValuesArray[n]);
    Serial.print(" ");
    Serial.print(sensorValuesArray[n+1]);
    Serial.print(" ");
    Serial.print(timeArray[n]);
    Serial.print(" ");
    Serial.println(timeArray[n+1]);
  }
}


void serial_print_data_1(int addr) {
  Serial.println("addr v1 t1:");
  Serial.print(addr);
  Serial.print(" ");
  Serial.print(sensorValuesArray[addr]);
  Serial.print(" ");
  Serial.println(timeArray[addr]);
}


void set_data(int addr, int ctrl_ref)
{
  // usage: set_data(atoi(&buf[1]), ctrl_ref);
  // int ctrl_ref=123
  if (addr<0 || S_BUFF_SZ<=addr) {
    Serial.print("E buf addr: ");
    Serial.println(addr);
    return;
  }
  sensorValuesArray[addr]= ctrl_ref;
}


int serial_read_str(char *buf, int buflen)
{
/* read till the end of the buffer or a terminating chr
*/
  int i, c;
  if (Serial.available() <= 0) {
    return 0;
  }
  for (i=0; i<buflen-1; i++) {
    c= Serial.read();
    if (c==0 || c==0x0A || c==0x0D) break;
    //buf[i]= c;
    *buf++= c;
    //Serial.print((char)c);
    if (Serial.available() <= 0) {
      delay(10); // 10milliseconds = 96bits / 9600bits/sec
      if (Serial.available() <= 0) break;
    }
  }
  //buf[i]= '\0';
  *buf='\0';
  return 1;
}


void set_data_array(char *str, int value)
{
  // two cases: use ref (and repeat it), use concatenated values
  int setDataMode= 0;
  if (str[0]=='a')
    setDataMode= 1;
  else if (str[0]=='x')
    setDataMode= 2;
  else {
    Serial.println("Err array set, 1st chr not a nor x");
    return;
  }

  int a1, a2, a3;
  char *endptr;
  a1= strtol(&str[1], &endptr, 10);
  a2= strtol(endptr+1, &endptr, 10);
  a3= strtol(endptr+1, &endptr, 10);

  if (setDataMode==1) {
    for (int i=a1; i<=a3; i+=a2)
        if (0<=i && i<S_BUFF_SZ)
          sensorValuesArray[i]= value;
  } else if (setDataMode==1) {
      for (int i=a1; i<=a3; i+=a2)
        if (0<=i && i<S_BUFF_SZ)
          sensorValuesArray[i]= strtol(endptr+1, &endptr, 10);
  }
}


// --------------------------------------------------
void pwm_config(int freqId) {
  // PWM pins 6 & 5, 9 & 10 or 11 & 3 are controlled by TCCR0B, TCCR1B or TCCR2B
  // freqId 1..5 <=> 32kHz, 4kHz, 500Hz, 125Hz, 31.25Hz
  if (freqId<1 || freqId>5) {
    //Serial.print("ERR: invalid pwm freqId - not in 1..5");
    //Serial.println("E pwm cf");
    Serial.println("E pwm cf");
    return;
  }

#if 0
  int prescalerVal= 0x07; // clear the last 3bits mask
  TCCR1B &= ~prescalerVal; // configuring pins 9 and 10
  prescalerVal= freqId;
  TCCR1B |= prescalerVal;
#else
  // config of PWM NA / not ok for WeMos D1
  Serial.println("W pwm NA");
#endif
}


/* Use global vars to decide pin (pins) to output
*/

//const int analogInPin = 0;  // Analog input pin that the potentiometer is attached to

// int myAnalogOutPin = 9; // Analog output pin that the LED is attached to
int myAnalogOutPin   =  7; // default analog output pin
int myAnalogOutPin2  = -1; // power driver receiving PWM from two pins
int myAnalogOutNegPin= -1; // power driver with forward/backwards pin

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)

int myAnalogOutput(int ctrl_u)
{
  int pin;
  if (myAnalogOutNegPin==-1 && myAnalogOutPin2==-1) {
    // old way, enforce ctrl_u in 0:255
    if (ctrl_u > 255) ctrl_u=255;
    if (ctrl_u < 0) ctrl_u=0;
	pin= myDigitalPin(myAnalogOutPin);
    analogWrite(pin, ctrl_u);
    return ctrl_u;
  }

  if (myAnalogOutPin2!=-1) {
    // power driver receives PWM at two pins
    if (ctrl_u > 255) ctrl_u=255;
    if (ctrl_u < -255) ctrl_u=-255;
    if (ctrl_u>=0) {
      // move motor forward
	  pin= myDigitalPin(myAnalogOutPin);
      analogWrite(pin, ctrl_u);
	  pin= myDigitalPin(myAnalogOutPin2);
      analogWrite(pin, 0);
    }
    else {
      // move motor backwards
	  pin= myDigitalPin(myAnalogOutPin);
      analogWrite(pin, 0);
	  pin= myDigitalPin(myAnalogOutPin2);
      analogWrite(pin, abs(ctrl_u));
    }
    return ctrl_u;
  }
  else {
    // power driver uses a forward/backwards pin
    // enforce ctrl_u in -255:255
    int uSign= (ctrl_u<0);
    int uAbs = abs(ctrl_u);
    if (uAbs > 255) uAbs=255;
    if (uAbs < 0) uAbs=0;
	pin= myDigitalPin(myAnalogOutNegPin);
    digitalWrite(pin, uSign);
	pin= myDigitalPin(myAnalogOutPin);
    analogWrite(pin, uAbs);
    if (uSign)
      {return -uAbs;}
    else
      {return uAbs;}
  }
}


// --------------------------------------------------
//ICACHE_RAM_ATTR void ISR_event_times() {
IRAM_ATTR void ISR_event_times() {
  // interrupt routine to monitor events
  ISR_isON= 1;
  ISR_cnt++;
  if (buffInd < S_BUFF_SZ)
    timeArray[buffInd++]= micros();
  else
    ISR_endFlag=1;
}


// --------------------------------------------------
int ctrl_ref=123, ctrl_e, ctrl_u, ctrl_y;
int ctrl_u_eq=0, ctrl_y_eq=0;
int ctrl_e1, ctrl_e2, ctrl_u1, ctrl_u2;
double a0=2.0, a1=0.0, a2=0.0, b1=0.0, b2=0.0;
int Ts= 1000; // 1 msec
int tmpInt, loopMode= 0, loopOutputFlag= 1;
int ctrl_verbose_flag= 0; //1;


void step_response() {
  // get a step response
  //  ctrl_ref= 127;
  int loopMode5output= 0;
  int inputA0;
  
  //Serial.println("-- Entering the open loop acq.");
  //pwm_config(1); // NA for WeMos

  //   set DAC
  if (loopOutputFlag && loopMode!=5)
    myAnalogOutput(ctrl_ref);

  // timed loop:
  t0= micros(); t1= t0+Ts; // 1 msec default Ts

  // ISR based loop
  if (loopMode==9)
    ISR_iniFlag= 1;

  //for (n=0; n<200; t1= t1+Ts) {
  for (n=0; n<S_BUFF_SZ; t1= t1+Ts) {

    yield(); // reset ESP8266 watch dog

    if (loopMode<6 || loopMode>8) {
      // for all loop modes (except 6..8) do constant sampling
      // while 1, if time
      while (micros()<t1)
        // loop timing, do nothing
        ;
    }

    //   read sensors
    switch (loopMode) {
      case 0:
        // read two channels: A0, A1
        sensorValuesArray[n]= analogRead(0);
        timeArray[n]= micros();
        sensorValuesArray[n+1]= analogRead(1); // this is not available in WeMos
        timeArray[n+1]= micros();
        n+=2;
        break;

      case 1:
        // read one analog channel: A1
        n++;
        sensorValuesArray[n]= analogRead(1); // this is not available in WeMos
        timeArray[n]= micros();
        n++;
        break;

      case 2:
        // read one digital channel: D8 
        n++;
        // sensorValuesArray[n]= digitalRead( myDigitalPin(15) ); // 15==D8
        sensorValuesArray[n]= digitalRead( myDigitalPin(8) ); // 15==D8
        timeArray[n]= micros();
        n++;
        break;

      case 3:
        // read analog channel A0 (ret Nx5: col1=ind col2+4=unused col3=A0 col5=time)
        n++;
        sensorValuesArray[n]= analogRead(0);
        timeArray[n]= micros();
        n++;
        break;

      case 30:
        // write a specific ref and read the state
        // read analog channel A0 (ret Nx5: col1=ind col3=A0 col5=time)
        // col2 has a reference value, col4 gets the time of its usage
        n++;
        sensorValuesArray[n]= analogRead(0); // input to the array
        timeArray[n]= micros();
        myAnalogOutput(sensorValuesArray[n-1]); // output given the array
        timeArray[n-1]= micros();
        n++;
        break;


      case 31:
        // read analog channel A0 (ret Nx5: col1=ind col2+3=A0 col4+5=time)
        sensorValuesArray[n]= analogRead(0);
        timeArray[n]= micros();
        n++;
        break;

      case 32:
        // report only input changes (assume range 0..1023)
        // save Analog0
        inputA0= analogRead(0);
        if (n==0 ||
            ( sensorValuesArray[n-1]<500 && inputA0>=500 ) ||
            ( sensorValuesArray[n-1]>500 && inputA0<=500 ) ) {
          // found a difference, save the data
          sensorValuesArray[n]= inputA0;
          timeArray[n]= micros();
          n++;
        }
        if (micros() > t0+5*1000000)
          n= S_BUFF_SZ;
        break;

      case 33:
        // report only input delta changes (assume range 0..1023)
        // save Analog0
        inputA0= analogRead(0);
        if (n==0 || abs(inputA0 -sensorValuesArray[n-1])>10 ) {
          // ADC has values 0..1023, found a difference, save the data
          // The threshold "10" should be adapted to the typical noise
          sensorValuesArray[n]= inputA0;
          timeArray[n]= micros();
          n++;
        }
        if (micros() > t0+5*1000000)
          n= S_BUFF_SZ;
        break;

      case 4:
        // read two channels A0 and D0
        sensorValuesArray[n]= analogRead(0);
        timeArray[n]= micros();
        sensorValuesArray[n+1]= digitalRead( myDigitalPin(0) );
        timeArray[n+1]= micros();
        n+=2;
        break;

      case 5:
        // set output only after 20% of the time
        if (loopOutputFlag)
          if (n>=S_BUFF_SZ/5 && !loopMode5output) {
            myAnalogOutput(ctrl_ref);
            loopMode5output= 1;
          }
        sensorValuesArray[n]= analogRead(0);
        timeArray[n]= micros();
        sensorValuesArray[n+1]= digitalRead( myDigitalPin(0) );
        timeArray[n+1]= micros();
        n+=2;
        break;

      case 6:
        // report only input changes (assume range 0..1023)
        // save Analog0 and Digital0
        inputA0= analogRead(0);
        if (n==0 ||
            ( sensorValuesArray[n-2]<500 && inputA0>=500 ) ||
            ( sensorValuesArray[n-2]>500 && inputA0<=500 ) ) {
          // found a difference, save the data
          sensorValuesArray[n]= inputA0;
          timeArray[n]= micros();
          sensorValuesArray[n+1]= digitalRead( myDigitalPin(0) );
          timeArray[n+1]= micros();
          n+=2;
        }
        break;

      case 7:
        // report at digital input changes
        inputA0= digitalRead( myDigitalPin(0) );
        if (n==0 ||
            ( sensorValuesArray[n-1]==0 && inputA0==1 ) ||
            ( sensorValuesArray[n-1]==1 && inputA0==0 ) ) {
          // found a difference, save the data
          sensorValuesArray[n]= analogRead(0);
          timeArray[n]= micros();
          sensorValuesArray[n+1]= inputA0;
          timeArray[n+1]= micros();
          n+=2;
        }
        break;

      case 8:
        // report at digital input changes
        inputA0= digitalRead( myDigitalPin(1) );
        if (n==0 ||
            ( sensorValuesArray[n-1]==0 && inputA0==1 ) ||
            ( sensorValuesArray[n-1]==1 && inputA0==0 ) ) {
          // found a difference, save the data
          sensorValuesArray[n+1]= inputA0; // D1
          timeArray[n+1]= micros();
          sensorValuesArray[n]= digitalRead( myDigitalPin(0) );
          timeArray[n]= micros();
          n+=2;
        }
        break;

      case 9:
        // use a interrupt service routine (ISR)
        // start ISR, iff not done before [ISRStartFlag]
        // attachInterrupt(digitalPinToInterrupt(D0), ISR, mode) 
        // check ISR has done all what is to do [ISRStopFlag]
        // detachInterrupt(digitalPinToInterrupt(D0))
        // aux fns (to avoid):   noInterrupts(); /* code not interruptable */ interrupts();
        if (ISR_iniFlag) {
          ISR_iniFlag= 0;
          ISR_endFlag= 0;
          ISR_wasON= 0;
          ISR_isON= 0;
          ISR_cnt= 0;
          buffInd= 0;
          //Serial.print("ISR going to be attached to pin: ");
          //Serial.println(digitalPinToInterrupt(D1));
          //attachInterrupt(digitalPinToInterrupt(D1), ISR_event_times, CHANGE);
          //attachInterrupt(digitalPinToInterrupt(D1), ISR_event_times, RISING);
          attachInterrupt(digitalPinToInterrupt(D2), ISR_event_times, RISING);
        }
        if (ISR_endFlag) {
          //detachInterrupt(digitalPinToInterrupt(D1));
          detachInterrupt(digitalPinToInterrupt(D2));
          //Serial.println("ISR detached.");
          ISR_endFlag= 0;
        }
        if ( n != buffInd ) {
          // keep track of the buffer filling
          n= buffInd;
          //Serial.print("n="); Serial.println(n);
        }
        /* if (ISR_isON && ~ISR_wasON) {
          Serial.println("ISR_isON && ~ISR_wasON");
          ISR_wasON= 1;
        } */
        /* if (ISR_cnt>0 && (ISR_cnt%100)==0)
          { Serial.print("ISR_cnt="); Serial.println(ISR_cnt); } */
        /* if (digitalRead(D2)) {
          Serial.println("loop break");
          n= S_BUFF_SZ;
        } */

        // if t0 is not updated force stop after 1000000usec = 1e6 usec= 1sec
        if (micros() > t0+1000000)
          n= S_BUFF_SZ;
        break;

      default:
        // invalid mode
        n+=2;
        Serial.println("err inv loopMode");
      
    }
  }

  // stop the motor
  if (loopOutputFlag)
    myAnalogOutput(0);

  //Serial.println("-- Open loop acq done.");
}


// --------------------------------------------------
void ctrl_finite_loop() {
//  ctrl_ref= 127;

  //pwm_config(1); // NA for WeMos

  // ctrl loop:
  t0= micros(); t1= t0+Ts; // 1 msec

  for (n=0; n<200; n+=2, t1= t1+Ts) {

    yield(); // reset ESP8266 watch dog

    // while 1, if time
    while (micros()<t1)
      // do nothing
      ;

    //   read sensors
    //sensorValuesArray[n]= analogRead(0);
    timeArray[n]= micros();
    ctrl_y= analogRead(1);
    sensorValuesArray[n+1]= ctrl_y;
    timeArray[n+1]= micros();

    //   calc ctrl
    ctrl_e= ctrl_ref - ctrl_y;
    ctrl_u= a0*ctrl_e +a1*ctrl_e1 +a2*ctrl_e2
                      -b1*ctrl_u1 -b2*ctrl_u2;
    sensorValuesArray[n]= ctrl_u; // save values before truncation
    ctrl_u= myAnalogOutput(ctrl_u);

    ctrl_u2= ctrl_u1;
    ctrl_u1= ctrl_u;
    ctrl_e2= ctrl_e1;
    ctrl_e1= ctrl_e;
    
    //   read cmd
    //   write cmd
    //   if break loop then break
  }

  // stop the motor
  myAnalogOutput(0);
}


// --------------------------------------------------
void ctrl_infinite_loop() {

  int c;
  //pwm_config(1); // NA for WeMos

  // ctrl loop:
  t0= micros(); t1= t0+Ts; // 1 msec

  for (n=0; n<200; n+=2, t1= t1+Ts) {

    yield(); // reset ESP8266 watch dog

    if (n>=198)
      n= 0; // make an infinite loop
    
    // while 1, if time
    while (micros()<t1)
      // do nothing
      ;

    //   read sensors
    switch (loopMode) {

      case 0:
        // the default control mode has a fixed ref
        // just read analog channel 1 (not zero)
        //sensorValuesArray[n]= analogRead(0);
        timeArray[n]= micros();

        ctrl_y= analogRead(1);
        sensorValuesArray[n+1]= ctrl_y;
        timeArray[n+1]= micros();
        break;

      case 9:
        // read two channels, 0=ref, 1=sensor
        // the extra analogRead makes the loop slower
        ctrl_ref= analogRead(0);
        sensorValuesArray[n]= ctrl_ref;
        timeArray[n]= micros();

        ctrl_y= analogRead(1);
        sensorValuesArray[n+1]= ctrl_y;
        timeArray[n+1]= micros();
        break;
    }

    //   calc ctrl
    ctrl_e= ctrl_ref - ctrl_y;
    ctrl_u= a0*ctrl_e +a1*ctrl_e1 +a2*ctrl_e2
                      -b1*ctrl_u1 -b2*ctrl_u2;
    sensorValuesArray[n]= ctrl_u; // save values before truncation
    myAnalogOutput(ctrl_u);

    ctrl_u2= ctrl_u1;
    ctrl_u1= ctrl_u;
    ctrl_e2= ctrl_e1;
    ctrl_e1= ctrl_e;
    
    //   read cmd, and write ans cmd
    //   if break loop then break
    
    if (Serial.available() > 0) {
	  c= Serial.read();
      buf2[0]= c;
      buf2[1]= '\0';
      Serial.print(buf2);
      break; // break the loop
    }
  }

  // stop the motor
  myAnalogOutput(0);
}


// --------------------------------------------------
void config_mode(char *buf) {
  int tmp;
  Serial.print("-- do cnf: ");
  Serial.println(buf);
  switch (buf[0]) {

    case 'f':
      loopOutputFlag= 0; break;
    case 'F':
      loopOutputFlag= 1; break;

    case 'v':
      ctrl_verbose_flag= 0; break;
    case 'V':
      ctrl_verbose_flag= '1'; break;

    case 'O':
      // config the analog output pin (where to send the step)
      tmp= buf[1];
      if ('0'<=tmp && tmp<='9')
        tmp= tmp-'0';
      else if ('a'<=tmp && tmp<='d')
        tmp= tmp-'a';
      else {
        Serial.println("E inv outpin");
        return;
      }
      myAnalogOutPin= tmp;

    default:
      Serial.println("E inv modecnf");
  }
}


void main_send_end_cmd() {
    Serial.print("> ");
}


void main_switch() {
  double x;

  // read cmd and execute it
  if (serial_read_str(&buf[0], BUFSZ)) {
    //Serial.print("-- Got cmd: ");
    Serial.println(buf); // echo the command just received

    // switch cmd
    switch (buf[0]) {
      case '\0':
          break;

      case '?':
        Serial.println("ver 20.11.2023 WeMos D1 R1");
        break;
      
      case 'T':
        // define the sampling period
        Ts= atoi(&buf[1]);
        Serial.print("Ts[us]=");
        Serial.println(Ts);
        break;

      case 'r':
        // set reference
        if (buf[1]>0)
          ctrl_ref= atof(&buf[1]);
        /*
        sprintf(buf, "ref=");
        itoa(ctrl_ref, buf2, BUFSZ2);
        strcat(buf, buf2); strcat(buf, "\n");
        Serial.print(buf);
        */
        Serial.print("ref=");
        Serial.println(ctrl_ref);
        break;
        
      case 'm':
        // select the loop mode
        if (buf[1]=='c') {
          config_mode(&buf[2]);
          Serial.print(buf);
          break;
        }
        loopMode= atoi(&buf[1]);
        sprintf(buf, "loopMode=%d\n", loopMode);
        Serial.print(buf);
        break;

      case 's':
        // step response (use ctrl_ref value)
        //Serial.println("-- Going to enter the open loop acq.");
        step_response();
        if (ctrl_verbose_flag)
          serial_print_data_n();
        //sprintf(buf, "step\n");
        //Serial.print(buf);
        break;

      case 'c':
        // config the controller
        switch (buf[1]) {
          case '0': x= atof(&buf[2]); a0= x; break;
          case '1': x= atof(&buf[2]); a1= x; break;
          case '2': x= atof(&buf[2]); a2= x; break;
          case '3': x= atof(&buf[2]); b1= x; break;
          case '4': x= atof(&buf[2]); b2= x; break;
          default: x=0.0; Serial.print('E');
        }
        Serial.print("x="); Serial.println(x);
        break;

      case 'C':
        // get the current controller
        /*
        Serial.print("par=[");
        Serial.print(a0); Serial.print(' ');
        Serial.print(a1); Serial.print(' ');
        Serial.print(a2); Serial.print(' ');
        Serial.print(b1); Serial.print(' ');
        Serial.print(b2); Serial.println("]");
        */
        Serial.print("Cz= (");
        Serial.print(a0); Serial.print(" + ");
        Serial.print(a1); Serial.print(" Z^-1 + ");
        Serial.print(a2); Serial.print(" Z^-2) / (1 + ");
        Serial.print(b1); Serial.print(" Z^-1 + ");
        Serial.print(b2); Serial.println(" Z^-2)");
        break;

      case 'i':
        // read sensor
        {
        int ch= 0;
        if (buf[1]!='\0') ch= buf[1]-'0';
        int v= analogRead(ch);
        buf[0]= 'i'; buf[1]= ch+'0'; buf[2]= '\0';
        /*itoa(v, buf2, BUFSZ2); strcat(buf, buf2);
        strcat(buf, "\n");
        Serial.print(buf);*/
        Serial.println(v);
        }
        break;

      case 'o':
        // set actuation
        {
        int v= atoi(&buf[1]);
        myAnalogOutput(v);
        sprintf(buf, "o"); itoa(v, buf2, BUFSZ2); strcat(buf, buf2); strcat(buf, "\n");
        Serial.print(buf);
        }
        break;

      case 'p':
        // pwm config, Arduino Uno version
        /*
        if (buf[1]>='1' & buf[1]<='5')
          pwm_config(buf[1]-'0');
        else
          buf[0]= 'E';
        Serial.print(buf);
        */
        // pwm config, Z-Uno only?
        /*{
        long int v= atoi(&buf[1]);
        analogWriteFrequency(v);
        Serial.print("PWM freq [Hz]= ");
        Serial.println(v);
        } */
        Serial.println("Err PWM NA");
        break;

      case 'x':
        // run the (finite time) control loop
        ctrl_finite_loop();
        if (ctrl_verbose_flag)
          serial_print_data_n();
        //Serial.print(buf);
        break;
        
      case 'X':
        main_send_end_cmd(); // reply cmd received
        // run the infinite control loop
        //   end loop with a received message
        ctrl_infinite_loop();
        if (ctrl_verbose_flag)
          serial_print_data_n();
        //Serial.print(buf);
        break;
        
      case '<':
        // get the ADC/DAC data buffer
        if (buf[1]=='?')
          serial_print_data_1( atoi(&buf[1]) ); // <?999 returns 1 sample
        else
          serial_print_data_n(); // return the full buffer
        break;
      case '>':
        // set the ADC/DAC data buffer
        // 2 cases: 1 value at a time, N values at a time
        if ('0'<=buf[1] && buf[1]<='9')
          set_data( atoi(&buf[1]), ctrl_ref );
        else
          set_data_array( &buf[1], ctrl_ref );
        break;

      case 'd':
        // digital input/output
        {
          // find the channel number
        int ch= 0;
        if (buf[1]>='0' & buf[1]<='9')
          ch= buf[1]-'0';
        else if (buf[1]>='a' & buf[1]<='d') // d=> dig output 13 (LED)
          ch= buf[1]-'a'+10;
        else if (buf[1]>='A' & buf[1]<='D')
          ch= buf[1]-'A'+10;

        Serial.print("ch="); Serial.println(ch);
        ch= myDigitalPin( ch ); // important name translation for WeMos

        if (buf[2]=='i') {
          // input the digital bit, dNi, N in {0,1,2,...}
          pinMode(ch, INPUT);
          if (digitalRead(ch))
            buf[3]= '1';
          else
            buf[3]= '0';
          buf[4]= '\0';
        }
        else if (buf[2]=='o') {
          // output the digital bit, dNoB, B in 0,1
          pinMode(ch, OUTPUT);
          if (buf[3]=='1')
            digitalWrite(ch, HIGH);
          else
            digitalWrite(ch, LOW);
        }
        else
          // do nothing
          buf[0]='E';
        }
        Serial.println(buf);
        break;
        
      case 'D':
        // delay in milisec
        t0= millis() +(unsigned long)atoi(&buf[1]);
        while (millis() < t0)
          /* do nothing */ ;
        Serial.print(buf);
        break;
		
	  case 'N':
	    // set digital pin to say negative output
        if (buf[1]!=0 && buf[1]=='X')
          myAnalogOutPin= atoi(&buf[2]);
        if (buf[1]!=0 && buf[1]=='Y')
          myAnalogOutPin2= atoi(&buf[2]);
        if (buf[1]!=0 && buf[1]=='N')
          myAnalogOutNegPin= atoi(&buf[2]);
        Serial.print("PWM pin1 = ");
        Serial.println(myAnalogOutPin);
        Serial.print("PWM pin2 = ");
        Serial.println(myAnalogOutPin2);
        Serial.print("Neg pin = ");
        Serial.println(myAnalogOutNegPin);
        break;

      default:
        strcat(buf, " <inv cmd\n");
        Serial.print(buf);
    }

    //Serial.print("> ");
    main_send_end_cmd();
  }
}


// --------------------------------------------------
void setup() {
  // start serial port at 9600 bps:
  // Serial.begin(9600);
  Serial.begin(115200);

  /*
  Serial.println("");
  Serial.print("D0:8=");
  for (n=0; n<=8; n++) {
    Serial.print(" ");
    Serial.print(myDigitalPin(n));
  }
  Serial.println("");
  */
}

void loop() {
  main_switch();
}
