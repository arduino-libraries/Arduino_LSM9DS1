/*
  Arduino LSM9DS1 - Simple Magnetometer
  Extended with library V2.0 function calls

  This example reads the magnetic field values from the LSM9DS1
  sensor and continuously prints them to the Serial Monitor
  or Serial Plotter.

  The circuit:
  - Arduino Nano 33 BLE Sense
  - or Arduino connected to LSM9DS1 breakout board

  created 10 Jul 2019
  by Riccardo Rizzo

  Modified by Femme Verbeek 
  10 July 2020

  This example code is in the public domain.
*/

#include <Arduino_LSM9DS1.h>
boolean viewInSerialPlotter=true;      // true optimises for serial plotter, false for serial monitor

void setup() 
{ Serial.begin(115200);
  while (!Serial);

  if (!IMU.begin()) 
  {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

/*****************   For a proper functioning of the magnetometer it needs to be calibrated            ********************
*****************   Replace the lines below by the output of the DIY_Calibration_Magnetometer sketch   ********************/
   IMU.setMagnetFS(0);  
   IMU.setMagnetODR(8); 
   IMU.setMagnetOffset(0,0,0);  //  uncalibrated
   IMU.setMagnetSlope (1,1,1);  //  uncalibrated
/******************************************************************************************************************************     
****  FS  Full Scale        range (0=±400 | 1=±800 | 2=±1200 | 3=±1600  (µT)                                              *****     
****  ODR Output Data Rate  range (6,7,8)=(40,80,400)Hz | not available on all chips (0..5): (0.625,1.25,2.5,5.0,10,20)Hz *****
*******************************************************************************************************************************/     
   IMU.magnetUnit = MICROTESLA;  //   GAUSS   MICROTESLA   NANOTESLA

   if (!viewInSerialPlotter)
   {  Serial.println("Magnetic Field in µT");
      Serial.print("Magnetometer Full Scale = ±");
      Serial.print(IMU.getMagnetFS());
      Serial.println ("µT");
      Serial.print("Magnetic field sample rate = ");
      Serial.print(IMU.getMagnetODR());        // alias IMU.magneticFieldSampleRate in library version 1.01
      Serial.println(" Hz");
      delay(4000);
   }
   Serial.println("X \t Y\t Z");
}

void loop() {
  float x, y, z;

  if (IMU.magnetAvailable())                   // alias IMU.magneticFieldAvailable in library version 1.01
  { IMU.readMagnet(x, y, z);                   // alias IMU.readMagneticField in library version 1.01

    Serial.print(x);
    Serial.print('\t');
    Serial.print(y);
    Serial.print('\t');
    Serial.println(z);
  }
}
