/*
 * Arduino LSM9DS1 - RPM Meter and Rev counter.
 * Example for V2 of the LSM9DS1 library.
 * 
 * Writtenby Femme Verbeek  14 july 2020
 * 
 * This program demonstrates the use of the LSM9DS1 Gyroscope 
 * When placed on a record turntable it will show the rotation speed in RPM, 33.33 or 45
 * It counts the revolutions by integrating the Gyroscope signal.
 * 
 * Run the DIY calibration program to get the gyroOffset an gyroSlope settings and paste their values below
 * where it is indicated
 * 
 * This example code is in the public domain.
 */

#include <Arduino_LSM9DS1.h>

void setup() 
{  Serial.begin(115200); 
   while (!Serial);
   if (!IMU.begin()) 
   {   Serial.println("Failed to initialize IMU!");
       while (1); 
   }

   IMU.gyroUnit =  REVSPERMINUTE ;          //change output unit of readGyro

//*******   The gyroscope needs to be calibrated. Offset controls drift and Slope scales the measured rotation angle  *********
//*****************   Copy/Replace the lines below by the output of the DIY_Calibration_Gyroscope sketch   ********************
   IMU.setGyroFS(3);
   IMU.setGyroODR(5);
   IMU.setGyroOffset (0,0,0);  // = uncalibrated
   IMU.setGyroSlope  (1,1,1);  // = uncalibrated
//*****************************************************************************************************************************     
//*********  FS  Full Scale     range  0: ±245°/s  1: ±500°/s  2: ±1000°/s  Default= 3: ±2000°/s    ***************************
//*********  ODR Output Data Rate range  0:off, 1:10Hz, 2:50Hz, Default= 3:119Hz, 4:238Hz, 5:476Hz, (not working 6:952Hz) *****
//*****************************************************************************************************************************     

   Serial.println("RPM_X \t RPM_Y \t RPM_Z \t Revs_X \t Revs_y \t Revs_Z ");   // legend in case you use the serial plotter
}

float countRevsX=0, countRevsY=0, countRevsZ=0;

void loop() 
{  
   int nrOfSamples=200;
   float measureTime= nrOfSamples/(IMU.getGyroODR()*60);     //in min
   float x, y, z;
   read_N_Gyro(nrOfSamples,x,y,z);
   countRevsX += x * measureTime;   
   countRevsY += y * measureTime;
   countRevsZ += z * measureTime;

    Serial.print(x);
    Serial.print('\t');
    Serial.print(y);
    Serial.print('\t');
    Serial.print(z);
    Serial.print('\t');
    Serial.print(countRevsX);
    Serial.print('\t');
    Serial.print(countRevsY);
    Serial.print('\t');
    Serial.println(countRevsZ);
}


void read_N_Gyro(unsigned int N, float& averX, float& averY, float& averZ) 
{    float x, y, z;
     averX=0; averY =0;averZ =0;
     for (int i=1;i<=N;i++)
     {  while (!IMU.gyroAvailable());
        IMU.readGyro(x, y, z);
        averX += x;    averY += y;     averZ += z;
     } 
     averX /= N;    averY /= N;     averZ /= N;
}
