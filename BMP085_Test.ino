/*
 BMP085_Test - Example of using I2C-based template library for Bosch BMP085 digital pressure sensor.
   Created by Adrian Studer, April 2013.
   
   Distributed under MIT License, see license.txt for details.   
*/

#include "BMP085_t.h"      // import BMP085 template library

BMP085<1> PSensor;         // instantiate sensor, 3 = high precision pressure reading

void setup()
{
  Serial.begin(9600);      // initialize serial, used to display readings in Serial Monitor
  PSensor.begin();         // initalize pressure sensor
}

void loop()
{
  PSensor.refresh();                    // read current sensor data
  PSensor.calculate();                  // run calculations for temperature and pressure
  Serial.print("Temperature: ");         
  Serial.print((PSensor.temperature+5)/10);  // display temperature in Celcius
  Serial.println("C");
  Serial.print("Pressure:    ");
  Serial.print((PSensor.pressure+50)/100);   // display pressure in hPa
  Serial.println("hPa");

  delay(5000);                          // wait a while before next loop
}

