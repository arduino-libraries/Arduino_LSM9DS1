/* Water level example for the Nano 33 BLE (Sense)
 * You need version 2.0 or higher of the LMS9DS1 library to run this example 
 * 
 * Calibration makes the difference between a few degrees and within a degree accuracy
 * Run the DIY calibration program first and copy/paste the Accelerometer calibration data below where it's indicated.
 * 
 *   The circuit:
 *  - Arduino Nano 33 BLE (Sense)
 *  - or Arduino connected to LSM9DS1 breakout board
 * 
 * Written by Femme Verbeek 
 *     6 June 2020  
 * Released to the public domain
*/
#include <Arduino_LSM9DS1.h>

void setup()
{
   Serial.begin(115200);
   while(!Serial);   //wait for a serial connection
   delay(1);
   if (!IMU.begin()) {Serial.println("Failed to initialize IMU!"); while (1);  }

/*****************  For an improved accuracy run the DIY_Calibration_Accelerometer sketch first.   ****************
*****************       Copy/Replace the lines below by the code output of the program            ****************/
   IMU.setAccelFS(2);           
   IMU.setAccelODR(3);            
   IMU.setAccelOffset(0, 0, 0);  // 0,0,0 = uncalibrated
   IMU.setAccelSlope (1, 1, 1);  // 1,1,1 = uncalibrated
/***********************************************************************************************************************************
*******  FS  Full Scale         range 0:±2g | 1:±24g | 2: ±4g | 3: ±8g  (default=2)                                           ******
*******  ODR Output Data Rate   range 0:off | 1:10Hz | 2:50Hz | 3:119Hz | 4:238Hz | 5:476Hz, (default=3)(not working 6:952Hz) ******
************************************************************************************************************************************/

   Serial.println(" pitchX \t pitchY"); //shows in the legend of the serial plotter.
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
