/*
  This file is part of the Arduino_LSM9DS1 library.
  Copyright (c) 2019 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
  
  Modifications by Femme Verbeek, Pijnacker, the Netherlands 23 may 2020, 
  Released to the public domain
  version 2.0.0
*/

#ifndef LSM9DS1_V2
      #define LSM9DS1_V2
#endif	  

#define accelerationSampleRate getAccelODR
#define gyroscopeSampleRate getGyroODR
#define magneticFieldSampleRate getMagnetODR

#define setAccelerationSampleRate setAccelODR		//development
#define setGyroscopeSampleRate setGyroODR			//development
#define setMagnetisFieldSampleRate setMagnetODR		// development

#define accelerationFullScale getAccelFS			//development
#define gyroscopeFullScale getGyroFS				//development
#define magneticFieldFullScale getMagnetFS			//development

#define setAccelerationFullScale setAccelFS			//development
#define setGyroscopeFullScale setGyroFS				//development
#define setMagneticFieldFullScale setMagnetFS		//development

#define readAcceleration readAccel		
#define readGyroscope readGyro
#define readMagneticField readMagnet

#define accelerationAvailable accelAvailable
#define gyroscopeAvailable gyroAvailable
#define magneticFieldAvailable magnetAvailable

#include <Arduino.h>
#include <Wire.h>
#define GAUSS             0.01           
#define MICROTESLA        1.0       // default
#define NANOTESLA         1000.0  
#define GRAVITY           1.0       // default
#define METERPERSECOND2   9.81  
#define DEGREEPERSECOND   1.0       //default
#define RADIANSPERSECOND  3.141592654/180

class LSM9DS1Class {
  public:
    LSM9DS1Class(TwoWire& wire);
    virtual ~LSM9DS1Class();

    int begin();
    void end();

    // Controls whether a FIFO is continuously filled, or a single reading is stored.
    // Defaults to one-shot.
    void setContinuousMode();
    void setOneShotMode();

    // Accelerometer
    float accelOffset[3] = {0,0,0}; // zero point offset correction factor for calibration
	  float accelSlope[3] = {1,1,1};  // slope correction factor for calibration
	  float accelUnit = GRAVITY;      //  GRAVITY   OR  METERPERSECOND2 
    virtual int   readAccel(float& x, float& y, float& z); // Results are in G (earth gravity) or m/s2.
    virtual int   accelAvailable(); // Number of samples in the FIFO.
	  virtual void  setAccelOffset(float x, float y, float z);  //Look out, from measurements only
	  virtual void  setAccelSlope(float x, float y, float z);
    virtual int   setAccelODR(int8_t range); // Sample Rate 0:off, 1:10Hz, 2:50Hz, 3:119Hz, 4:238Hz, 5:476Hz, 6:952Hz, 7:NA Automatic setting of BW
    virtual float getAccelODR(); // Sample Rate of the sensor.
    virtual float setAccelBW(int8_t range); //0,1,2,3 Override autoBandwidth setting see doc.table 67
    virtual float getAccelBW();  //Bandwidth setting 0,1,2,3  see documentation table 67
    virtual int   setAccelFS(int8_t range); // 0: ±2g ; 1: ±16g ; 2: ±4g ; 3: ±8g
    virtual float getAccelFS(); // Full Scale setting (output = 2.0,  16.0 , 4.0 , 8.0)  

    // Gyroscope
	  float gyroOffset[3] = {0,0,0};      // zero point offset correction factor for calibration
	  float gyroSlope[3] = {1,1,1};  // slope correction factor for calibration
    float gyroUnit = DEGREEPERSECOND;   // DEGREEPERSECOND   or  RADIANSPERSECOND
    virtual int   readGyro(float& x, float& y, float& z); // Results are in degrees/second or rad/s.
    virtual int   gyroAvailable(); // Number of samples in the FIFO.
	  virtual void  setGyroOffset(float x, float y, float z);  //Look out, from measurements only
	  virtual void  setGyroSlope(float x, float y, float z);
    virtual int   setGyroODR(int8_t range); //Sampling Rate 0:off, 1:10Hz, 2:50Hz, 3:119Hz, 4:238Hz, 5:476Hz, 6:952Hz, 7:NA
    virtual float getGyroODR(); // Sampling rate of the sensor.
    virtual int   setGyroBW(int8_t range);  //Bandwidth setting 0,1,2,3  see documentation table 46 and 47
    virtual float getGyroBW();  //Bandwidth setting 0,1,2,3  see documentation table 46 and 47
    virtual int   setGyroFS(int8_t range); // (0= ±245 dps; 1= ±500 dps; 2= ±1000 dps; 3= ±2000 dps)
    virtual float getGyroFS(); //  (output = 245.0,  500.0 , 1000.0, 2000.0) 

    // Magnetometer
	  float magnetOffset[3] = {0,0,0}; // zero point offset correction factor for calibration
	  float magnetSlope[3] = {1,1,1};  // slope correction factor for calibration
    float magnetUnit = MICROTESLA;  //  GAUSS  or MICROTESLA 
    virtual int   readMagnet(float& x, float& y, float& z); // Default results are in uT (micro Tesla) 
    virtual int   magnetAvailable(); // Number of samples in the FIFO.
	  virtual void  setMagnetOffset(float x, float y, float z);  //Look out, from measurements only
	  virtual void  setMagnetSlope(float x, float y, float z);
    virtual int   setMagnetODR(int8_t range); // Sampling rate (0..7) corresponds to {0.625,1.25,2.5,5.0,10.0,20.0,40.0,80.0}Hz
    virtual float getMagnetODR(); // Sampling rate of the sensor in Hz.
    virtual int   setMagnetFS(int8_t range); // 0=400.0; 1=800.0; 2=1200.0 , 3=1600.0  (µT)
    virtual float getMagnetFS(); //  get chip's full scale setting  

  private:
    bool continuousMode;
    int readRegister(uint8_t slaveAddress, uint8_t address);
    int readRegisters(uint8_t slaveAddress, uint8_t address, uint8_t* data, size_t length);
    int writeRegister(uint8_t slaveAddress, uint8_t address, uint8_t value);

  private:
    TwoWire* _wire;
};

extern LSM9DS1Class IMU;
