/*Test program for Arduino__LSM9DS1 Library version 2.0 extensions
 * Written by Femme Verbeek Pijnacker the Netherlands 30 may 2020.
 * Run through all the new set and get functions
 */

#include <Arduino_LSM9DS1.h>

void setup() {
   Serial.begin(115200);
   while(!Serial);         //Wait for serial connection
   delay(10);
   if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1); }
   float x,y,z;
   Serial.println("Testing all library chip settings\n");
   Serial.println ("Full Scale settings should hardly inluence measured values");
   for (int i = 0;i<=4;i++){
      if (IMU.setAccelFS(i))
      {  printResult ("setAccelFS(", i ,IMU.getAccelFS()," g ");
         read_N_Accel(2,x,y,z);       //throw away first two samples
         read_N_Accel(50,x,y,z); 
         printXYZData("Accel uncalibrated ",x,y,z,"g ");
      }
      else Serial.println ("setAccelFS parameter out of range \n"); } 

   read_N_Gyro(50,x, y, z);
   IMU.setGyroOffset(x,y,z);   //Calibrate Offset
   for (int i = 0;i<=4;i++){
      if (IMU.setGyroFS(i)) 
      { printResult ("setGyroFS(",i,IMU.getGyroFS()," deg/s ");
        read_N_Gyro(2,x,y,z); //throw away first two samples
        read_N_Gyro(50,x,y,z); 
        printXYZData("Gyro offset calibrated ",x,y,z,"dps ");
      }
      else Serial.println ("setGyroFS full scale parameter out of range \n");}
 
   for (int i = 0;i<=4;i++){
      if (IMU.setMagnetFS(i))
      {  printResult ("setMagnetFS(", i ,IMU.getMagnetFS()," µT ");
         read_N_Magnet(2,x,y,z);       //throw away first two samples
         read_N_Magnet(10,x,y,z); 
         printXYZData("Magnet ",x,y,z,"dps ");
      }
      else Serial.println ("setMagnetFS full scale parameter out of range \n"); } 

    Serial.println("\nTest Operational Mode 0=off, 1 accelerometer only, 2= Accel + Gyro \n");    
  Serial.println("Default operational mode "+String(IMU.getOperationalMode())); 
  IMU.setGyroODR(0);
  Serial.println("setGyroODR(0) Operational mode "+String(IMU.getOperationalMode())); 
  IMU.setAccelODR(0);
  Serial.println("setAccelODR(0) Operational mode "+String(IMU.getOperationalMode())); 
  IMU.setGyroODR(3);
  Serial.println("setGyroODR(3) Operational mode "+String(IMU.getOperationalMode())); 
 
  Serial.println("\n setGyroODR result (shared ODR, i>0 overrules the Accel setting)");
  for (int i=0;i<=7;i++)
    if (IMU.setGyroODR(i)==0) Serial.println("setGyroODR parameter out of range "+String(i)); 
    else printODRData(i);
   
  Serial.println("\n setAccelODR result (shared ODR, i=0 switches off Gyro and Accel)"); 
  for (int i=0;i<=7;i++)
     if (IMU.setAccelODR(i)==0) Serial.println("setAccelODR parameter out of range "+String(i)); 
     else printODRData(i);
      
  Serial.println("\n Switch off Gyro (Accelerometer only mode)"); 
  IMU.setGyroODR(0);         //switch off gyro
  for (int i=0;i<=7;i++)
     if (IMU.setAccelODR(i)==0) Serial.println("setAccelODR parameter out of range "+String(i)); 
      else printODRData(i);
        
  Serial.println("\n setMagnetODR sample rate result"); 
  for (int i = 0;i<=16;i++){
      if (IMU.setMagnetODR(i))
         printResult ("setMagnetODR(", i , IMU.getMagnetODR()," Hz \n");
      else Serial.println ("setMagnetODR  parameter out of range "+String(i));}         

  Serial.println("\n Accelerometer automatic band width result "); 
  for (int i=0;i<=6;i++)
  {  IMU.setAccelODR(i); 
     Serial.print("Accel ODR = "+String(IMU.getAccelODR())); 
     Serial.println(" automatic BW setting (Hz) "+ String (IMU.getAccelBW()));     
  }    

   Serial.println ("\n Accelerometer band width override");
   for (int i = 0;i<=4;i++)
   {  if (IMU.setAccelBW(i) )    // override automatic bandwith
         printResult("setAccelBW(", i ,IMU.getAccelBW(), "Hz \n" ); 
      else Serial.println ("setAccelBW parameter out of range "+String(i) );
   }
   
   Serial.println ("\n Gyroscope band width setting");
   for (int i = 0;i<=6;i++)
   {   if (IMU.setGyroODR(i))
       {  Serial.println ("Gyroscope ODR = " + String( IMU.getGyroODR() ) );
          for (int j = 0;j<=4;j++)
          {   if (IMU.setGyroBW(j) )    // the gyro has no automatic bandwith
                   printResult("setGyroBW(", j ,IMU.getGyroBW(), "Hz \n" );
             else Serial.println ("setGyroBW parameter out of range"+String(j));
          }    
       }
       else Serial.println ("failed setting gyroscope sample rate "); 
   }

  Serial.println ("\n Change output Units changes the value of read... \n");

   Serial.print("Change accel output unit to m/s2  ");
   IMU.accelUnit= METERPERSECOND2;
   read_N_Accel(20,x, y, z); 
   printXYZData("readAccel uncalibrated ",x,y,z,"m/s2 "); 

   Serial.print("Change accel output unit to g  ");
   IMU.accelUnit= GRAVITY;
   read_N_Accel(20,x, y, z); 
   printXYZData("readAccel uncalibrated ",x,y,z,"g "); 

   Serial.print("\nSimulate gyro measurement by offsetting 100 dps  ");
   IMU.setGyroOffset(0,0,0);   //Zero offset
   read_N_Gyro(20,x, y, z); 
   IMU.setGyroOffset(100+x,100+y,100+z);   //Add 100 dps to measurement to simulate a measurement

   Serial.print("\nChange gyro output unit to deg/s  ");
   IMU.gyroUnit = DEGREEPERSECOND;
   read_N_Gyro(20,x, y, z); 
   printXYZData("readGyro offset  ",x,y,z,"deg/s "); 
   
   Serial.print("Change gyro output unit to rad/s ");
   IMU.gyroUnit = RADIANSPERSECOND;
   read_N_Gyro(20,x, y, z); 
   printXYZData("readGyro offset  ",x,y,z,"rad/s "); 

   Serial.print("Change gyro output unit to Rev/M ");
   IMU.gyroUnit = REVSPERMINUTE;
   read_N_Gyro(20,x, y, z); 
   printXYZData("readGyro offset  ",x,y,z,"RPM "); 

   Serial.print("Change gyro output unit to Rev/s ");
   IMU.gyroUnit = REVSPERSECOND;
   read_N_Gyro(20,x, y, z); 
   printXYZData("readGyro offset  ",x,y,z,"RPS "); 

   Serial.print("\nChange Magnet output unit to microTesla ");
   IMU.magnetUnit = MICROTESLA;
   read_N_Magnet(20,x, y, z); 
   printXYZData("readMagnet  ",x,y,z,"µT "); 
 
   Serial.print("Change Magnet output unit to nanoTesla  ");
   IMU.magnetUnit = NANOTESLA;
   read_N_Magnet(20,x, y, z); 
   printXYZData("readMagnet  ",x,y,z,"nT "); 

   Serial.print("Change Magnet output unit to Gauss      ");
   IMU.magnetUnit = GAUSS;
   read_N_Magnet(20,x, y, z); 
   printXYZData("readMagnet  ",x,y,z,"G "); 

}

void loop() {  }

void printResult (String msg, int nr,float value, String dimension)
{  Serial.print (msg+String(nr));
   Serial.print(") Setting "+String(value));
   Serial.print(dimension);
}
void printODRData(int i)
{  Serial.print("Settting "+String(i)); 
   Serial.print(" Oper.mode "+String(IMU.getOperationalMode())); 
   Serial.print(" ODR Gyro "+String( IMU.getGyroODR(),3  ) );
   Serial.print("Hz Accel "+String( IMU.getAccelODR(),3  ) );
   Serial.println("Hz");
}    
void printXYZData(String msg, float x,float y,float z,String unit)
{  Serial.print (msg);
   Serial.print (x+unit );
   Serial.print (y+unit );
   Serial.println (z+unit );
}

void read_N_Accel(unsigned int N, float& averX, float& averY, float& averZ) 
{    float x, y, z;
     averX=0; averY =0;averZ =0;
     for (int i=1;i<=N;i++)
     {  while (!IMU.accelAvailable());
        IMU.readAccel(x, y, z);
        averX += x/N;    averY += y/N;     averZ += z/N;
     } 
}
void read_N_Gyro(unsigned int N, float& averX, float& averY, float& averZ) 
{    float x, y, z;
     averX=0; averY =0;averZ =0;
     for (int i=1;i<=N;i++)
     {  while (!IMU.gyroAvailable());
        IMU.readGyro(x, y, z);
        averX += x/N;    averY += y/N;     averZ += z/N;
     } 
}
void read_N_Magnet(unsigned int N, float& averX, float& averY, float& averZ) 
{    float x, y, z;
     averX=0; averY =0;averZ =0;
     for (int i=1;i<=N;i++)
     {  while (!IMU.magnetAvailable());
        IMU.readMagnet(x, y, z);
        averX += x/N;    averY += y/N;     averZ += z/N;
     } 
}



  
