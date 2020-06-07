/* Compass example for the Nano 33 BLE (Sense)
 * You need version 2.0 or higher of the LMS9DS1 library to run this example 
 * 
 * The compass must be calibrated for the magnetic disturbance of the setup and the environment.
 * Run the DIY calibration program first and copy/paste the Magnetometer calibration data below where it's indicated.
 * 
 * Written by Femme Verbeek 
 *     6 June 2020  
 * Released to the public domain
*/

#include <Arduino_LSM9DS1.h>

void setup() {
     Serial.begin(115200);
     while(!Serial);                     // wait till the serial monitor connects
     delay(1);
     if (!IMU.begin()) {  
        Serial.println("Failed to initialize IMU!");  
        while (1); }
        IMU.setMagnetODR(8);         // Sample Rate (0..7) corresponds to {0.625,1.25,2.5,5,10,20,40,80,400}Hz

//********************  Replace the lines below by the magnetic output of the DIY Calibration program   *********************

   IMU.magnetOffset[0] = 0; IMU.magnetOffset[1] = 0; IMU.magnetOffset[2] = 0; 
   IMU.magnetSlope [0] = 1; IMU.magnetSlope [1] = 1; IMU.magnetSlope [2] = 1; 

//****************************************************************************************************************************     
   Serial.println(" Heading \t Inclination \t Strength \t mag.X \t mag.Y \t mag.Z ");  
}

void loop ()
{  float magY,magX,magZ;
   doNMeasurements (50,magX,magY,magZ);                                  //Average measurements to reduce noise
   float heading= atan2(magY,magX)*180/PI +180;
   float fieldStrength = sqrt(magX*magX +magY*magY+magZ*magZ);
   float inclination = atan(-magZ/sqrt(magX*magX +magY*magY)) *180/PI;    // by definition a positive inclination means the Z component is negative
   Serial.print(String(heading)+'\t'); Serial.print(String(inclination)+'\t'); Serial.print(String(fieldStrength)+'\t');
   Serial.print(String(magX)+"µT\t");    Serial.print(String(magY)+"µT\t");        Serial.print(String(magZ)+"µT\n");
}

void doNMeasurements(unsigned int N, float& averX, float& averY, float& averZ) 
{    float x, y, z;
     averX=0; averY =0; averZ =0;
     for (int i=1;i<=N;i++)
     { while (!IMU.magneticFieldAvailable());
        IMU.readMagneticField(x, y, z);
        averX += x; averY += y;  averZ += z;
     } 
     averX /= N;    averY /= N;  averZ /= N;
}
 
