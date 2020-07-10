/*
  Arduino LSM9DS1 - Simple Magnetometer
  Extended with library V2.0 function calls

  This example reads the magnetic field values from the LSM9DS1
  sensor and continuously prints them to the Serial Monitor
  or Serial Plotter.

  The circuit:
  - Arduino Nano 33 BLE Sense
  - or Arduino Uno connected to LSM9DS1 breakout board

  created 10 Jul 2019
  by Riccardo Rizzo

  Modified by Femme Verbeek 
  10 July 2020

  This example code is in the public domain.
*/

#include <Arduino_LSM9DS1.h>

void setup() 
{ Serial.begin(115200);
  while (!Serial);

  if (!IMU.begin()) 
  {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

   // note the FS value does not change the output of the read method, it just assigns more bits to the chip output,
   // increasing accuracy at the cost of a smaller range 
   IMU.setMagnetFS(0); //   1=±800µT   2=±1200µT  3=±1600µT  Default= 0:±400 µT 
   Serial.print("Magnetometer Full Scale = ±");
   Serial.print(IMU.getMagnetFS());
   Serial.println ("µT");

   // Change the sample frequency ( ODR = Output Dats rate)  
   // Note: setting 0..5 did not work on all systems. The default setting = 5 for compatibility reasons 
   IMU.setMagnetODR(6);   // Sampling rate (0..8)->{0.625,1.25,2.5,5.0,10,20,40,80,400}Hz  
   Serial.print("Magnetic field sample rate = ");
   Serial.print(IMU.getMagnetODR());                       // alias IMU.magneticFieldSampleRate());
   Serial.println(" Hz");
  
   IMU.magnetUnit = MICROTESLA;  //   GAUSS   MICROTESLA   NANOTESLA
   Serial.println("Magnetic Field in µT");
   
   Serial.println("X\tY\tZ");
}

void loop() {
  float x, y, z;

  if (IMU.magneticFieldAvailable()) {
    IMU.readMagneticField(x, y, z);

    Serial.print(x);
    Serial.print('\t');
    Serial.print(y);
    Serial.print('\t');
    Serial.println(z);
  }
}
