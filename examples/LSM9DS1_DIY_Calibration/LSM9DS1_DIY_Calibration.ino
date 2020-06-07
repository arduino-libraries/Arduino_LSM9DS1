/* DIY calibration program for the LSM9DS1 chip 
 *  
 * Follow the instructions on the screen how to do calibration measurements. 
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
//boolean blinkEnabled=false;
const float gyroSlopeCriterion = 10;
const float accelCriterion = 0.06;
float EarthMagnetStrength = 49.0;  //= µT
float EarthMagnetInclination = 67.0;  //= degree

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
{ Serial.println("\n\n\n\n\n\n\n\n"); 
  Serial.println("                             DIY Calibration program \n");
  Serial.println("The board with the LSM9DS1 sensor is best fitted into a non metallic rectangular box with flat sides.");
  Serial.println("In the following procedures you'll be asked to rotate the sensor around one of it's axes at the time");
  Serial.println("or simply aim the axis upward and keep still. The simplest way is to put the box with one of its flat");
  Serial.println("sides on the table ");
  Serial.println("To operate the menu in the IDE serial monitor you must press a letter key followed by Enter  \n");

  Serial.println("   Calibrate \n");
  Serial.println("(A)ccelerometer ");
  Serial.println("(G)yroscope ");
  Serial.println("(M)agnetometer ");
  Serial.println("\n\n\n\n\n\n\n\n"); 
  while (!Serial.available()) ;
  char incomingByte = toupper (Serial.read());                      // read the incoming byte
  if (byte(incomingByte)!=13) Serial.readStringUntil(13); // empty readbuffer 
  switch (incomingByte)
  { case 'A': {calibrateAccelMenu(); break;}
    case 'G': {calibrateGyroMenu(); break;}
    case 'M': {calibrateMagnetMenu(); break;}
    default : {Serial.println("I received: "+String(incomingByte)); break;}
  }
}  

float maxAX = 1, maxAY=1, maxAZ=1, minAX=-1, minAY=-1, minAZ=-1;

void calibrateAccelMenu()
{char incomingByte = 0;
 while (incomingByte!='X') 
 {  Serial.println("\n\n"); 
    Serial.println("Calibrate Accelerometer");
    Serial.println("Place the board on a horizontal surface with one of its axes vertical");
    Serial.println("Each of the axes must be measured facing up and facing down, so at least a total of 6 measurements. ");
    Serial.println("The program recognises which axis is held vertical. If the angle is to far oblique the measurement is not valid.");
    
    Serial.println("\nPress (M) to start a calibration measurement. ");
    Serial.println("\nPress (X) to exit \n");
    Serial.print ("Max values  X="+String(maxAX,4));  Serial.print ("\tY="+String(maxAX,4));Serial.println ("\tZ="+String(maxAZ,4));
    Serial.print ("Min values  X="+String(minAX,4));  Serial.print ("\tY="+String(minAX,4));Serial.println ("\tZ="+String(minAZ,4));
   
    Serial.println("\nWhen all directions are measured, the code below can be copy/pasted in a sketch\n");
    for (int i= 0; i<=2 ; i++) 
    {  Serial.print("   IMU.accelOffset["+String(i));
       Serial.print("] = "+String(IMU.accelOffset[i],6)); 
       Serial.print(";");
    } 
    Serial.println();
    for (int i= 0; i<=2 ; i++) 
    {  Serial.print("   IMU.accelSlope ["+String(i));
       Serial.print("] = "+String(IMU.accelSlope[i],6)); 
       Serial.print(";");
    }
    Serial.println("\n\n");
    while (!Serial.available()) ;               // wait for character to be entered
    incomingByte = toupper(Serial.read());   
    if (byte(incomingByte)!=13) Serial.readStringUntil(13); // empty readbuffer 
    if ((incomingByte)=='M') calibrateAccel();
  }
}


void calibrateGyroMenu()
{char incomingByte = 0;
boolean OffsetCalibrated=false;
int turnangle = 180;
  Serial.println("\n\n\n\n\n\n\n\n\n\n\n"); 
  while (incomingByte!='X') 
  { Serial.println("       Calibrate Gyroscope\n");
    if (!OffsetCalibrated) Serial.println(" Start by calibrating the Offset\n");
    else
    {   Serial.println("Each of the gyroscope's axes, XYZ, must be individually calibrated for it's sensitivity (slope) ");
        Serial.println("During a slope calibration measurement the board must be rotated about one axis over a known turn angle. ");
        Serial.println("The rotation must be pure, i.e. without much rotation about the other two axes. An easy method is to");
        Serial.println("place the board on a flat surface with the rotation axis vertical and its side pushed against a non moving");
        Serial.println("object. Rotate it over the turn angle in a few seconds, press it against the object again and hit Enter ");
        Serial.println("to stop measuring. Each of the axes X,Y and Z must be measured. The program automatically detects which. \n");
        Serial.print  (" (A)ngle "+String(turnangle));Serial.println("°, change angle to rotate the sensor over ");
        Serial.println(" (M)easure Slope, turn the board and press enter when finished ");
    } 
    Serial.println(" (C)alibrate Offset, just keep the sensor still during 5 sec measurement\n");
    Serial.println(" (X) eXit to the main menu");
    if (OffsetCalibrated)
    {   Serial.println("\n The code below can be copy/pasted in a sketch\n");
        for (int i= 0; i<=2 ; i++) 
        {  Serial.print("   IMU.gyroOffset["+String(i));
           Serial.print("] = "+String(IMU.gyroOffset[i],6)); 
           Serial.print(";");
        } 
        Serial.println();
        for (int i= 0; i<=2 ; i++) 
        {  Serial.print("   IMU.gyroSlope ["+String(i));
           Serial.print("] = "+String(IMU.gyroSlope[i],6)); 
           Serial.print(";");
        }
        Serial.println();
    }
    else Serial.println("\n\n\n\n\n\n\n\n"); 
    while (!Serial.available());                // wait for anything to be entered
    incomingByte = toupper(Serial.read());      // read char and convert to upcase
    if (byte(incomingByte)!=13) Serial.readStringUntil(13); // empty readbuffer 
    switch (incomingByte)
    { case 'C': {Serial.println("\n\nMeasuring. Just a moment."); 
                  calibrateGyroOffset(); 
                  OffsetCalibrated=true; 
                  break;}
      case 'A': { turnangle *=2; 
                  if (turnangle>360) turnangle=90;  
                  break;}
      case 'M': { Serial.print("\n\n\n\n\n\nMeasuring. Turn the sensor over "+String(turnangle));Serial.println(" degrees\n");
                  Serial.println("Press Enter when finished.");
                  calibrateGyroslope(turnangle);
                  break;}
      default : {Serial.println("I received: "+String(incomingByte)); break;}
    }    
   Serial.println(""); 
  }
}
void calibrateMagnetMenu()
{char incomingByte = 0;
String ans;
 Serial.println("\n\n\n\n\n\n\n\n\n\n\n"); 
 while (incomingByte!='X') 
 {  Serial.println("Calibrate Magnetometer");
    Serial.println("The magnetic field measurement will be heavily disturbed by your set-up, so an \"in-situ\" calibration is advised.");
    Serial.println("The demonstrated min-max calibration was slightly improved to diminish outlyers but is still only very simple.");
    Serial.println("While measuring the min and max magnetic values must be found for all axes by twisting the board around. The ");
    Serial.println("min-max values are constantly printed. If one of them changes you found a higher or lower value. It helps if you ");
    Serial.println("know the direction of the Earth magnetic fieldlines. All you have to do is align the sensor axes with it.  ");
    Serial.println("More info about the Earth magnetic field https://en.wikipedia.org/wiki/Earth%27s_magnetic_field   ");
    Serial.println("  ");
    Serial.print  (" (S)trength of Local Earth magnetic field   "+String(EarthMagnetStrength));Serial.println(" µT  Needed for correct Slope setting");    
    Serial.println(" (M)in-Max Calibration, twist board around,  press enter to stop measuring");
    Serial.println(" (X) eXit to the main menu");
    Serial.println("  ");
    Serial.println("\n The code below can be copy/pasted in a sketch\n");
        for (int i= 0; i<=2 ; i++) 
        {  Serial.print("   IMU.magnetOffset["+String(i));
           Serial.print("] = "+String(IMU.magnetOffset[i],6)); 
           Serial.print(";");
        } 
        Serial.println();
        for (int i= 0; i<=2 ; i++) 
        {  Serial.print("   IMU.magnetSlope ["+String(i));
           Serial.print("] = "+String(IMU.magnetSlope[i],6)); 
           Serial.print(";");
        }
        Serial.println("\n\n");    
    while (!Serial.available()) ;             // wait for character to be entered
    incomingByte = toupper(Serial.read());   
    if (byte(incomingByte)!=13) Serial.readStringUntil(13); // empty readbuffer 
    switch (incomingByte)
    { case 'S': { readAnswer("\n\nEnter local Field intensity " ,EarthMagnetStrength  );   break;}
      case 'M': { calibrateMagnet() ;
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
   IMU.setMagnetOffset(0,0,0);                      // Offsets must be 0 when calibrating
   IMU.setMagnetSlope(1,1,1);                       // slopes must be 1 when calibrating
   IMU.setMagnetODR(8);    //Fast rate 400Hz
   read_N_Magnet(10, Xmin, Ymin, Zmin);             // starting values
   Xmax = Xmin; Ymax = Ymin; Zmax = Zmin;
   while (!Serial.available())                       // measure until enter key pressed
   {  read_N_Magnet(10, x, y, z);                    //average over a number of samples to reduce the effect of outlyers
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
   Serial.readStringUntil(13);        //Empty read buffer
   IMU.setMagnetOffset( (Xmax+Xmin)/2,(Ymax+Ymin)/2, (Zmax+Zmin)/2 ) ;                                      // store offset
   IMU.setMagnetSlope ( (2*EarthMagnetStrength)/(Xmax-Xmin),(2*EarthMagnetStrength)/(Ymax-Ymin),(2*EarthMagnetStrength)/(Zmax-Zmin));  // store slope  

}   



void calibrateGyroslope(unsigned int turnangle)  // rotate board over known angle
{  float tempSlope[3];
   boolean validMmt=false;
   float dirX=0, dirY=0, dirZ=0,sigmaX2=0,sigmaY2=0,sigmaZ2=0;
   float x, y, z;
   unsigned int count=0;
   for (int i =0;i<=2;i++) tempSlope[i]= IMU.gyroSlope[i];
   IMU.setGyroSlope(1,1,1);
   while (!Serial.available())         // measure until enter pressed
   {  while (!IMU.gyroAvailable());
       IMU.readGyro(x, y, z);
       dirX += x/IMU.getGyroODR(); dirY += y/IMU.getGyroODR(); dirZ += z/IMU.getGyroODR();
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
          tempSlope[0]=dirX/turnangle;
          Serial.print ("Valid measurement, X detected, slope ");  Serial.println (tempSlope[0]);
       }
   }
   if (dirY>max(dirX,dirZ))
   {   if (sigmaX2<gyroSlopeCriterion && sigmaZ2<gyroSlopeCriterion )
       {  validMmt= true;
          tempSlope[1]=dirY/turnangle;
           Serial.print ("Valid measurement, Y detected, slope ");Serial.println (tempSlope[1]);
       }
   }
   if (dirZ>max(dirY,dirX))
   {   if (sigmaY2<gyroSlopeCriterion && sigmaX2<gyroSlopeCriterion )
       {  validMmt= true;
          tempSlope[2]=dirZ/turnangle;
          Serial.print ("Valid measurement, Z detected, slope ");Serial.println (tempSlope[2]);
      }
   }
   for (int i =0;i<=2;i++) IMU.gyroSlope[i] = tempSlope[i];
   if ( !validMmt ) 
    { Serial.print("\n\nNot a valid measurement!  "); 
      delay(500);}
}

void calibrateGyroOffset(){  // don't move the board during calibration
float x, y, z;// , addX=0, addY=0, addZ=0  ;
       IMU.setGyroOffset(0,0,0);  // Offsets must be zero when calibrating
       IMU.setGyroODR(5);  //476 Hz
       read_N_Gyro(2500,x,y,z);
       IMU.setGyroOffset(x, y,z); // Store the average measurements as offset
       digitalWrite(LED_BUILTIN, 0);       // onboard led off
       Serial.print("\n\n\n\n\n\n"); 
}   

void calibrateAccel()
{  boolean validMmt=false;
   float x,y,z;
   Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"); 
   Serial.println("measuring \n"); 
   IMU.setAccelOffset(0,0,0);
   IMU.setAccelSlope(1,1,1);
   IMU.setAccelODR(5);  //476 Hz
   read_N_Accel(2500,x,y,z);
   if (abs(x)>max(abs(y),abs(z)))
   {    Serial.println("X detected");  
       if (sqrt(y*y+z*z)/x<accelCriterion)
       {  validMmt= true;
          if (x>0) maxAX=x; else minAX=x;
       }
   }
   if (abs(y)>max(abs(x),abs(z)))
   {  Serial.println("Y detected");  
      if (sqrt(x*x+z*z)/y<accelCriterion)
       {  validMmt= true;
          if (y>0) maxAY=y; else minAY=y;
       }  
   }
   if (abs(z)>max(abs(x),abs(y)))
   {  Serial.println("Z detected");  
      if ( sqrt(x*x+y*y)/z<accelCriterion)
       {  validMmt= true;
          if (z>0) maxAZ=z; else minAZ=z;
       }  
   }
   IMU.setAccelOffset((maxAX+minAX)/2,(maxAY+minAY)/2,(maxAZ+minAZ)/2);
   IMU.setAccelSlope((maxAX-minAX)/2,(maxAY-minAY)/2,(maxAZ-minAZ)/2);
   if ( !validMmt ) 
    { Serial.print("\n\n\nNot a valid measurement!  ");
      Serial.print("x="+String(x));Serial.print("  y="+String(y));Serial.println("  z="+String(z));
    }
}

void read_N_Accel(unsigned int N, float& averX, float& averY, float& averZ) 
{    float x, y, z;
     averX=0; averY =0;averZ =0;
     for (int i=1;i<=N;i++)
     {  while (!IMU.accelAvailable());
        IMU.readAccel(x, y, z);
        averX += x;    averY += y;     averZ += z;
        digitalWrite(LED_BUILTIN, (millis()/125)%2);       // blink onboard led every 250ms
        if ((i%30)==0)Serial.print('.'); 
     } 
     averX /= N;    averY /= N;     averZ /= N;
     digitalWrite(LED_BUILTIN,0);                          // led off
}

void read_N_Gyro(unsigned int N, float& averX, float& averY, float& averZ) 
{    float x, y, z;
     averX=0; averY =0;averZ =0;
     for (int i=1;i<=N;i++)
     {  while (!IMU.gyroAvailable());
        IMU.readGyro(x, y, z);
        averX += x;    averY += y;     averZ += z;
        digitalWrite(LED_BUILTIN, (millis()/125)%2);       // blink onboard led every 250ms
        if ((i%30)==0)Serial.print('.'); 
     } 
     averX /= N;    averY /= N;     averZ /= N;
     digitalWrite(LED_BUILTIN,0);                         // led off
}

void read_N_Magnet(unsigned int N, float& averX, float& averY, float& averZ) 
{    float x, y, z;
     averX=0; averY =0;averZ =0;
     for (int i=1;i<=N;i++)
     {  while (!IMU.magnetAvailable());
        IMU.readMagnet(x, y, z);
        averX += x;    averY += y;     averZ += z;
        digitalWrite(LED_BUILTIN, (millis()/125)%2);       // blink onboard led every 250ms
        if ((i%30)==0)Serial.print('.'); 
     } 
     averX /= N;    averY /= N;     averZ /= N;
     digitalWrite(LED_BUILTIN,0);                         // led off
}
