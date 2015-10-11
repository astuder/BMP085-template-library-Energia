Bosch BMP085 Template Library for Energia and Arduino
=====================================================

BMP085_t.h - I2C-based template library for Bosch BMP085 digital pressure sensor.
Created by Adrian Studer, April 2013.

Distributed under MIT License, see license.txt for details.   

The Bosch BMP085 is a barometric pressure sensor with I2C interface. 
Multiple vendors like Adafruit and Sparkfun sell breakout boards. It is also quite commonly found
on cheap chinese IMUs, for example the GY-80.

Users [report](http://forum.43oh.com/topic/3777-energia-library-bosch-bmp085-template-library/?p=44410), that this library also works with the newer Bosch BMP180 pressure sensor, as well as [Software I2C Libraries](http://forum.43oh.com/topic/3617-energia-library-software-i2c-master-for-msp430g2553/).

[This thread on 43oh](http://forum.43oh.com/topic/3777-energia-library-bosch-bmp085-template-library/) is the best place to report issues with this library, ask questions, or simply show what you built with it.

MSP430
------

Tested with Energia 0101E0016
* LaunchPad Rev1.5 w/ TI MSP430G2553
* LaunchPad w/ TI MSP430F5529

Connections MSP430G2553
* P1.6 => I2C SCL
* P1.7 => I2C SDA
* You may have to remove LED2 jumper for I2C to work properly.

Connections MSP430F5529
* P3.0 => I2C SCL
* P3.1 => I2C SDA

Tiva and Stellaris
------------------

Tested with Energia 0101E0010
* Stellaris LaunchPad Rev A, TI LM4F120H5QR
* Tiva C Series LaunchPad Rev A, TI TM4C123GH6PM

Connections
* PD.0 / SCL(3) => I2C SCL 
* PD.1 / SDA(3) => I2C SDA 

Energia uses I2C module 3 as default. To use a different I2C module call
Wire.setModule(N) before Wire.begin(), where N is the desired module.

Arduino
-------

Tested with Arduino 1.0.4
Arduino Uno R3, Atmel ATmega328

Connections
* A5 => I2C SCL
* A4 => I2C SDA

Usage
-----

- Instantiate sensor template
- Initalize Wire I2C library
- Call **begin()** once when starting up
- Call **refresh()** to retrive fresh raw data from sensor
- Raw readings are now available through attributes **rawTemperature** and **rawPressure**
- Call **calculate()** to calculate temperature and pressure based on raw data
- Access temperature and pressure through attributes **temperature** and **pressure**

Examples
--------

Instantiating sensor with default settings:

	BMP085<> MySensor;

Instantiating sensor connecting EOC to pin 1.5, no oversampling:

	BMP085<0,P1_5> MySensor;

Instantiating sensor with highest precision pressure reading

	BMP085<3> MySensor;

Instantiating sensor for temperature reading only, output in Fahrenheit

	BMP085<4,0,BMP085_F> MySensor;

Initalizing I2C and sensor on startup

	Wire.begin();
	MySensor.begin();

Retrieving a new temperature and pressure reading

	MySensor.refresh();
	MySensor.calculate();
	int myTemperature = MySensor.temperature;
	long myPressure = MySensor.pressure;

Template
--------

	BMP085<oversampling, eocpin, tempunit, i2caddress>

oversampling - Precision of pressure reading
* 0-3 (low-high), 4=read temperature only, default is 0
* 0 is fastest (max 10ms), 3 slowest (max 31ms)
* Oversampling also increases code size by 80-90 bytes
* Reading temperature only takes 5ms and reduces code size by 700 bytes

eocpin - Digital pin connected to the sensor's EOC pin
* 0=not connected, default is 0
* The EOC pin indicates when sensor data is ready to be read.
* Using the EOC pin is typically 30% faster than waiting a fixed time when reading sensor data
* Sketch size grows by 20-200 bytes depending on use of digitalRead() in your sketch

tempunit - Unit for temperature calculation
* BMP085_C=Celsius, BMP085_F=Fahrenheit
* default is BMP085_C

i2caddress - I2C address of sensor
* default is 0x77

Methods
-------

* begin - Reads sensor calibration data, configures EOC pin as input (optional)
* refresh - Retrieves fresh raw data from sensor
* calculate - Calculates temperature and pressure from raw sensor data
 
Attributes
----------

* temperature - Temperature in 0.1 degree Celsius
* pressure - Pressure in Pascal
* rawTemperature - Raw temperature reading from sensor
* rawPressure - Raw pressure reading from sensor
