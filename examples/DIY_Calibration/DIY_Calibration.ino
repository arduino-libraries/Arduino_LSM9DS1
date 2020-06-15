/* DIY calibration program for the LSM9DS1 chip 
 *  
 * Follow the instructions on the screen how to do calibration measurements. 
 * See instruction video https://youtu.be/BLvYFXoP33o
 * No special tools or setups are needed, however it is handy if the board with the LSM9DS1 chip is fitted inside 
 * a non-metalic rectangular box.
 * The offset and slope factors are displayed on screen as code that can be copy/pasted directly into a sketch.  
 * Each new instance of the chip will require it's own unique set of calibration factors. 
 * 
 * written by Femme Verbeek 30-5-2020
 * 
 * This program uses V2 of the LSM9DS1 library 
 * Tested on an Arduino Nano 33 BLE Sense board.
 */

#include <Arduino_LSM9DS1.h> 

// not really my habit to use so many global variabeles but WTH this is still a small program
const float gyroSlopeCriterion = 50;
const float accelCriterion = 0.1;
float EarthMagnetStrength = 49.0;  //= µT
float EarthMagnetInclination = 67.0;  //= degree
char xyz[3]= {'X','Y','Z'};
float maxAX = 1, maxAY=1, maxAZ=1, minAX=-1, minAY=-1, minAZ=-1; // Accel Slope
float zeroAX1 =0,zeroAX2 =0,zeroAY1 =0,zeroAY2 =0,zeroAZ1 =0,zeroAZ2 =0;  //Accel Offset
//boolean accelOffsetOK[4]={false,false,false,false};
boolean accelOK=false;
uint8_t acceMMlOK=0; // bit 0..2 maxXYZ bit 3..5 minXYZ
boolean gyroOK=false;
boolean gyroOffsetOK=false;
boolean gyroSlopeOK[3]={false,false,false};
boolean magnetOK=false;

void setup() {
  Serial.begin(115200); 
  while (!Serial);
  pinMode(LED_BUILTIN,OUTPUT); 
  delay(10);
  if (!IMU.begin()) { Serial.println("Failed to initialize IMU!");while (1); }
  IMU.setGyroODR(4);   //238Hz   
  IMU.setMagnetODR(7); //80Hz
}

void loop() 
{  MainMenu();
}


void MainMenu()
{ Serial.println("\n\n\n\n\n"); 
  Serial.println("                             DIY Calibration program ");
  Serial.println("The board with the LSM9DS1 sensor is best fitted into a non metallic rectangular box with flat sides");
  Serial.println("or find another way to keep the board horizontal or vertical on each of it's sides and rotate it .");
  Serial.println("In the following procedures you'll be asked to rotate the sensor around one of it's axes at the time");
  Serial.println("or simply aim the axis upward and keep still. The simplest way is to put the box with one of its flat");
  Serial.println("sides on the table and turn it by hand. See instruction video https://youtu.be/BLvYFXoP33o");
  Serial.println("To operate the menu in the IDE serial monitor you must press a letter key followed by Enter  \n");

  Serial.print("   Calibrate ");
  Serial.print  ("\n  (A) Accelerometer ");
           if (accelOK)   Serial.print("-OK-");
 // Serial.print  ("\n  (O) Gyroscope Offset ");
 //          if (gyroOffsetOK) Serial.print("-OK-");
  Serial.print  ("\n  (G) Gyroscope     ");
    if (gyroOK )          Serial.print("-OK-");
  Serial.print  ("\n  (M) Magnetometer  ");
            if (magnetOK) Serial.print("-OK-");
  Serial.println("\n\n The code below can be copy/pasted in a sketch");

  if (accelOK)   printParam ("   IMU.accelOffset ",IMU.accelOffset);
  Serial.println();
  if (accelOK)   printParam ("   IMU.accelSlope  ", IMU.accelSlope);
  Serial.println();

  if (gyroOffsetOK) printParam ("   IMU.gyroOffset  ", IMU.gyroOffset);
  Serial.println();
  if (gyroSlopeOK[0] &&gyroSlopeOK[1] &&gyroSlopeOK[2] ) printParam ("   IMU.gyroSlope   ", IMU.gyroSlope);
  Serial.println();
  if (magnetOK)
  {  printParam ("   IMU.magnetOffset",IMU.magnetOffset);  
     Serial.println();
     printParam ("   IMU.magnetSlope ",IMU.magnetSlope);   
  }
  Serial.println("");  
  while (!Serial.available()) ;
  char incomingByte = toupper (Serial.read());                      // read the incoming byte
  if (byte(incomingByte)!=13) Serial.readStringUntil(13); // empty readbuffer 
  switch (incomingByte)
  { case 'A': {calibrateAccelMenu(); break;}
//    case 'G': {calibrateGyroOffset(); break;}
    case 'G': {calibrateGyroMenu(); break;}
    case 'M': {calibrateMagnetMenu(); break;}
    default : {Serial.println("I received: "+String(incomingByte)); break;}
  }
}  

void printParam(String txt, float param[3])
{   for (int i= 0; i<=2 ; i++) 
    {  Serial.print(txt+"[");
       Serial.print(String(i)+"] = "); 
       Serial.print(String(param[i],6)+";");
    }
}

//**********************************************************************************************************************************
//*********************************************              Accelerometer            **********************************************
//**********************************************************************************************************************************

void calibrateAccelMenu()
{char incomingByte = 0;
 while (incomingByte!='X') 
 {  Serial.println("\n\n"); 
    Serial.println("Calibrate Accelerometer Offset and Slope");
    Serial.println("Place the board on a horizontal surface with one of its axes vertical. Each of the axes must be measured ");
    Serial.println("pointing up and pointing down, so a total of 6 measurements. ");
    Serial.println("The program recognises which axis is vertical. If the angle is to far oblique the measurement is not valid.\n");

    Serial.println("Press (C) to start a calibration measurement. ");
    Serial.println("Press (X) to exit \n");

    Serial.println("Measured status");
    for (int i=0;i<=2;i++){  Serial.print(xyz[i]); if (bitRead(acceMMlOK,i)==1)Serial.print("+ = ( -OK- ) "); else Serial.print("+ = not done "); }
    Serial.println();
    for (int i=0;i<=2;i++){  Serial.print(xyz[i]); if (bitRead(acceMMlOK,i+3)==1)Serial.print("- = ( -OK- ) "); else Serial.print("- = not done "); }
//    Serial.print ("\nMax values  X="+String(maxAX,4));  Serial.print ("\tY="+String(maxAY,4));Serial.println ("\tZ="+String(maxAZ,4));
//    Serial.print ("Min values  X="+String(minAX,4));  Serial.print ("\tY="+String(minAY,4));Serial.println ("\tZ="+String(minAZ,4));
   
    Serial.println("\n\nCurrent  accelerometer calibration values (copy/paste-able)\n");
    printParam("   IMU.accelOffset",IMU.accelOffset);
    Serial.println();
    printParam ("   IMU.accelSlope ",IMU.accelSlope); 
    Serial.println("\n\n\n\n");
    while (!Serial.available()) ;               // wait for character to be entered
    incomingByte = toupper(Serial.read());   
    if (byte(incomingByte)!=13) Serial.readStringUntil(13); // empty readbuffer 
    if ((incomingByte)=='C') calibrateAccelSlope();
  }
}

void calibrateAccelSlope()
{  boolean validMmt=false;
   float x,y,z;
   Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"); 
   Serial.println("measuring \n"); 
   IMU.setAccelSlope(1,1,1);
   IMU.setAccelODR(5);  //476 Hz
   raw_N_Accel(2500,x,y,z);
   if (abs(x)>max(abs(y),abs(z)))
   {    Serial.println("X detected");  
       if (sqrt(y*y+z*z)/x<accelCriterion)
       {  validMmt= true;
          if (x>0) {maxAX=x; 
                    acceMMlOK=acceMMlOK | 0b00000001 ;}
          else     {minAX=x; 
                    acceMMlOK=acceMMlOK | 0b00001000 ; }
       }
   }
   if (abs(y)>max(abs(x),abs(z)))
   {  Serial.println("Y detected");  
      if (sqrt(x*x+z*z)/y<accelCriterion)
       {  validMmt= true;
          if (y>0) {maxAY=y; 
                    acceMMlOK=acceMMlOK | 0b00000010 ; }
          else     {minAY=y; 
                    acceMMlOK=acceMMlOK | 0b00010000 ; }
       }  
   }
   if (abs(z)>max(abs(x),abs(y)))
   {  Serial.println("Z detected");  
      if ( sqrt(x*x+y*y)/z<accelCriterion)
       {  validMmt= true;
          if (z>0) {maxAZ=z; 
                    acceMMlOK=acceMMlOK | 0b00000100 ; }
          else     {minAZ=z;
                    acceMMlOK=acceMMlOK | 0b00100000 ; }
       }  
   }
   IMU.setAccelOffset((maxAX+minAX)/2,(maxAY+minAY)/2,(maxAZ+minAZ)/2);
   IMU.setAccelSlope ((maxAX-minAX)/2,(maxAY-minAY)/2,(maxAZ-minAZ)/2);
   if (acceMMlOK==0b00111111) accelOK = true;
   
   if ( !validMmt ) 
    { Serial.print("\n\n\nNot a valid measurement!  ");
      Serial.println(" x="+String(x));Serial.print("  y="+String(y));Serial.println("  z="+String(z));
    }
}


//**********************************************************************************************************************************
//*********************************************              Gyroscope                **********************************************
//**********************************************************************************************************************************


void calibrateGyroMenu()
{char incomingByte = 0;
//boolean OffsetCalibrated=false;
float turnangle = 360;
  Serial.println("\n\n\n\n\n\n\n\n\n\n\n"); 
  if (!gyroOffsetOK) 
  {   Serial.println(" Start by calibrating the Offset\n");  // Slope can't be calibrated with an uncalibrated offset 
      calibrateGyroOffset(); 
  }    
  while (incomingByte!='X') 
  { Serial.println("       Calibrate Gyroscope\n");
    Serial.println("The gyroscope sensor must be calibrated for it's sensitivity (slope). During a slope calibration the ");
    Serial.println("board must be rotated calmly about one axis over a known angle. Change the angle to your convenience. ");
    Serial.println("A larger angle is more accurate, but more difficult to do. The rotation must be pure, without much"); 
    Serial.println("rotation about the other two axes. It can be done by hand. Keeping the board on a flat surface with");
    Serial.println("the rotation axis vertical while turning is good enough. For an accurate result you can start and end ");
    Serial.println("with its side pushed against a non moving object. Press Enter when finished.");
    Serial.println("Each of the axes X,Y and Z must be measured. The program automatically detects which. \n");
    Serial.println(" (A) Change the measuring angle to turn the board");
    Serial.print  (" (C) Calibrate Slope, turn the board over "+String(turnangle,0));Serial.println("° and press enter when finished ");
    Serial.println(" (O) Calibrate Offset again");
    Serial.println(" (X) eXit to the main menu\n");

    Serial.println("Offset calibration ( -OK- )");
    Serial.print  ("Slope calibration  " );
    for (int i= 0; i<=2 ; i++) 
    {   Serial.print(xyz[i]);
        if (gyroSlopeOK[i]) Serial.print("= ( -OK- ) "); else Serial.print("= not done "); 
    }
    Serial.println();
    Serial.println("\n Gyroscope calibration parameters");
    printParam("   IMU.gyroOffset ",IMU.gyroOffset); 
    Serial.println();
    printParam("   IMU.gyroSlope  ",IMU.gyroSlope); 
    Serial.println();
    while (!Serial.available());                // wait for anything to be entered
    incomingByte = toupper(Serial.read());      // read char and convert to upcase
    if (byte(incomingByte)!=13) Serial.readStringUntil(13); // empty readbuffer 
    switch (incomingByte)
    { case 'A': { readAnswer("\n\nHow many degrees turn for the measurement? ", turnangle);
                  break;}
      case 'C': { Serial.print("\n\n\n\n\n\nMeasuring. Turn the sensor over "+String(turnangle));Serial.println(" degrees\n");
                  Serial.println("Press Enter when finished.");
                  calibrateGyroslope(turnangle);
                  break;}
      case 'O': { calibrateGyroOffset();     }
      default : {Serial.println("I received: "+String(incomingByte)); break;}
    }    
   Serial.println(""); 
  }
 Serial.println("\n\n\n\n\n\n\n"); 
}

void calibrateGyroOffset()  // don't move the board during calibration
{   float x, y, z;// , addX=0, addY=0, addZ=0  ;
       Serial.println("\n\n\n\nMeasuring offset. Just a moment."); 
       Serial.println("\n\n\n\nKeep the board still during measurement"); 
       IMU.setGyroODR(5);  //476 Hz
       raw_N_Gyro(2500,x,y,z);
       IMU.setGyroOffset(x, y,z); // Store the average measurements as offset
       digitalWrite(LED_BUILTIN, 0);       // onboard led off
       Serial.print("\n\n\n\n\n\n"); 
       gyroOffsetOK=true;
}   
 
void calibrateGyroslope(unsigned int turnangle)  // rotate board over known angle
{ // float tempSlope[3];
   boolean validMmt=false;
   float dirX=0, dirY=0, dirZ=0,sigmaX2=0,sigmaY2=0,sigmaZ2=0;
   float x, y, z;
   unsigned int count=0;
//  for (int i =0;i<=2;i++) tempSlope[i]= IMU.gyroSlope[i];
//   IMU.setGyroSlope(1,1,1);
   while (!Serial.available())         // measure until enter key pressed
   {  while (!IMU.gyroAvailable());
       IMU.readRawGyro(x, y, z);
       dirX += (x - IMU.gyroOffset[0])/IMU.getGyroODR();    // slope is still raw but offset must already be calibrated 
       dirY += (y - IMU.gyroOffset[1])/IMU.getGyroODR(); 
       dirZ += (z - IMU.gyroOffset[2])/IMU.getGyroODR();
       sigmaX2 +=x*x;  sigmaY2+=y*y;  sigmaZ2+=z*z;
       count++;
       if ((count%30)==0)Serial.print('.'); 
       digitalWrite(LED_BUILTIN, (millis()/125)%2);       // blink onboard led every 250ms
   }
   digitalWrite(LED_BUILTIN,0);       // led off
   Serial.readStringUntil(13);        //Empty read buffer
   Serial.print("\n\n\nMeasured direction change X "); 
   Serial.print(String(dirX,6)+"°\tY "); Serial.print(String(dirY,6)+"°\t Z "); Serial.println(String(dirZ,6)+"°");          
   sigmaX2 /= count;    sigmaY2 /= count;    sigmaZ2 /= count;
   Serial.print("Std.dev. ");  
   Serial.print(String(sigmaX2,6)+'\t'); Serial.print(String(sigmaY2,6)+'\t'); Serial.println(sigmaZ2,6);          
   dirX=abs(dirX); dirY=abs(dirY); dirZ=abs(dirZ);
   if (dirX>max(dirY,dirZ))
   {  
       if (sigmaY2<gyroSlopeCriterion && sigmaZ2<gyroSlopeCriterion )
       {  validMmt= true;
          IMU.gyroSlope[0]=turnangle/dirX;
          gyroSlopeOK[0]=true;
          Serial.print ("Valid measurement, X detected, slope ");  Serial.println (IMU.gyroSlope[0]);
       }
   }
   if (dirY>max(dirX,dirZ))
   {   if (sigmaX2<gyroSlopeCriterion && sigmaZ2<gyroSlopeCriterion )
       {  validMmt= true;
          IMU.gyroSlope[1]=turnangle/dirY;
          gyroSlopeOK[1]=true;
          Serial.print ("Valid measurement, Y detected, slope ");Serial.println (IMU.gyroSlope[1]);
       }
   }
   if (dirZ>max(dirY,dirX))
   {   if (sigmaY2<gyroSlopeCriterion && sigmaX2<gyroSlopeCriterion )
       {  validMmt= true;
          IMU.gyroSlope[2]=turnangle/dirZ;
          gyroSlopeOK[2]=true;
          Serial.print ("Valid measurement, Z detected, slope ");Serial.println (IMU.gyroSlope[2]);
      }
   }
   if (gyroSlopeOK[0] && gyroSlopeOK[1] && gyroSlopeOK[2]) gyroOK=true;
   if ( !validMmt ) 
   { Serial.println("\n\nNot a valid measurement!  "); 
     delay(1000);
   }
   Serial.println("\n\n");
}


//**********************************************************************************************************************************
//*********************************************              Magnetometer             **********************************************
//**********************************************************************************************************************************


void calibrateMagnetMenu()
{ char incomingByte = 0;
  String ans;
    Serial.println("\n\n\n\n\n\n\n\n\n\n\n"); 
 while (incomingByte!='X') 
{   Serial.println("Calibrate Magnetometer");
    Serial.println("During measurement each of the sensor XYZ axes must be aligned in both directions with the Earth's magnetic field.");   
    Serial.println("This takes about 10 seconds, if you know the local direction of the magnetic field lines. If you don't, it will take");
    Serial.println("several minutes, as you have to twist the board around, aiming every axis in every direction until the min and max ");
    Serial.println("values no longer change. Info about the Earthmagnetic field https://en.wikipedia.org/wiki/Earth's_magnetic_field ");
    Serial.println("E.g. in my case (Northern hemisphere)declination=0°, inclination=67°, means the aiming direction is South and a");
    Serial.println("rather steep 67° above the horizon. ");
    Serial.println("The magnetic field measurement will be heavily disturbed by your set-up, so an \"in-situ\" calibration is advised.");

    Serial.println("  ");
    Serial.print  (" (L) Local intensity of Earth magnetic field  "+String(EarthMagnetStrength));Serial.println(" µT  Change into your local value.");    
    Serial.println(" (C) Calibrate Magnetometer, Twist board around to find min-max values or aim along earth mag field,  press enter to stop");
    Serial.println(" (X) eXit to the main menu");
    Serial.println("  ");
    Serial.println("\n The code below can be copy/pasted in a sketch\n");
    if (magnetOK) printParam("   IMU.magnetOffset",IMU.magnetOffset);
    Serial.println();
    if (magnetOK) printParam("   IMU.magnetSlope ",IMU.magnetSlope); 
    Serial.println("\n\n");    
    while (!Serial.available()) ;             // wait for character to be entered
    incomingByte = toupper(Serial.read());   
    if (byte(incomingByte)!=13) Serial.readStringUntil(13); // empty readbuffer 
    switch (incomingByte)
    { case 'L': { readAnswer("\n\nEnter local Field intensity " ,EarthMagnetStrength  );   break;}
      case 'C': { calibrateMagnet() ;
                  Serial.print("\n\n\n\n\n\n");
                  break;}
      default : {Serial.println("I received: "+String(incomingByte)); break;}
    }   

    
  }
}

void readAnswer(String msg, float& param)
{String ans;
     Serial.print(msg); 
     while (!Serial.available());
     ans = Serial.readStringUntil(13);
     Serial.println(ans);
     if (ans!="") param= ans.toFloat();
     Serial.println("\n\n\n\n\n\n\n"); 
}

void calibrateMagnet()  // measure Offset and Slope of XYZ
{  float x, y, z, Xmin, Xmax, Ymin, Ymax, Zmin, Zmax  ;
   unsigned long count=0;
//   IMU.setMagnetOffset(0,0,0);                      // Offsets must be 0 when calibrating
//   IMU.setMagnetSlope(1,1,1);                       // slopes must be 1 when calibrating
   IMU.setMagnetODR(8);    //Fast rate 400Hz
   raw_N_Magnet(10, Xmin, Ymin, Zmin);             // starting values
   Xmax = Xmin; Ymax = Ymin; Zmax = Zmin;
   while (!Serial.available())                       // measure until enter key pressed
   {  raw_N_Magnet(10, x, y, z);                    //average over a number of samples to reduce the effect of outlyers
      Xmax = max (Xmax, x); Xmin = min (Xmin, x);
      Ymax = max (Ymax, y); Ymin = min (Ymin, y);
      Zmax = max (Zmax, z); Zmin = min (Zmin, z);
      count++;
      if ((count & 5)==0)                           //reduce the number of prints by a factor 
      { Serial.print("Xmin = "+String(Xmin)); Serial.print("  Xmax = "+String(Xmax));
        Serial.print(" Ymin = "+String(Ymin)); Serial.print("  Ymax = "+String(Ymax));
        Serial.print(" Zmin = "+String(Zmin)); Serial.print("  Zmax = "+String(Zmax));
        Serial.println();
      }   
   } 
   while (Serial.available()) Serial.read();    //readStringUntil(13);        //Empty read buffer
   IMU.setMagnetOffset( (Xmax+Xmin)/2,(Ymax+Ymin)/2, (Zmax+Zmin)/2 ) ;                                      // store offset
   IMU.setMagnetSlope ( (2*EarthMagnetStrength)/(Xmax-Xmin),(2*EarthMagnetStrength)/(Ymax-Ymin),(2*EarthMagnetStrength)/(Zmax-Zmin));  // store slope  
   magnetOK=true;
} 




void raw_N_Accel(unsigned int N, float& averX, float& averY, float& averZ) 
{    float x, y, z;
     averX=0; averY =0;averZ =0;
     for (int i=1;i<=N;i++)
     {  while (!IMU.accelAvailable());
        IMU.readRawAccel(x, y, z);
        averX += x;    averY += y;     averZ += z;
        digitalWrite(LED_BUILTIN, (millis()/125)%2);       // blink onboard led every 250ms
        if ((i%30)==0)Serial.print('.'); 
     } 
     averX /= N;    averY /= N;     averZ /= N;
     digitalWrite(LED_BUILTIN,0);                          // led off
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
