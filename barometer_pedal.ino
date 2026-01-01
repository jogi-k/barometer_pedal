/*

A barometer-pedal for electronic piano.
For handicapped people, who can't handle a foot-pedal.
Instead they can put underpressure or overpressure into a simple tube.
Uses an (differential) air-pressure sensor from NXP : MPXV 7007 DP
This is connected to an analog port : A0
As it is differential air pressure sensor it gives 
~ 2.5 V on no pressure 
goes down to 0V on underpressure 
goes up to 5V on overpressure.

The original idea is just to open a relay (as used on Roland E-Piano to replace a Roland DP-2 which is an opener).
But as it is a differential sensor, it allows two reactions: On underpressure and on over-pressure.
To make the HW and the SW more flexible the limit for switching can be changed with simple potentiometers.
And also to make the usage more flexible it allows to act as opener as well as to close a circuit.




*/
 
int analogPin0 = A0; // potentiometer wiper (middle terminal) connected to analog pin 0
int analogPin1 = A1; // potentiometer wiper (middle terminal) connected to analog pin 1
int analogPin2 = A2; // potentiometer wiper (middle terminal) connected to analog pin 2

int analog_val0;
int analog_val1;
int analog_val2;


// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  analog_val2 = analogRead(analogPin2);  // read the input pin
  Serial.println(analog_val2);          // debug value
  delay(100);
}
