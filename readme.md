I2C-based template library for Bosch BMP085 digital temperature and pressure sensor for Energia.

 Created by Adrian Studer, April 2013. 
 Distributed under MIT License, see license.txt for details.   

 This library was developed with Energia on MSP430G2553 LaunchPad. It should be easily portable to
 Arduino by replacing Energia.h. 

 Pin out on MSP430 LaunchPad: 
   P1_6 => I2C SCL
   P1_7 => I2C SDA
   You might need to remove LED2 jumper for I2C to work properly.
   *I had to patch Energia to make this work for MSP430G2553* https://github.com/energia/Energia/pull/226
   
 Usage:
   1. Instantiate sensor template
   2. Call begin() once when starting up
   3. Call refresh() to retrive fresh raw data from sensor
      Raw readings are now available through attributes "rawTemperature" and "rawPressure"
   4. Call calculate() to calculate temperature and pressure based on raw data
   5. Access temperature and pressure through attributes "temperature" and "pressure"
 
 Examples:
   Instantiating sensor with default settings:
     BMP085<> MySensor;
   Instantiating sensor connecting EOC to pin 1.5, no oversampling:
     BMP085<0,P1_5> MySensor;
   Instantiating sensor with highest precision pressure reading
     BMP085<3> MySensor;
 
   Initalizing sensor on startup
     MySensor.begin();
     
   Retrieving a new temperature and pressure reading
     MySensor.refresh();
     MySensor.calculate();
     int myTemperature = MySensor.temperature;
     long myPressure = MySensor.pressure;
   
 Template:
   BMP085<oversampling,eocpin,i2caddress>
     oversampling - Precision of pressure reading, 0-3 (low-high), 4=read temperature only, default is 0
        0 is fastest (max 10ms), 3 slowest (max 31ms)
        Oversampling also increases code sizeby 80-90 bytes
        Reading temperature only takes 5ms and reduces code size by 700 bytes
     eocpin - Digital pin connected to the sensor's EOC pin, 0=not connected, default is 0
        Using the EOC pin is typically 30% faster than waiting a fixed time when reading sensor data
        Sketch size grows by 20-200 bytes depending on use of digitalRead() in your sketch
     i2caddress - I2C address of sensor, default is 0x77
 
 Methods:
   begin - Initalizes I2C, reads sensor calibraiton data, configures EOC pin as input (optional)
   refresh - Retrieves fresh raw data from sensor
   calculate - Calculates temperature and pressure from raw sensor data
 
 Attributes:
   temperature - Temperature in 0.1 degree Celsius
   pressure - Pressure in Pascal
   rawTemperature - Raw temperature reading from sensor
   rawPressure - Raw pressure reading from sensor
