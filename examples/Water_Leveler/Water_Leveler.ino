/* Water level example for the Nano 33 BLE (Sense)
 * You need version 2.0 or higher of the LMS9DS1 library to run this example 
 * 
 * Calibration makes the difference between a few degrees and within a degree accuracy
 * Run the DIY calibration program first and copy/paste the Accelerometer calibration data below where it's indicated.
 * 
 * Written by Femme Verbeek 
 *     6 June 2020  
 * Released to the public domain
*/
#include <Arduino_LSM9DS1.h>

void setup() {
   Serial.begin(115200);
   while(!Serial);   //wait for a serial connection
   delay(1);
   if (!IMU.begin()) {Serial.println("Failed to initialize IMU!"); while (1);  }
   IMU.setAccelODR(5);  //476 Hz

//*****************  Replace the lines below by the accelerometer output of the DIY Calibration program   *******************

   IMU.accelOffset[0] = 0;   IMU.accelOffset[1] = 0;   IMU.accelOffset[2] = 0;  
   IMU.accelSlope [0] = 1;   IMU.accelSlope [1] = 1;   IMU.accelSlope [2] = 1;  
  
//****************************************************************************************************************************     
   Serial.println("pitchX\tpitchY"); //shows in the legend of the serial plotter.
}

void loop() 
{ float x, y, z, pitchx, pitchy;
  read_N_Accel(50,x,y,z);
  if (abs(x)>0.1 || abs(z)>0.1) pitchx = atan2(x,z)*180/PI; 
  else pitchx=0;
  if (abs(y)>0.1 || abs(z)>0.1) pitchy = atan2(y,z)*180/PI;
  else pitchy=0;
  Serial.print(pitchx, 1);
  Serial.print('\t');  
  Serial.println(pitchy, 1);
}

void read_N_Accel(unsigned int N, float& averX, float& averY, float& averZ) 
{    float x, y, z;
     averX=0; averY =0;averZ =0;
     for (int i=1;i<=N;i++)
     {  while (!IMU.accelAvailable());
        IMU.readAccel(x, y, z);
        averX += x;    averY += y;     averZ += z;
     } 
     averX /= float(N);    averY /= float(N);     averZ /= float(N);
}
