/*          DIY calibration program for the LSM9DS1 chip 
 *  
 * Follow the instructions on the screen how to do calibration measurements. 
 * See instruction video https://youtu.be/BLvYFXoP33o
 * No special tools or setups are needed, however it is handy if the board with the LSM9DS1 chip is fitted inside 
 * a non-metalic rectangular box with flat sides.
 * The offset and slope factors are displayed on screen as code that can be copy/pasted directly into a sketch.  
 * Each new instance of the chip will require it's own unique set of calibration factors. 
 * 
 * To operate the menu in the IDE serial monitor you must press a letter key followed by Enter 
 * 
 *                     Magnetometer
 * During calibration of the Magnetometer each of it's sensor axes X, Y and Z must be aimed in both ways (positive 
 * and negative direction) parallel to the earth magnetic field lines. 
 * It helps a lot if you know roughly what this direction is.  
 * Info about the Earthmagnetic field https://en.wikipedia.org/wiki/Earth's_magnetic_field
 * The angle above the horizon is called the inclination angle. If you live in the northern hemisphere roughly in  
 * southern direction, and in the southern hemisphere roughly in northern direction.
 * 
 * written by Femme Verbeek 30-5-2020
 * 
 * This program uses V2 of the LSM9DS1 library 
 * Tested on an Arduino Nano 33 BLE Sense board.
 */

#include <Arduino_LSM9DS1.h> 


float EarthMagnetStrength = 49.0;  //= µT

boolean magnetOK=false;
uint8_t magnetODRindex=8;  // (0..8)->{0.625,1.25,2.5,5.0,10,20,40,80,400}Hz
uint8_t magnetFSindex=0;   // 0=±400.0; 1=±800.0; 2=±1200.0 , 3=±1600.0  (µT) 


void setup() {
  Serial.begin(115200); 
  while (!Serial);
  pinMode(LED_BUILTIN,OUTPUT); 
  delay(10);
  if (!IMU.begin()) { Serial.println(F("Failed to initialize IMU!")); while (1);  }
  IMU.setMagnetODR(magnetODRindex);
  IMU.setMagnetFS(magnetFSindex);
}

void loop() 
{ // MainMenu();
  calibrateMagnetMenu();
}

void printParam(char txt[], float param[3])
{   for (int i= 0; i<=2 ; i++) 
    {  Serial.print(txt);Serial.print("[");
       Serial.print(i);Serial.print("] = "); 
       Serial.print(param[i],6);Serial.print(";");
    }
}

void printSetParam(char txt[], float param[3])
{   Serial.print(txt);Serial.print("(");
    Serial.print(param[0],6);Serial.print(", ");
    Serial.print(param[1],6);Serial.print(", ");
    Serial.print(param[2],6);Serial.print(");");
}

//**********************************************************************************************************************************
//*********************************************              Magnetometer             **********************************************
//**********************************************************************************************************************************


void calibrateMagnetMenu()
{ char incomingByte = 0;
  byte b;
 // String ans;
    Serial.println(F("\n\n\n\n\n\n\n\n\n\n\n")); 
 while (1)   //(incomingByte!='X') 
{   Serial.println(F("Calibrate Magnetometer"));
    Serial.println(F("During measurement each of the sensor XYZ axes must be aligned in both directions with the Earth's magnetic field."));   
    Serial.println(F("This takes about 10 seconds, if you know the local direction of the magnetic field lines. If you don't, it will take"));
    Serial.println(F("several minutes, as you have to twist the board around, aiming every axis in every direction until the min and max "));
    Serial.println(F("values no longer change. Info about the Earthmagnetic field https://en.wikipedia.org/wiki/Earth's_magnetic_field "));
    Serial.println(F("E.g. in my case (Northern hemisphere)declination=0°, inclination=67°, means the aiming direction is South and a"));
    Serial.println(F("rather steep 67° above the horizon. "));
    Serial.println(F("The magnetic field measurement will be heavily disturbed by your set-up, so an \"in-situ\" calibration is advised.\n"));
    Serial.print  (F(" (F) Full Scale setting "));Serial.print(magnetFSindex);Serial.print(" = ±"); Serial.print(IMU.getMagnetFS(),0);Serial.println(F(" µT"));
    Serial.print  (F(" (R) Output Data Rate (ODR) setting "));Serial.print(magnetODRindex);Serial.print(" = ");Serial.print(IMU.getMagnetODR(),0);Serial.println(F("Hz (actual value)"));
    Serial.print  (F(" (L) Local intensity of Earth magnetic field  "));Serial.print(EarthMagnetStrength);Serial.println(F(" µT  Change into your local value."));    
    Serial.println(F(" (C) Calibrate Magnetometer, Twist board around to find min-max values or aim along earth mag field,  press enter to stop\n"));

    Serial.println(F("   // Magnetometer code"));
    Serial.print  (F("   IMU.setMagnetFS("));Serial.print(magnetFSindex);Serial.println(");");
    Serial.print  (F("   IMU.setMagnetODR("));Serial.print(magnetODRindex);Serial.println(");");
    printSetParam("   IMU.setMagnetOffset",IMU.magnetOffset);
    Serial.println();
    printSetParam("   IMU.setMagnetSlope ",IMU.magnetSlope); 
    Serial.println(F("\n\n"));    
    incomingByte= readChar();
    switch (incomingByte)
    { case 'L': { readAnswer("\n\nLocal Earth Magnetic Field intensity  " ,EarthMagnetStrength  );   break;}
      case 'C': { calibrateMagnet() ;
                  Serial.print(F("\n\n\n\n\n\n"));
                  break;}
      case 'F': { Serial.print (F("\n\nEnter new FS nr 0=±400.0; 1=±800.0; 2=±1200.0 , 3=±1600.0  (µT)> ")); 
                  b= readChar()-48; Serial.println(b);
                  if (b!=magnetFSindex && b >=0 && b<=3) magnetFSindex=b;
                  IMU.setMagnetFS(magnetFSindex); 
                  Serial.print("\n\n\n"); 
                  break;}
      case 'R': { Serial.print (F("\n\nEnter new ODR nr 6: 40Hz, 7: 80Hz, 8: 400Hz not adviced 0..5: 0.625,1.25,2.5,5.0,10,20 Hz  ")); 
                  b= readChar()-48; Serial.println(b);
                  if (b!=magnetODRindex && b>=1 && b<=8) magnetODRindex=b;
                  IMU.setMagnetODR(magnetODRindex);
                  Serial.print("\n\n\n\n\n\n\n\n\n"); 
                  break;
                }  
      default : {Serial.println(F("No menu choise\n\n"));Serial.print(incomingByte); break;}
    }   
  }
}


char readChar()
{  char ch;
   while (!Serial.available()) ;             // wait for character to be entered
   ch= toupper(Serial.read());   
   delay(10);
   while (Serial.available()){Serial.read();delay(1);} // empty readbuffer 
   return ch;
}

void readAnswer(char msg[], float& param)
{ char ch=0;
  byte count=0;
  const byte NofChars = 8;
  char ans[NofChars];
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
  if (count>1) param= atof(ans);
  while (Serial.available()){Serial.read();}
     Serial.println(F("\n\n\n\n\n\n\n")); 
}


void calibrateMagnet()  // measure Offset and Slope of XYZ
{  float x, y, z, Xmin, Xmax, Ymin, Ymax, Zmin, Zmax  ;
   unsigned long count=0;
   IMU.setMagnetODR(8);    //Fast rate 400Hz
   raw_N_Magnet(10, Xmin, Ymin, Zmin);             // find some starting values
   Xmax = Xmin; Ymax = Ymin; Zmax = Zmin;
   while (!Serial.available())                       // measure until enter key pressed
   {  raw_N_Magnet(10, x, y, z);                    //average over a number of samples to reduce the effect of outlyers
      Xmax = max (Xmax, x); Xmin = min (Xmin, x);
      Ymax = max (Ymax, y); Ymin = min (Ymin, y);
      Zmax = max (Zmax, z); Zmin = min (Zmin, z);
      count++;
      if ((count & 5)==0)                           //reduce the number of prints by a factor 
      { Serial.print(F("Xmin = "));Serial.print(Xmin); Serial.print(F("  Xmax = "));Serial.print(Xmax);
        Serial.print(F(" Ymin = "));Serial.print(Ymin); Serial.print(F("  Ymax = "));Serial.print(Ymax);
        Serial.print(F(" Zmin = "));Serial.print(Zmin); Serial.print(F("  Zmax = "));Serial.print(Zmax);
        Serial.println();
      }   
   } 
   while (Serial.available()) Serial.read();    //readStringUntil(13);        //Empty read buffer
   IMU.setMagnetOffset( (Xmax+Xmin)/2,(Ymax+Ymin)/2, (Zmax+Zmin)/2 ) ;                                      // store offset
   IMU.setMagnetSlope ( (2*EarthMagnetStrength)/(Xmax-Xmin),(2*EarthMagnetStrength)/(Ymax-Ymin),(2*EarthMagnetStrength)/(Zmax-Zmin));  // store slope  
   magnetOK=true;
   IMU.setMagnetODR(magnetODRindex);
} 


void raw_N_Magnet(unsigned int N, float& averX, float& averY, float& averZ) 
{    float x, y, z;
     averX=0; averY =0;averZ =0;
     for (int i=1;i<=N;i++)
     {  while (!IMU.magnetAvailable());
        IMU.readRawMagnet(x, y, z);
        averX += x;    averY += y;     averZ += z;
        digitalWrite(LED_BUILTIN, (millis()/125)%2);       // blink onboard led every 250ms
        if ((i%30)==0)Serial.print('.'); 
     } 
     averX /= N;    averY /= N;     averZ /= N;
     digitalWrite(LED_BUILTIN,0);                         // led off
}
