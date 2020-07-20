/*
  Arduino LSM9DS1 - Simple Accelerometer
  Extended with library V2.0 function calls

  This example reads the acceleration values from the LSM9DS1
  sensor and continuously prints them to the Serial Monitor
  or Serial Plotter.

  The circuit:
  - Arduino Nano 33 BLE (Sense)
  - or Arduino Uno connected to LSM9DS1 breakout board

  created 10 Jul 2019
  by Riccardo Rizzo

  Modified by Femme Verbeek 10 jul 2020

  This example code is in the public domain.
*/

#include <Arduino_LSM9DS1.h>
boolean viewInSerialPlotter=true;      //true optimises for serial plotter, false for serial monitor

void setup() 
{  Serial.begin(115200);
   while (!Serial);
 
   if (!IMU.begin())
   { Serial.println("Failed to initialize IMU!");
     while (1);
   }
/*******************    For an improved accuracy run the DIY_Calibration_Accelerometer sketch first.     ****************
********************         Copy/Replace the lines below by the code output of the program              ****************/
   IMU.setAccelFS(3);           
   IMU.setAccelODR(5);           // 
   IMU.setAccelOffset(0, 0, 0);  //   uncalibrated
   IMU.setAccelSlope (1, 1, 1);  //   uncalibrated
/***********************************************************************************************************************************
*******  FS  Full Scale         range 0:±2g | 1:±24g | 2: ±4g | 3: ±8g  (default=2)                                           ******
*******  ODR Output Data Rate   range 0:off | 1:10Hz | 2:50Hz | 3:119Hz | 4:238Hz | 5:476Hz, (default=3)(not working 6:952Hz) ******
************************************************************************************************************************************/
   IMU.accelUnit=  GRAVITY;    // or  METERPERSECOND2    
   
   if (!viewInSerialPlotter)
   {  Serial.println("Gyroscope in degrees/second \n");
      Serial.print("Accelerometer Full Scale = ±");
      Serial.print(IMU.getAccelFS());
      Serial.println ("g");
      Serial.print("Accelerometer sample rate = ");
      Serial.print(IMU.getAccelODR());        // alias  AccelerationSampleRate());
      Serial.println(" Hz \n");
      delay(4000);
   }
   Serial.println(" X \t Y \t Z ");
}

void loop() {
  float x, y, z;

  if (IMU.accelAvailable())                   // alias IMU.accelerationAvailable in library version 1.01
  {  IMU.readAccel(x, y, z);                  // alias IMU.readAcceleration  in library version 1.01

     Serial.print(x);
     Serial.print('\t');
     Serial.print(y);
     Serial.print('\t');
     Serial.println(z);
  }
}
