/*
  Arduino LSM9DS1 - Simple Accelerometer
  Extended with library V2.0 function calls

  This example reads the acceleration values from the LSM9DS1
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

void setup() 
{  Serial.begin(115200);
   while (!Serial);
 
   if (!IMU.begin())
   { Serial.println("Failed to initialize IMU!");
     while (1);
   }
   // note the FS value does not change the output of the read method, it just assigns more bits to the chip output,
   // increasing accuracy at the cost of a smaller range 
   IMU.setAccelFS(2);   // 0: ±2g ; 1: ±24g ; 2: ±4g ; 3: ±8g  (default =2)
   Serial.print("Accelerometer Full Scale = ±");
   Serial.print(IMU.getAccelFS());
   Serial.println ("g");

   // Change the sample frequency ( ODR = Output Dats rate)  
   IMU.setAccelODR(3);  // Output Data Rate 0:off, 1:10Hz, 2:50Hz, default = 3:119Hz, 4:238Hz, 5:476Hz, (not working 6:952Hz) 
   Serial.print("Accelerometer sample rate = ");
   Serial.print(IMU.getAccelODR());         // alias  AccelerationSampleRate());
   Serial.println(" Hz \n");

   IMU.accelUnit=  GRAVITY;    // or  METERPERSECOND2    
   Serial.println("Gyroscope in degrees/second \n");

 
   Serial.println("X\tY\tZ");
}

void loop() {
  float x, y, z;

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);

    Serial.print(x);
    Serial.print('\t');
    Serial.print(y);
    Serial.print('\t');
    Serial.println(z);
  }
}
