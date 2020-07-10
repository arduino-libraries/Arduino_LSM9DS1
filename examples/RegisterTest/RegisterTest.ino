/*Test program for Arduino__LSM9DS1 Library version 2.0 extensions
 * Written by Femme Verbeek Pijnacker the Netherlands 6 July 2020.
 * 
 * This program verifies that all the new LSM9DS1 set- and get- chip-setting-fuctions are working
 * 
 * The LSM9DS1 datasheet (https://www.st.com/resource/en/datasheet/lsm9ds1.pdf)
 * 
 * The following anomalies in the functioning of the LSM9DS1 chip have been found. 
 * 
 - Upon setAccelFS(1);  the needed multiplication factor should be 16, turns out to be 24, 
    but the sensor maxes out at 20 g  
 - The   setGyroFS(2);*  is *NA* according to the datasheet, but when tested it worked nicely at 1000 °/s 
 - The Gyroscope full scale setting does have a small effect on the offset that needs to be applied. So the
    workaround is to apply the same ODR setting when calibrating. See the testGyroFS function below.
 - The magnet ODR settings <= 20 Hz did not work on all systems. That means that the original library would
    not have worked on the failing systems. For this reason the default ODR setting was changed from 20 to 40Hz.
 - setAccelODR(6) and setGyroODR(6) should result in a sampling frequency of 952Hz, but it turned out to
   be smaller than 500Hz and with a lot of spikes in the signal. As this affected the accuracy at virtually 
   no gain in speed, it is advised not to use setting 6.
 * 
 * Additional anomalies found on an Arduino UNO with a separate LSM9DS1
 *
  - Settings 1 of setAccelODR( ) and setGyroODR ( ) did not work  
  - setMagnetODR did not work for settings 0 to 5 
 * 
 */

#include <Arduino_LSM9DS1.h>

float x,y,z;

void setup() {
   Serial.begin(115200);
   while(!Serial);         //Wait for serial connection
   delay(10);
   if (!IMU.begin()) {
    Serial.println(F("Failed to initialize IMU!"));
    while (1); }
   Serial.println(F("Testing all library chip settings\n"));
   Serial.println (F("Full Scale settings should hardly inluence measured values"));
   testAccelFS(); 
   testGyroFS();
   testMagnetFS();
   testOperationalMode();
   testGyroSharedODR();
   testAccelSharedODR();
   testAccelOnlyODR();
   testMagnetODR();
   testAccelAutomaticBW();
   testAccelBWOverride();
   testGyroBW();   
   testAccelUnitChange();
   testGyroUnitChange();
   testMagnetUnitChange();
}

void loop() {  }

//----------------------------------- FS  (Full scale) functions   -------------------------------------

void testAccelFS()
{   for (int i = 0;i<=4;i++)
    { if (IMU.setAccelFS(i))
      {  printResult ("\nsetAccelFS(", i ,IMU.getAccelFS()," g ");
         read_N_Accel(2,x,y,z);       //throw away first two samples
         read_N_Accel(50,x,y,z); 
         printXYZData("Accel uncalibrated ",x,y,z,"g ");
      }
      else Serial.println (F("\nsetAccelFS parameter out of range \n")); 
    }
}
void testGyroFS()
{
  IMU.setGyroODR(4);
  Serial.print("setGyroODR ("); Serial.print(4);Serial.print(") = "); Serial.print(IMU.getGyroODR());Serial.print("Hz ");  Serial.print(50);Serial.print(" samples");
      for (int i = 0;i<=4;i++)
      {  if (IMU.setGyroFS(i)) 
         { printResult ("\nsetGyroFS(",i,IMU.getGyroFS(),"°/s ");
           read_N_Gyro(2,true,x,y,z); //throw away first two samples
           read_N_Gyro(50,true,x,y,z); 
           IMU.setGyroOffset(x,y,z);   //Calibrate Offset
           printXYZData("Gyro raw ",x,y,z,"°/s ");
           read_N_Gyro(50,false,x,y,z); 
           printXYZData("Gyro calibrated ",x,y,z,"°/s ");
         }
         else Serial.println (F("\nsetGyroFS parameter out of range "));
      }  
}  

/*void testGyroFS()  // test combinations of ODR, FS and a number of samples in the average 
{
 for (int k=1;k<=6;k++)  
 {  IMU.setGyroODR(k);
     for (int j=2;j<=100;j*=1.5)
    { Serial.print("setGyroODR ("); Serial.print(k);Serial.print(") = "); Serial.print(IMU.getGyroODR());Serial.print("Hz ");  Serial.print(j);Serial.print(" samples");
      for (int i = 0;i<=4;i++)
      {  if (IMU.setGyroFS(i)) 
         { printResult ("\nsetGyroFS(",i,IMU.getGyroFS(),"°/s ");
           readRaw_N_Gyro(2,true,x,y,z); //throw away first two samples
           readRaw_N_Gyro(j,true,x,y,z); 
           IMU.setGyroOffset(x,y,z);   //Calibrate Offset
           printXYZData("Gyro raw ",x,y,z,"°/s ");
           read_N_Gyro(j,false,x,y,z); 
           printXYZData("Gyro calibrated ",x,y,z,"°/s ");
         }
         else Serial.println (F("\nsetGyroFS parameter out of range "));
      }
    }
  }  
} */

void testMagnetFS()
{   for (int i = 0;i<=4;i++)
    {  if (IMU.setMagnetFS(i))
      {  printResult ("\nsetMagnetFS(", i ,IMU.getMagnetFS()," µT ");
         read_N_Magnet(2,x,y,z);       //throw away first two samples
         read_N_Magnet(10,x,y,z); 
         printXYZData("Magnet ",x,y,z,"µT ");
      }
      else Serial.println (F("\nsetMagnetFS parameter out of range"));  
    }   
}    

//-------------------------------  ODR (Output Data Rate) functions  -----------------------------

void testOperationalMode()
{ Serial.println(F("\nTest Operational Mode 0=off, 1 accelerometer only, 2= Accel + Gyro \n"));    
  Serial.print(F("Default operational mode "));Serial.println(IMU.getOperationalMode()); 
  IMU.setGyroODR(0);
  Serial.print(F("setGyroODR(0) Operational mode "));Serial.println(IMU.getOperationalMode()); 
  IMU.setAccelODR(0);
  Serial.print(F("setAccelODR(0) Operational mode "));Serial.println(IMU.getOperationalMode()); 
  IMU.setGyroODR(3);
  Serial.print(F("setGyroODR(3) Operational mode "));Serial.println(IMU.getOperationalMode()); 
}

void testGyroSharedODR()
{  Serial.println(F("\n setGyroODR result (shared ODR, i>0 overrules the Accel setting)"));
   for (int i=0;i<=7;i++)
      if (IMU.setGyroODR(i)==0) {Serial.print(F("setGyroODR parameter out of range "));Serial.println(i); }
      else printODRData(i);
}

void testAccelSharedODR()  
{  Serial.println(F("\n setAccelODR result (shared ODR, i=0 switches off Gyro and Accel)")); 
  for (int i=0;i<=7;i++)
     if (IMU.setAccelODR(i)==0) {Serial.print(F("setAccelODR parameter out of range "));Serial.println(i);} 
     else printODRData(i);
}

void testAccelOnlyODR()
{ Serial.println(F("\n Switch off Gyro (Accelerometer only mode)")); 
  IMU.setGyroODR(0);         //switch off gyro
  for (int i=0;i<=7;i++)
     if (IMU.setAccelODR(i)==0) {Serial.print(F("setAccelODR parameter out of range "));Serial.println(i);} 
      else printODRData(i);
}

void testMagnetODR()        
{  Serial.println(F("\n setMagnetODR sample rate result")); 
  for (int i = 0;i<=16;i++)
  {    if (IMU.setMagnetODR(i))
         printResult ("setMagnetODR(", i , IMU.getMagnetODR()," Hz \n");
      else {Serial.print (F("setMagnetODR  parameter out of range "));Serial.println(i);}         
  }
}

//---------------------------------  BW (Band width)  functions  -------------------------------
void testAccelAutomaticBW()
{ Serial.println(F("\n Accelerometer automatic band width result ")); 
  for (int i=0;i<=6;i++)
  {  IMU.setAccelODR(i); 
     Serial.print(F("Accel ODR = "));Serial.print(IMU.getAccelODR()); 
     Serial.print(F(" automatic BW setting (Hz) " ));Serial.println (IMU.getAccelBW());     
  }    
}

void testAccelBWOverride()
{   Serial.println (F("\n Accelerometer band width override"));
   for (int i = 0;i<=4;i++)
   {  if (IMU.setAccelBW(i) )    // override automatic bandwith
         printResult("setAccelBW(", i ,IMU.getAccelBW(), "Hz \n" ); 
      else {Serial.print(F("setAccelBW parameter out of range "));Serial.println(i) ;}
   }
}
void testGyroBW()   
{  Serial.println (F("\n Gyroscope band width setting"));
   for (int i = 0;i<=6;i++)
   {   if (IMU.setGyroODR(i))
       {  Serial.print (F("Gyroscope ODR = " ));Serial.println( IMU.getGyroODR()  );
          for (int j = 0;j<=4;j++)
          {   if (IMU.setGyroBW(j) )    // the gyro has no automatic bandwith
                   printResult("setGyroBW(", j ,IMU.getGyroBW(), "Hz \n" );
             else {Serial.print (F("setGyroBW parameter out of range"));Serial.println(j);}
          }    
       }
       else Serial.println (F("failed setting gyroscope sample rate ")); 
   }
}   

//----------------------------------------  Changing Output units  -------------------------------

void testAccelUnitChange()
{  Serial.println (F("\n Changing Accelerometer output units "));
   IMU.setGyroODR(5);
   IMU.accelUnit= METERPERSECOND2;
   read_N_Accel(20,x, y, z); 
   printXYZData("\nreadAccel ",x,y,z,"m/s2 "); 
   IMU.accelUnit= GRAVITY;
   read_N_Accel(20,x, y, z); 
   printXYZData("\nreadAccel ",x,y,z,"g "); 
}   

void testGyroUnitChange()
{  Serial.print(F("\n  Simulate gyro measurement by offsetting 100 dps"));
   IMU.setGyroODR(5);
   read_N_Gyro(20,true,x, y, z); 
   IMU.setGyroOffset(100+x,100+y,100+z);   //Add 100 dps to measurement to simulate a measurement

   IMU.gyroUnit = DEGREEPERSECOND;
   read_N_Gyro(20,false,x, y, z); 
   printXYZData("\nreadGyro ",x,y,z," deg/s "); 
   IMU.gyroUnit = RADIANSPERSECOND;
   read_N_Gyro(20,false,x, y, z); 
   printXYZData("\nreadGyro ",x,y,z," rad/s "); 
   IMU.gyroUnit = REVSPERMINUTE;
   read_N_Gyro(20,false,x, y, z); 
   printXYZData("\nreadGyro ",x,y,z," RPM "); 
   IMU.gyroUnit = REVSPERSECOND;
   read_N_Gyro(20,false,x, y, z); 
   printXYZData("\nreadGyro ",x,y,z," RPS "); 
}

void testMagnetUnitChange()
{   IMU.magnetUnit = MICROTESLA;
   read_N_Magnet(20,x, y, z); 
   printXYZData("\nreadMagnet  ",x,y,z," µT "); 
   IMU.magnetUnit = NANOTESLA;
   read_N_Magnet(20,x, y, z); 
   printXYZData("\nreadMagnet  ",x,y,z," nT "); 
   IMU.magnetUnit = GAUSS;
   read_N_Magnet(20,x, y, z); 
   printXYZData("\nreadMagnet  ",x,y,z," G "); 
}


//---------------------------------------    print functions   -------------------------------   
void printResult (char msg[], int nr,float value, char dimension[])
{  Serial.print (msg);Serial.print(nr);
   Serial.print(F(") Setting "));Serial.print(value);
   Serial.print(dimension);
}
void printODRData(int i)
{  Serial.print(F("Settting "));Serial.print(i); 
   Serial.print(F(" Oper.mode "));Serial.print(IMU.getOperationalMode()); 
   Serial.print(F(" ODR Gyro "));Serial.print( IMU.getGyroODR(),3  );
   Serial.print(F("Hz Accel "));Serial.print( IMU.getAccelODR(),3  );
   Serial.println(F("Hz"));
}    
void printXYZData(char msg[], float x,float y,float z,char dimension[])
{  Serial.print (msg);
   Serial.print (x); Serial.print (dimension );
   Serial.print (y); Serial.print (dimension );
   Serial.print (z); Serial.print (dimension );
}

//--------------------------------------------   measurements   ----------------------------------
void read_N_Accel(unsigned int N, float& averX, float& averY, float& averZ) 
{    float x, y, z;
     averX=0; averY =0;averZ =0;
     for (int i=1;i<=N;i++)
     {  while (!IMU.accelAvailable());
        IMU.readAccel(x, y, z);
        averX += x/N;    averY += y/N;     averZ += z/N;
     } 
}
void read_N_Gyro(unsigned int N, boolean raw, float& averX, float& averY, float& averZ) 
{    float x, y, z;
     averX=0; averY =0;averZ =0;
     for (int i=1;i<=N;i++)
     {  while (!IMU.gyroAvailable());
        if (raw) IMU.readRawGyro(x, y, z);
        else IMU.readGyro(x, y, z);
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



  
