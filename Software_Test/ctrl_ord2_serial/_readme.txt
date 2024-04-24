
Arduino commanded by Matlab

Oct2011, Oct2015, Nov2023, J. Gaspar, jag@isr.tecnico.ulisboa.pt

----------------------------------------------------------------------------


Before using, please set the correct serial port in the Matlab interface.
You usually find the COM port through the Arduino IDE.
For example, in the case of communicating through COM3 run the following:

	>> arduino_ini('COM3'); arduino_end


Usage example after uploading "ctrl_ord2*.ino" to the Arduino:

	>> arduino_ini; arduino_cmd({'C', 's', '<'}); arduino_end



List of commands to send to a Arduino running "ctrl_ord2*.ino"
----------------------------------------------------------

i	- read sensor 0
i(1)	- read sensor (i)= adc channel
o(d)	- set actuation (d) in 0..255
p(1)	- pwm config (1) in 1..5 [not working in WeMos]
d(d)i	- digital input, (d) in 0..9 a..d
d(d)o(f)	- digital output, (d) in 0..9 a..d, (f) in 0..1
D(d)	- delay in milisec (d)=decimal

C	- get the current controller coefs
c(1)(x)	- config the controller (1) in 0..4 (x)=float
T(x)	- define the sampling period (x)=float
r(x)	- set reference (x)=float
m(d)	- select the loop mode (d)=decimal in 0,1,2,9
mcf	- loopOutputFlag=0
mcF	- loopOutputFlag=1
mcv	- ctrl_verbose_flag=0
mcV	- ctrl_verbose_flag=1
s	- step response using ctrl_ref value
x	- run the control loop
X	- run the infinite control loop
<	- get the data buffers

Notes:
(1) means sending digit 1; in some commands other digits are valid
(d) decimal value, e.g. 0, 10, 255
(x) float value, e.g. 1.234



Usage example of 'arduino_interf.m'
-----------------------------------

function arduino_tst
%
% Arduino+EthernetShield test: Arduino runs a digital controller which is
% configured, run and assessed through a TCPIP/socket interface
%

% 7.Dec.2009, 23.2.2010 (new interf), J. Gaspar

arduino_ini

%arduino_cmd('T0 mcf o100 m1 s')
%cmdList= {'T0', 'mcf', 'o100', 'm1', 's'}; arduino_cmd(cmdList);

while 1
    s= input('- string(s) to send (or "quit")? ', 's');
    if strcmp(s, 'quit')
        break;
    end
    arduino_cmd(s);
end

arduino_end


Debug mode of 'arduino_interf.m'
--------------------------------

function arduino_interf_tst(tstId)
if nargin<1
    tstId=1;
end
switch tstId
    case 1
        arg1= {...
            'fid= fopen(''arduino_interf_tst_tmp.txt'', ''rt'');', ...
            'ret= arduino_get_array_main(fid, 1);', ...
            'fclose(fid);' };
        arduino_interf('debug', arg1)

    otherwise
        error('invalid tstId')
end
