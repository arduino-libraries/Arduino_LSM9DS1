/* DIY calibration program for the LSM9DS1 chip 
 *  
 * Follow the instructions on the screen how to do calibration measurements. 
 * Menu operation: type a letter in the input box of the serial monitor followed by enter.
 * See instruction video https://youtu.be/BLvYFXoP33o
 * No special tools or setups are needed, however it is handy if the board with the LSM9DS1 chip is fitted inside 
 * a non-metalic rectangular box.
 * 
 * Each new instance of the LSM9DS1 chip will require it's own unique set of calibration factors. 
 * For an improved accuracy it is recommended that the sketch and the calibration program use the same settings for  
 * Full Scale (FS) and Output Data rate (ODR). This is the reasin why the possibility was added in this program.
 * The settings and calibration factors are displayed on screen as code that can be copy/pasted 
 * directly into a sketch.  
 * 
 *         Gyroscope Calibration
 * The program starts with a short menu that offers the possibility to change the FS and ODR value.
 * Next the Offset must be measured. This takes only a few seconds, during which the board must be kept still.  
 * This also enables the next step, which is to calibrate the slope. During a slope calibration the sensor must be rotated 
 * calmly about one axis over a known angle. This angle can be changed to your convenience. A larger angle is more accurate, 
 * but more difficult to do. The rotation can be done by hand. It must must be pure, without much rotation about the other 
 * two axes. e.g. by keeping the board on a flat surface with the rotation axis vertical while turning is good enough.  
 * Press Enter to finish the measurement.    
 * Each of the axes X,Y and Z must be measured, so in total three measurements. The program automatically detects which
 * and shows this on screen. The rotation direction clockwise or anti-clockwise is unimportant, both directions will work. 
 * It is important that the offset is measured before the slope calibration. If for some reason the offset has to be remeasured
 * make sure you remeasure the slope as well
 * 
 * 
 * written by Femme Verbeek 6 July 2020
 * 
 * This program uses V2 of the LSM9DS1 library 
 * Tested on an Arduino Nano 33 BLE Sense board.
 */

#include <Arduino_LSM9DS1.h> 

const float gyroSlopeCriterion = 50;  //Smaller value requires more pureness of the rotation  
char xyz[3]= {'X','Y','Z'};
boolean gyroOffsetOK=false;
boolean gyroSlopeOK[3]={false,false,false};
uint8_t gyroODRindex=5;
uint8_t gyroFSindex=2;   // (0= ±245 dps; 1= ±500 dps; 2= ±1000 dps; 3= ±2000 dps)

void setup() {
  Serial.begin(115200); 
  while (!Serial);
  pinMode(LED_BUILTIN,OUTPUT); 
  delay(10);
  if (!IMU.begin()) { Serial.println(F("Failed to initialize IMU!"));while (1); }
  IMU.setGyroODR(gyroODRindex);   //238Hz   
  IMU.setGyroFS(gyroFSindex);
  calibrateGyroMenu();
}

void loop() { }

/*void printParam(char txt[], float param[3])
{   for (int i= 0; i<=2 ; i++) 
    {  Serial.print(txt);Serial.print("[");
       Serial.print(i);Serial.print("] = "); 
       Serial.print(param[i],6);Serial.print(";");
    }
}*/

void printSetParam(char txt[], float param[3])
{   Serial.print(txt);Serial.print("(");
    Serial.print(param[0],6);Serial.print(", ");
    Serial.print(param[1],6);Serial.print(", ");
    Serial.print(param[2],6);Serial.print(");");
}

//**********************************************************************************************************************************
//*********************************************              Gyroscope                **********************************************
//**********************************************************************************************************************************


void calibrateGyroMenu()
{ char incomingByte = 0;
  byte b;
  uint16_t turnangle = 180;
  uint16_t NofCalibrationSamples = 1000;
  while (1)//    (incomingByte!='X') 
  {
    if (!gyroOffsetOK) 
    {  Serial.println(F("\n\nStep 1       CALIBRATE GYROSCOPE OFFSET "));
       Serial.println(F("First choose the sample frequency (ODR) and the Full Scale value. The accelerometer and the gyroscope"));  
       Serial.println(F("share their ODR, so the setting here must be the same as in the DIY_Calibration_Gyroscope sketch."));  
       Serial.println(F("This is far more important for the Gyroscope than for the accelerometer. "));  
       Serial.println(F("Next enter \"O\" to start the gyroscope offset measurement. \nDuring this offset measurement the sensor must be kept still.\n"));  
    } else   
    {  Serial.println(F("\n\nStep 2       CALIBRATE GYROSCOPE SLOPE")); 
       Serial.println(F("During a slope calibration the sensor must be rotated calmly about one axis over a known angle."));
       Serial.println(F("Change the angle to your convenience. A larger angle is more accurate, but more difficult to do."));
       Serial.println(F("The rotation must be pure, without much rotation about the other two axes. It can be done by hand."));
       Serial.println(F("Keeping the board on a flat surface with the rotation axis vertical while turning is good enough."));
       Serial.println(F("For an accurate result you can start and end with its side pushed against a non moving object.")); 
       Serial.println(F("When you're done turning the sensor, press (Enter) to stop measuring. Each of the axes X,Y and Z "));
       Serial.println(F("must be measured. The program automatically detects which. \n"));

       Serial.println(F(" (A) Change the measuring angle to turn the board"));
       Serial.print  (F(" (C) Calibrate Slope ==>> Turn the board over "));Serial.print(turnangle);Serial.println(F("° and press enter when finished "));
    }  Serial.print  (F(" (F) Full Scale setting "));Serial.print(gyroFSindex);Serial.print(" = "); Serial.print(IMU.getGyroFS(),0);Serial.println(F("°/s"));
       Serial.print  (F(" (R) Output Data Rate (ODR) setting "));Serial.print(gyroODRindex);Serial.print(" = ");Serial.print(IMU.getGyroODR(),0);Serial.println(F("Hz (actual value)"));
       Serial.print  (F(" (N) Number of calibration samples "));Serial.println(NofCalibrationSamples);
       Serial.println(F(" (O) Calibrate Offset (keep board still during measurement)"));
     
       Serial.println(F("\nOffset calibration ( -OK- )"));
       Serial.print  (F("Slope calibration axis  " ));
       for (int i= 0; i<=2 ; i++) 
       {   Serial.print(xyz[i]);
           if (gyroSlopeOK[i]) Serial.print(F("= ( -OK- ) ")); else Serial.print(F("= not done ")); 
       }
    Serial.println(F("\n\n   // Gyroscope code"));
    Serial.print  (F    ("   IMU.setGyroFS("));   Serial.print(gyroFSindex);
    Serial.print(  F(");\n   IMU.setGyroODR("));Serial.print(gyroODRindex);Serial.println(");");
    printSetParam("   IMU.setGyroOffset ",IMU.gyroOffset); 
    Serial.println();
    printSetParam("   IMU.setGyroSlope ",IMU.gyroSlope); 
    Serial.println();
    incomingByte= readChar();
    switch (incomingByte)
    { case 'A': { readAnswer("\n\n\n\n\n\nMeasurement turnangle for the board ", turnangle);
                  break;}
      case 'C': { Serial.print(F("\n\n\n\n\n\nMeasuring. Turn the sensor over "));Serial.print(turnangle);Serial.println(F(" degrees\n"));
                  Serial.println(F("Press Enter when finished."));
                  calibrateGyroslope(turnangle);
                  break;}
      case 'F': { Serial.print (F("\n\nEnter new FS nr  0: ±245 1: ±500 2: ±1000 3:± 2000 dps > ")); 
                  b= readChar()-48; Serial.println(b);
                  if (b!=gyroFSindex && b >=0 && b<=3) gyroFSindex=b;
                  IMU.setGyroFS(gyroFSindex); 
                  gyroOffsetOK=false;
                  Serial.print("\n\n\n"); 
                  break;}
      case 'R': { Serial.print (F("\n\nEnter new ODR nr   1:10,2:50 3:119,4:238,5:476 Hz> ")); 
                  b= readChar()-48; //Serial.println(b);
                  if (b!=gyroODRindex && b>=1 && b<=5) 
                  {   gyroODRindex=b;
                      IMU.setGyroODR(gyroODRindex);
                      gyroOffsetOK=false;
                  }    
                  Serial.print("\n\n\n"); 
                  break;
                }  
      case 'N': { readAnswer("\n\n\n\n\n\nThe number of calibration samples ", NofCalibrationSamples);
                  break;}
      case 'O': { calibrateGyroOffset(NofCalibrationSamples);     }
    }    
   Serial.println(""); 
  }
}

void calibrateGyroOffset(uint16_t N)  // don't move the board during calibration
{  float x, y, z;// , addX=0, addY=0, addZ=0  ;
   Serial.println(F("\n\n\n\nMeasuring offset. Just a moment.")); 
   Serial.println(F("\n\nKeep the board still during measurement")); 
   raw_N_Gyro(N,x,y,z);
   IMU.setGyroOffset(x, y,z); // Store the average measurements as offset
   Serial.print("\n\n\n\n\n"); 
   gyroOffsetOK=true;
}   
 
void calibrateGyroslope(unsigned int turnangle)  // rotate board over known angle
{  boolean validMmt=false;
   float dirX=0, dirY=0, dirZ=0,sigmaX2=0,sigmaY2=0,sigmaZ2=0;
   float x, y, z, maxXYZ;
   unsigned int count=0;
   while (!Serial.available())         // measure until enter key pressed
   {  while (!IMU.gyroAvailable());
       IMU.readRawGyro(x, y, z);
       dirX += (x - IMU.gyroOffset[0])/IMU.getGyroODR();    // slope is still raw but offset must already be calibrated 
       dirY += (y - IMU.gyroOffset[1])/IMU.getGyroODR(); 
       dirZ += (z - IMU.gyroOffset[2])/IMU.getGyroODR();
       sigmaX2 +=x*x;  sigmaY2+=y*y;  sigmaZ2+=z*z;
       if (count==0) maxXYZ= abs(x);
       maxXYZ= max(maxXYZ,abs(x)); maxXYZ= max(maxXYZ,abs(y)); maxXYZ= max(maxXYZ,abs(z)); 
       count++;
       if ((count%30)==0)Serial.print('.'); 
       digitalWrite(LED_BUILTIN, (millis()/125)%2);       // blink onboard led every 250ms
   }
   digitalWrite(LED_BUILTIN,0);       // led off
   Serial.readStringUntil(13);        //Empty read buffer
   Serial.print(F("\n\n\nMeasured direction change X ")); 
   Serial.print(dirX,6);Serial.print("°\tY "); Serial.print(dirY,6);Serial.print("°\t Z "); Serial.println(dirZ,6);Serial.print("°");          
   sigmaX2 /= count;    sigmaY2 /= count;    sigmaZ2 /= count;
   Serial.print(F("Std.dev. "));  
   Serial.print(sigmaX2,6);Serial.print('\t'); Serial.print(sigmaY2,6);Serial.print('\t'); Serial.println(sigmaZ2,6);          
   Serial.print(F("percentage of Full Scale ")); Serial.print(100*maxXYZ/IMU.getGyroFS());Serial.println('%');          
   if (maxXYZ/IMU.getGyroFS()>0.95) Serial.print(F("Maximum rotation speed reached. Choose a different FS setting or turn more slowly."));
   else
   { dirX=abs(dirX); dirY=abs(dirY); dirZ=abs(dirZ);
     if (dirX>max(dirY,dirZ))
     { if (sigmaY2<gyroSlopeCriterion && sigmaZ2<gyroSlopeCriterion )
       {  validMmt= true;
          IMU.gyroSlope[0]=turnangle/dirX;
          gyroSlopeOK[0]=true;
          Serial.print ("Valid measurement, X detected, slope ");  Serial.println (IMU.gyroSlope[0]);
       }
     }
     if (dirY>max(dirX,dirZ))
     { if (sigmaX2<gyroSlopeCriterion && sigmaZ2<gyroSlopeCriterion )
       {  validMmt= true;
          IMU.gyroSlope[1]=turnangle/dirY;
          gyroSlopeOK[1]=true;
          Serial.print ("Valid measurement, Y detected, slope ");Serial.println (IMU.gyroSlope[1]);
       }
     }
     if (dirZ>max(dirY,dirX))
     { if (sigmaY2<gyroSlopeCriterion && sigmaX2<gyroSlopeCriterion )
       {  validMmt= true;
          IMU.gyroSlope[2]=turnangle/dirZ;
          gyroSlopeOK[2]=true;
          Serial.print ("Valid measurement, Z detected, slope ");Serial.println (IMU.gyroSlope[2]);
       }
     }
   }  
   if ( !validMmt ) 
   { Serial.println(F("\n\nNot a valid measurement!")); 
     delay(2000);
   }
   else Serial.println("\n\n");
}


char readChar()
{  char ch;
   while (!Serial.available()) ;             // wait for character to be entered
   ch= toupper(Serial.read());   
   delay(10);
   while (Serial.available()){Serial.read();delay(1);} // empty readbuffer 
   return ch;
}

void readAnswer(char msg[], uint16_t& param)
{ char ch=0;
  byte count=0;
  const byte NofChars = 8;
  char ans[NofChars];
//  float val;
  while (Serial.available()){Serial.read();}  //empty read buffer
  Serial.print(msg); 
  Serial.print(param); 
  Serial.print(F(" Enter new value ")); 
  while (byte(ch)!=10 && byte(ch)!=13 && count<(NofChars-1)   )
  {  if (Serial.available())
     {  ch= Serial.read();
        ans[count]=ch;
        count++;
     }
  }      
  ans[count]=0;
  Serial.println(ans);
  if (count>1) param= atoi(ans);
  while (Serial.available()){Serial.read();}
     Serial.println("\n\n\n\n\n\n\n"); 
}

void raw_N_Gyro(unsigned int N, float& averX, float& averY, float& averZ) 
{    float x, y, z;
     averX=0; averY =0;averZ =0;
     for (int i=1;i<=N;i++)
     {  while (!IMU.gyroAvailable());
        IMU.readRawGyro(x, y, z);
        averX += x;    averY += y;     averZ += z;
        digitalWrite(LED_BUILTIN, (millis()/125)%2);       // blink onboard led every 250ms
        if ((i%30)==0)Serial.print('.'); 
     } 
     averX /= N;    averY /= N;     averZ /= N;
     digitalWrite(LED_BUILTIN,0);                         // led off
}
