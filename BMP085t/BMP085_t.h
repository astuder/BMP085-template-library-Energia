/*
 BMP085_t.h - I2C-based template library for Bosch BMP085 digital pressure sensor.
   Created by Adrian Studer, April 2013. 
 
   Distributed under MIT License, see license.txt for details.   
   
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
        Reading temperature only takes 5ms and reducess code size by 700 bytes
     eocpin - Digital pin connected to the sensor's EOC pin, 0=not connected, default is 0
        Using the EOC pin is typically 30% faster than waiting a fixed time when reading sensor data
        Sketch size grows by 20-200 bytes depending on use of digitalRead() in your sketch
     i2caddress - I2C address of sensor, default is 0x77
 
 Methods:
   begin - Initalizes I2C, reads sensor calibraiton data, configures EOC pin as input (optional)
   refresh - Retrieves fresh raw data from sensor
   calculate - Calculates temperature and pressure from raw sensor data
 
 Attributes:
   temperature - Temperature in 0.1 degree Celcius
   pressure - Pressure in Pascal
   rawTemperature - Raw temperature reading from sensor
   rawPressure - Raw pressure reading from sensor
 
 */

#ifndef BMP085_T_h
#define BMP085_T_h

#include <inttypes.h>
#include "Wire.h"
#include "Energia.h"

//#define DEBUG_BMP085           // uncomment for debug output (init serial in your sketch!)

#define BMP085_I2C_ADDR  0x77  // I2C address of BMP085 sensor, factory default is 0x77

// Bosch BMP085 chip ID 
#define BMP085_CHIP_ID   85    // Bosch BMP085 chip identifier - not mentioned in datasheet but found in example source code

// register addresses
#define BMP085_REG_ID    0xd0  // chip ID register
#define BMP085_REG_VER   0xd1  // chip version register
#define BMP085_REG_CTRL  0xf4  // measurement control
#define BMP085_REG_ADC   0xf6  // measurement result (MSB, LSB, XLSB)
#define BMP085_REG_RST   0xe0  // soft reset
#define BMP085_REG_CAL   0xaa  // start of calibration EEPROM

// measurement commands
#define BMP085_CMD_T     0x2e  // measure temperature
#define BMP085_CMD_P     0x34  // measure pressure

// structure of calibration EEPROM
struct BMP085_cal_t {
  int16_t    ac1;
  int16_t    ac2;
  int16_t    ac3;
  uint16_t   ac4;
  uint16_t   ac5;
  uint16_t   ac6;
  int16_t    b1;
  int16_t    b2;
  int16_t    mb;
  int16_t    mc;
  int16_t    md;
};

template <uint8_t oversampling = 0, uint16_t eocpin = 0, uint8_t i2caddress = BMP085_I2C_ADDR>
struct BMP085 {
  
public:
  int16_t temperature;      // last calculated temperature in 0.1 C
  int32_t pressure;         // last calculated pressure in Pa 
  uint16_t rawTemperature;  // last measured temperature
  uint32_t rawPressure;     // last measured pressure 

  // call once to initalize, reads sensor calibration data
  void begin()              
  {
    Wire.begin();

#ifdef DEBUG_BMP085
    // verify that we're actually talking to a BMP085 sensor
    if(m_read8(BMP085_REG_ID) != BMP085_CHIP_ID) {
      Serial.println("BMP085 device not found");
      return;
    }
#endif

    // read calibration data from sensor EEPROM
    int16_t *calarray = (int16_t*) &cal;
    uint8_t addr = 0;
    while(addr < 11)
    {
      calarray[addr] = m_read16(BMP085_REG_CAL + (addr * 2));
      addr++;
    }

#ifdef DEBUG_BMP085
    Serial.print("AC1="); 
    Serial.println(cal.ac1);
    Serial.print("AC2="); 
    Serial.println(cal.ac2);
    Serial.print("AC3="); 
    Serial.println(cal.ac3);
    Serial.print("AC4="); 
    Serial.println(cal.ac4);
    Serial.print("AC5="); 
    Serial.println(cal.ac5);
    Serial.print("AC6="); 
    Serial.println(cal.ac6);
    Serial.print("B1="); 
    Serial.println(cal.b1);
    Serial.print("B2="); 
    Serial.println(cal.b2);
    Serial.print("MB="); 
    Serial.println(cal.mb);
    Serial.print("MC="); 
    Serial.println(cal.mc);
    Serial.print("MD="); 
    Serial.println(cal.md);
#endif

    if(eocpin) {                         // if EOC pin is configured to detect end of conversion..
      pinMode(eocpin, INPUT);            // setup EOC pin as digital input
    }

    return;  // done with initialization
  };

  // reading fresh raw data from sensor
  void refresh()
  {
#ifdef DEBUG_BMP085
    Serial.println("Reading temperature..");
#endif

    m_sendCmd(BMP085_CMD_T);                // send command to read temperature

    if(eocpin) {                            // if EOC pin is configured..
      while(digitalRead(eocpin)==LOW);      // wait for end of conversion
    } else {                                // if not..
      delay(5);                             // wait 5ms for data to be ready
    }

    rawTemperature = m_read16(BMP085_REG_ADC);    // read raw temperature data
    
#ifdef DEBUG_BMP085
    Serial.print("UT="); 
    Serial.println(rawTemperature);
#endif

    if(oversampling >= 4) return;            // done if in temperature only mode

#ifdef DEBUG_BMP085
    Serial.println("Reading pressure..");
#endif

    m_sendCmd(BMP085_CMD_P | (oversampling << 6)); // send command to read pressure, incl. oversampling mode

    if(eocpin) {                            // if EOC pin is configured..
      while(digitalRead(eocpin)==LOW);      // wait for end of conversion
    } else {                                // if not..
      if(oversampling == 0) delay(5);       // delay for conversion to complete
      else if(oversampling == 1) delay(8);  //   using maximum conversion times
      else if(oversampling == 2) delay(14); //   depending on level of oversampling
      else if(oversampling == 3) delay(26);
    }

    if(oversampling == 0) {                    // if no oversampling..
      rawPressure = m_read16(BMP085_REG_ADC);  // read 16 bit raw pressure
    } else {                                   // if there is oversampling read more bits
      rawPressure = (((uint32_t)m_read16(BMP085_REG_ADC) << 8) | m_read8(BMP085_REG_ADC + 2)) >> (8-oversampling);
    }

#ifdef DEBUG_BMP085
    Serial.print("UP="); 
    Serial.println(rawPressure);
#endif

    return;        // done refreshing raw data
  }

  // calculating temperature and pressure from raw data
  void calculate()
  {
#ifdef DEBUG_BMP085
    Serial.println("Calculating temperature in 0.1 Celcius..");
#endif

    // calculating temperature
    int32_t calcX1 = (((int32_t)rawTemperature - (int32_t)cal.ac6) * (int32_t)cal.ac5) >> 15;
    int32_t calcX2 = ((int32_t)cal.mc << 11) / (calcX1 + cal.md);
    int32_t calcB5 = calcX1 + calcX2;
    temperature = (calcB5 + 8) >> 4;
    
#ifdef DEBUG_BMP085
    Serial.print("X1="); 
    Serial.println(calcX1);
    Serial.print("X2="); 
    Serial.println(calcX2);
    Serial.print("B5="); 
    Serial.println(calcB5);
    Serial.print("T="); 
    Serial.println(temperature);
#endif

    if(oversampling >= 4) return;    // done if in temperature only mode

#ifdef DEBUG_BMP085
    Serial.println("Calculating pressure in Pascal..");
#endif

    // calculating pressure
    int32_t calcB6 = calcB5 - 4000;
    calcX1 = ((int32_t)cal.b2 * (calcB6 * calcB6 >> 12)) >> 11;
    calcX2 = (int32_t)cal.ac2 * calcB6 >> 11;
    int32_t calcX3 = calcX1 + calcX2;
    int32_t calcB3 = ((((int32_t)cal.ac1 * 4 + calcX3) << oversampling) + 2) >> 2;
#ifdef DEBUG_BMP085
    Serial.print("B6=");
    Serial.println(calcB6);
    Serial.print("X1="); 
    Serial.println(calcX1);
    Serial.print("X2="); 
    Serial.println(calcX2);
    Serial.print("X3="); 
    Serial.println(calcX3);
    Serial.print("B3="); 
    Serial.println(calcB3);
#endif
    calcX1 = (int32_t)cal.ac3 * calcB6 >> 13;
    calcX2 = ((int32_t)cal.b1 * (calcB6 * calcB6 >> 12)) >> 16;
    calcX3 = ((calcX1 + calcX2) + 2) >> 2;
    calcB4 = (int32_t)cal.ac4 * (uint32_t)(calcX3 + 32768) >> 15;
    calcB7 = ((uint32_t)rawPressure - calcB3) * (50000 >> oversampling);
    if(calcB7 < 0x80000000) pressure = ((calcB7 * 2) / calcB4);
    else pressure = (calcB7 / calcB4) * 2;
#ifdef DEBUG_BMP085
    Serial.print("X1="); 
    Serial.println(calcX1);
    Serial.print("X2="); 
    Serial.println(calcX2);
    Serial.print("X3="); 
    Serial.println(calcX3);
    Serial.print("B4="); 
    Serial.println(calcB4);
    Serial.print("B7="); 
    Serial.println(calcB7);
    Serial.print("p="); 
    Serial.println(pressure);
#endif
    calcX1 = (pressure >> 8) * (pressure >> 8);
#ifdef DEBUG_BMP085
    Serial.print("X1="); 
    Serial.println(calcX1);
#endif
    calcX1 = (calcX1 * 3038) >> 16;
    calcX2 = (-7357 * pressure) >> 16;
    pressure = pressure + ((calcX1 + calcX2 + 3791) >> 4);
#ifdef DEBUG_BMP085
    Serial.print("X1="); 
    Serial.println(calcX1);
    Serial.print("X2="); 
    Serial.println(calcX2);
    Serial.print("p="); 
    Serial.println(pressure);
#endif
    
    return;    // done with calculations
  };

private:
  BMP085_cal_t  cal;    // Sensor calibration data, intialized during begin()
  
//  int32_t       calcX1, calcX2, calcX3;    // temporary variables used in calculations
//  int32_t       calcB3, calcB5, calcB6;    // code size is smaller if some are declared local
  uint32_t      calcB4, calcB7;              // but larger if there are too many..

  // send command to BMP085
  void m_sendCmd(uint8_t cmd) {  
    Wire.beginTransmission(i2caddress);
    Wire.write(byte(BMP085_REG_CTRL));      
    Wire.write(byte(cmd));
    Wire.endTransmission();
  }
  
  // read 8 bits from I2C
  uint8_t m_read8(uint8_t addr) {
    Wire.beginTransmission(i2caddress);
    Wire.write(addr);
    Wire.endTransmission();

    Wire.requestFrom(i2caddress, (uint8_t)1);    // need to cast int to avoid compiler warnings
    return Wire.read();
  };

  // read 16 bits from I2C
  uint16_t m_read16(uint8_t addr) {
    Wire.beginTransmission(i2caddress);
    Wire.write(addr);
    Wire.endTransmission();

    Wire.requestFrom(i2caddress, (uint8_t)2);    // need to cast int to avoid compiler warnings
    return (Wire.read() << 8 | Wire.read());
  };

};  // end struct/template BMP085

#endif
