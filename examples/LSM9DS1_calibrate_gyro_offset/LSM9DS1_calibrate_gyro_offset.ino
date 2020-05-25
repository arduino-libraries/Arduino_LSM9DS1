/*Example program for Arduino__LSM9DS1 Library version 2.0 
 * Tested on Arduino Nano 33 BLE SENSE 
 * Written by Femme Verbeek 15 may 2020.
 * 
 * Measures the gyroscope offset and sets the IMU.gyroOffset parameters for X,Y,Z
 * To start the calibration measurement, open the serial monitor.
 * For a good result the board must be held still during the calibration measurements
 * During the calibration the onboard LED's are blinking
 * When finished the program prints the calibration result 
 * The program pauses till you press enter
 * Next the program will continue measuring and print the now calibrated results. 
 * Best close the serial monitor first, open the serial monitor and send the character from there.  
 * 
 * To see a graphic result of the measurements, close the serial monitor when the program pauses,
 * open the serial plotter, type any character and press "send".
 */
const int averageNSamples=10;  //average output over N measurements

#include <Arduino_LSM9DS1.h>
//unsigned long timer;
void setup() 
{  Serial.begin(115200);
   while(!Serial);
   delay(1);
   if (!IMU.begin()) {
      Serial.println("Failed to initialize IMU!");
      while (1); }
   IMU.setGyroODR(4); //Sampling Rate 0:off, 1:10Hz, 2:50Hz, 3:119Hz, 4:238Hz, 5:476Hz, 6:952Hz, 7:NA
   IMU.setGyroBW(0); // bandwidth see table 47 datasheet
   Serial.print("Gyro ODR "+String( IMU.getGyroODR()  ) );
   Serial.println(" BW setting "+String( IMU.getGyroBW()  ) );
   Serial.println("Calibrating. Just a moment. ");
   Serial.println("Keep the sensor still as long as the LED's are flashing");

   calibrateGyroOffset(12000); // do calibration measurements during 12000 ms. 
 
   Serial.println("calibration results"); 
   Serial.print("Content of IMU.gyroOffset : "); 
   for (int i= 0; i<=2 ; i++) {Serial.print(IMU.gyroOffset[i],6);Serial.print("  ");}
   Serial.println();
   Serial.println("Press enter to start mesuring");
   Serial.println("To see a graph of the measurements, close this serial monitor, open the serial plotter type any character and click the send button");
   while (!Serial.available());    // pause the program, enter continues
   while (Serial.available()) Serial.read();   // clear the read buffer
   Serial.println(" X \t Y \t Z ");
}

// continue doing calibrated measurements, this is best viewed in the serial plotter
void loop()       
{  float x, y, z, averX=0, averY=0, averZ=0;
   for (int i=1;i<=averageNSamples;i++)
   {  while (!IMU.gyroAvailable());
       IMU.readGyro(x, y, z);
       averX += x; averY += y; averZ += z;
   }
   Serial.print(String(averX/averageNSamples,6)+'\t');
   Serial.print(String(averY/averageNSamples,6)+'\t');
   Serial.println(averZ/averageNSamples,6);          
}
    
void calibrateGyroOffset(unsigned int duration){  // don't move the board during calibration
unsigned long count = 0, start = millis();
float x, y, z, addX=0, addY=0, addZ=0  ;
       IMU.setGyroOffset(0,0,0);  // Offsets must be zero when calibrating
       while ((millis()-start) < duration)
       { if (IMU.gyroscopeAvailable())
         {  IMU.readGyroscope(x, y, z);  
            count++;
            addX += x; addY += y; addZ += z;
            digitalWrite(LED_BUILTIN, (millis()/125)%2);       // blink onboard led every 250ms
         }
       } 
       IMU.setGyroOffset(addX/count, addY/count, addZ/count); // Store the average measurements as offset
       digitalWrite(LED_BUILTIN, 0);       // onboard led off
       Serial.println("nr of samples "+String(count));
}   
