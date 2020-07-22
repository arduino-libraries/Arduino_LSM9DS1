# Getting started with LSM9DS1 V2 library

The library provides methods to get IMU data out of the LSM9DS1 chip and was developed on an 
Arduino Nano 33 BLE Sense board.
This V2 library was designed to replace the older version 1.01
More information about the LSM9DS1 chip can be found in the [datasheet](https://www.st.com/resource/en/datasheet/lsm9ds1.pdf) 

## Installation

In the Arduino IDE select: *Sketch \ Include Library \ Add .ZIP Library*  and point to the location where the .Zip file is stored.
When the library has been added to the Arduino library list, it will be possible to use the 
Library manager as well.

If there was a previous version of the LSM9DS1 library already installed you should probably delete or remove that first
from the Arduino library installation directory  

..../Arduino/libraries/Arduino_LSM9DS1 

## Calibration

If you want to make use of the new calibration possibilities run the interactive example sketches
DIY_Calibration first. Follow the directions on the screen of the serial monitor. Copy the code
from the screen into the setup() of your sketch. 
See [instruction video](https://youtu.be/BLvYFXoP33o)

It turned out that the best calibration for the Gyroscope depended on the chip's Full Scale and Output Data Rate.
For this reason changing these settings has been added in the calibration programs. 

The functions readAccel, readGyro, readMagnet produce calibrated data in the unit of choise 
The functions readRawAccel, readRawGyro, readRawMagnet produce uncalibrated data and are meant for calibration purposes.

## Use any of the new setting possibilities by placing commands in your sketch
All the "set" methods have a corresponding "get" function.

**Output Data Rate** 
(for read... and readRaw... functions)
```
(range)=  (0..5) -> { off, 10, 50, 119, 238, 476}  Hz  default = 119hz
  IMU.setAccelODR(range); 
  IMU.setGyroODR (range); 

(range)= (0..8) -> {0.625, 1.25, 2.5, 5, 10, 20, 40, 80, 400} Hz  default = 20hz
IMU.setMagnetODR(range); 
```

**Full Scale setting**
(for read... and readRaw... functions)
```
   IMU.setAccelFS(range); // 0: ±2g ; 1: ±24g ; 2: ±4g ; 3: ±8g
   IMU.setGyroFS(range); // (0= ±245,  1= ±500, 2= ±1000, 3= ±2000) °/s
   IMU.setMagnetFS(range); // 0=±400.0; 1=±800.0; 2=±1200.0 , 3=±1600.0  (µT)
```

**Output unit unit you want to get the output in**
(for read... and readRaw... functions)
```
   IMU.accelUnit = GRAVITY;      //  GRAVITY   OR  METERPERSECOND2 
   IMU.gyroUnit = DEGREEPERSECOND;   // DEGREEPERSECOND  RADIANSPERSECOND REVSPERMINUTE REVSPERSECOND
   IMU.magnetUnit = MICROTESLA;  //  GAUSS,  MICROTESLA NANOTESLA
```
**Choose a BandWidth filter**
For advanced users only
```
   IMU.setAccelBW(range); //0,1,2,3 Override autoBandwidth setting see doc.table 67
   IMU.setGyroBW(range);  //Bandwidth setting 0,1,2,3  see doc. table 46 and 47
   
```

