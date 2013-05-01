/*
 ReadSensor - Basic example of using I2C-based template library for Bosch BMP085 digital pressure sensor.
   Created by Adrian Studer, April 2013.
   Distributed under MIT License, see license.txt for details.   
*/

#include <Wire.h>          // required by BMP085 library
#include <BMP085_t.h>      // import BMP085 template library

BMP085<0> PSensor;         // instantiate sensor, 0 = low precision pressure reading

void setup()
{
  Serial.begin(9600);      // initialize serial, used to display readings in Serial Monitor
  Wire.begin();            // initialize I2C that connects to sensor
  PSensor.begin();         // initalize pressure sensor
}

void loop()
{
  PSensor.refresh();                    // read current sensor data
  PSensor.calculate();                  // run calculations for temperature and pressure
  Serial.print("Temperature: ");         
  Serial.print(PSensor.temperature/10);  // display temperature in Celsius
  Serial.print(".");
  Serial.print(PSensor.temperature%10);  // display temperature in Celsius
  Serial.println("C");
  Serial.print("Pressure:    ");
  Serial.print((PSensor.pressure+50)/100);   // display pressure in hPa
  Serial.println("hPa");

  delay(5000);                          // wait a while before next loop
}

