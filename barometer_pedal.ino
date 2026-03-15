/*

A barometer-pedal for electronic piano.
For handicapped people, who can't handle a foot-pedal.
Instead they can put underpressure or overpressure into a simple tube.
Uses an (differential) air-pressure sensor from NXP : MPXV 7007 DP
This is connected to an analog port : A0
As it is differential air pressure sensor it gives 
~ 2.5 V on no (differential) pressure 
goes down to 0V on underpressure 
goes up to 5V on overpressure.

The original idea is just to open a relay (as used on Roland E-Piano to replace a Roland DP-2 which is an opener).
But as it is a differential sensor, it allows two reactions: On underpressure and on over-pressure.
To make the HW and the SW more flexible the limit for switching can be changed with simple potentiometers.
And also to make the usage more flexible it allows to act as opener as well as to close a circuit.

Attention: The relay switches on LOW !


*/

#define PLOTTER

 
int pressure_sensor_pin = A0; // MPXV 7007 DP analog output connected to analog pin 0, return 0 ... 1024 , differential pressure therefore idle value should be ~ 512
int vacuum_limit_poti_pin  = A2; // potentiometer wiper (middle terminal) connected to analog pin 1. Gives also 0 ... 1024 
int pressure_limit_poti_pin  = A1; // potentiometer wiper (middle terminal) connected to analog pin 2. Gives also 0 ... 1024



int relay_vacuum_pin = 2;
int relay_pressure_pin = 3;
int norm_hold_switch_pin = 4;

int calibration_value = 0;
enum modus { norm, hold  } actual_modus = norm;



int Calibrate(void)
{
  int accumulate = 0;
  int i;

  // we measure 50 values between 0 and 1024. This fits into 16 Bit int, 64k 
  // we measure 50 values and delay 20 ms => the loop takes 20*50 ms = 1000ms = 1sek
  for ( i = 0; i < 50; ++i )
  {
    accumulate +=  analogRead(pressure_sensor_pin);
    delay( 20 );
  }
  return accumulate/50;
}

int adjust_max_pressure( int analog_value)
{
  // the potentiometer shall allow to adjust the needed pressure value 
  // pressure starts ~ 512 and ends at 1023
  // the exact middle_value is stored in calibration_value
  // poti starts (unfortunately, wrong soldered, but to correct this we have software ...) from 1023 on the left end and 0 on the right end
  // we want the poti on the right end to force max pressure
  // ==>
  // invert measured value
  float value = 1023 - analog_value;
  // we need to map this now to the range from ~ 512 to 1023
  float adjusted_value = (float) calibration_value + (1024.0 - (float) calibration_value )/ 1024.0 * value;
  return adjusted_value;
}

int adjust_max_vacuum( int analog_value)
{
  // the potentiometer shall allow to adjust the needed pressure value 
  // vacuum starts at 0 (max vacuum) and ends ~ 512 
  // the exact middle_value is stored in calibration_value
  // poti starts (unfortunately, wrong soldered, but to correct this we have software ...) from 1023 on the left end and 0 on the right end
  // we want the poti on the left end to force max vacuum
  // ==>
  // invert measured value
  float value = 1023 - analog_value;
  // we need to map this now to the range from 0 to ~ 512 
  float adjusted_value = ((float) calibration_value )/ 1024.0 * value;
  return adjusted_value;
}


// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
  pinMode(relay_vacuum_pin, OUTPUT);
  pinMode(relay_pressure_pin, OUTPUT);
  pinMode(norm_hold_switch_pin, INPUT_PULLUP);
  digitalWrite(relay_vacuum_pin, HIGH);
  digitalWrite(relay_pressure_pin, HIGH);
  calibration_value = Calibrate();
  
  delay(5000);
  Serial.println("Calibration: " );
  Serial.println(calibration_value);
  // preparation for the plotter 
#ifdef PLOTTER
  Serial.println("Act_Pressure:,Vacuum_Lim:,Pressure_Lim:,Vacuum_Relay:,Pressure_Relay:");
#endif

}


int old_relay_pressure_val = 0; 
int old_relay_vacuum_val = 0;
int vacuum_relay_state = 0; 
int pressure_relay_state = 0; 

// the loop routine runs over and over again forever:
void loop() {
  int act_baro_val;
  int vacuum_limit_val;
  int pressure_limit_val;
  int relay_pressure_val;
  int relay_vacuum_val;
  int corrected_pressure_limit;
  int corrected_vacuum_limit;
  int act_switch_value;
  
  vacuum_limit_val    = analogRead(vacuum_limit_poti_pin);  // read the input pin
  pressure_limit_val  = analogRead(pressure_limit_poti_pin);  // read the input pin
  act_baro_val    = analogRead(pressure_sensor_pin);  // read the input pin

  act_switch_value = digitalRead( norm_hold_switch_pin );
  if ( act_switch_value == 1 )
  {
     actual_modus = norm;
  }
  else
  {
    actual_modus = hold;
  }
  corrected_pressure_limit = adjust_max_pressure(pressure_limit_val);
  corrected_vacuum_limit   = adjust_max_vacuum( vacuum_limit_val );


#ifdef PLOTTER  
  Serial.print(act_baro_val);
  Serial.print(",");
  Serial.print( corrected_vacuum_limit );
  Serial.print(",");
  Serial.print(corrected_pressure_limit);// debug value
  Serial.print(","); 

#endif 
  // Step 1 : Check if pressure or vacuum are greater than expected limit
  if ( act_baro_val  > corrected_pressure_limit )
  {
    relay_pressure_val = 1;
  }
  else
  {
    relay_pressure_val = 0;
  }
  if ( act_baro_val   <  corrected_vacuum_limit )
  {
    relay_vacuum_val = 1;
  }
  else
  {
    relay_vacuum_val = 0;
  }

  // Step 2a: If modus == norm, then just take over the calculated values and accept them as the state 
  if ( actual_modus == norm )
  {
    vacuum_relay_state = relay_vacuum_val;
    pressure_relay_state = relay_pressure_val; 
  }
  else  // Step 2b : if modus == hold then a logical raising edge will invert the value of the relays 
  {
    if( (relay_vacuum_val == 1)  && (old_relay_vacuum_val == 0 ))
    {
      vacuum_relay_state = ! vacuum_relay_state; 
    }
    if( (relay_pressure_val == 1)  && (old_relay_pressure_val == 0 ))
    {
      pressure_relay_state = ! pressure_relay_state; 
    }

  }
  old_relay_pressure_val = relay_pressure_val;
  old_relay_vacuum_val = relay_vacuum_val;
  // Step 3: Apply the evaluated relay_state
  if ( pressure_relay_state == 1 )
  {
    digitalWrite(relay_pressure_pin, LOW);
    Serial.print("250,");
  }
  else
  {
    digitalWrite(relay_pressure_pin, HIGH);
    Serial.print("0,");
  }
  if ( vacuum_relay_state == 1 )
  {
    digitalWrite(relay_vacuum_pin, LOW);
    Serial.println("250");
  }
  else
  {
    digitalWrite(relay_vacuum_pin, HIGH);
    Serial.println("0");
  }
  
  delay(100);
}
