/*
  Arduino LSM9DS1 - Simple Gyroscope
  Extended with library V2.0 function calls

  This example reads the gyroscope values from the LSM9DS1
  sensor and continuously prints them to the Serial Monitor
  or Serial Plotter.

  The circuit:
  - Arduino Nano 33 BLE Sense
  - or Arduino Uno connected to LSM9DS1 breakout board

  created 10 Jul 2019
  by Riccardo Rizzo
  
  Modified by Femme Verbeek 10 jul 2020
  
  This example code is in the public domain.
*/

#include <Arduino_LSM9DS1.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!IMU.begin())
  { Serial.println("Failed to initialize IMU!");
    while (1);
  } 

   // note the FS value does not change the output of the read method, it just assigns more bits to the chip output,
   // increasing accuracy at the cost of a smaller range 
   IMU.setAccelFS(3);   // 0: ±245°/s  1: ±500°/s  2: ±1000°/s  Default= 3: ±2000°/s
   Serial.print("Gyroscope Full Scale = ±");
   Serial.print(IMU.getGyroFS());
   Serial.println ("°/s");

   // Change the sample frequency ( ODR = Output Dats rate)  
   IMU.setAccelODR(3);    // Output Data Rate 0:off, 1:10Hz, 2:50Hz, Default= 3:119Hz, 4:238Hz, 5:476Hz, (not working 6:952Hz) 
   Serial.print("Gyroscope sample rate = ");
   Serial.print(IMU.getGyroODR());        //alias  IMU.gyroscopeSampleRate());
   Serial.println(" Hz");

   IMU.gyroUnit= DEGREEPERSECOND;  //   DEGREEPERSECOND  RADIANSPERSECOND  REVSPERMINUTE  REVSPERSECOND  
   Serial.println("Gyroscope in degrees/second \n");

   delay(4000);
   Serial.println("X\tY\tZ");
}

void loop() {
  float x, y, z;

  if (IMU.gyroscopeAvailable())
  {  
     IMU.readGyroscope(x, y, z);
     Serial.print(x);
     Serial.print('\t');
     Serial.print(y);
     Serial.print('\t');
     Serial.println(z);
  }
}
