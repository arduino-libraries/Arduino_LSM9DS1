/* Calibrated compass example for the Nano 33 BLE Sense
 * You need version 2.0 or higher of the LMS9DS1 library to run this example 
 * 
 * The compass must be calibrated for the magnetic disturbance of the environment.
 * Calibration starts automaticlly when the serial monitor is opened and stops after a given time. 
 * Calibration in progress is indicated by the flashing onboard LED.
 * 
 * During calibration the board must be turned slowly over a full 360 deg. 
 * When finished calibrating the program stores and prints the offset and slope factors. 
 * 
 * After pressing "enter" the program continues working as a compass. 
 * 
 * Written by Femme Verbeek 2020  
 * Released to the public domain
*/

#include <Arduino_LSM9DS1.h>

float EarthMagField = 49000;  //= nT    For local value see https://en.wikipedia.org/wiki/Earth%27s_magnetic_field
int averageCompassByN = 10;       //number of samples per compass reading. Higher number = more accurate but slower
int averageCalibrationByN = 10;   //goldielocs number too high = fewer measurements in 360deg turn, to low = more noise
int calibrationTime = 20; //s
void setup() {
     pinMode(LED_BUILTIN,OUTPUT);
     Serial.begin(115200);
     while(!Serial);                     // wait till the serial monitor connects
     delay(1);
     if (!IMU.begin()) {  
        Serial.println("Failed to initialize IMU!");  
        while (1); }
     IMU.setMagnetODR(7);         // Sample Rate (0..7) corresponds to {0.625,1.25,2.5,5.0,10.0,20.0,40.0,80.0}Hz
     IMU.magnetUnit =  NANOTESLA;     // calibrate in nanotesla as this corresponds to the local field strenth number we found.
     Serial.println("Calibrating. Just a moment. ");
     Serial.println("Keep the compass sensor horizontal.");
     Serial.print("In the next "+String(calibrationTime));
     Serial.println(" seconds the compass must be turned slowly over a full 360 degrees!");

     calibrateMagnet(calibrationTime*1000);

     Serial.println("calibration results"); 
     Serial.print("IMU.magnetOffset[] : "); 
     for (int i= 0; i<=2 ; i++){ Serial.print(IMU.magnetOffset[i],4);Serial.print("  ");}
     Serial.println();
     Serial.print("IMU.magnetSlope[]  : "); 
     for (int i= 0; i<=2 ; i++){ Serial.print(IMU.magnetSlope[i],4);Serial.print("  ");}
     Serial.println();
     Serial.println("Press enter to start the compass");
     while (!Serial.available());    // pause the program, enter continues, use the serial plotter to view a graph
     while (Serial.available()) Serial.read();   // clear the read buffer
     Serial.println("Direction\tFieldStrength\tmagX\tmagY");          // legend for serial plotter
     IMU.magnetUnit = MICROTESLA;        // Switch to microtesla as this looks better in the graph

}

void loop ()
{  float magY,magX;
   doNMeasurements (averageCompassByN,magX,magY,false);
   float heading= atan2(magY,magX)*180/PI +180;
   float fieldStrength = sqrt(magX*magX +magY * magY);
   Serial.print(heading); Serial.print("\t");   Serial.print(fieldStrength);Serial.print("\t");
   Serial.print(magX); Serial.print("\t");   Serial.print(magY);Serial.print("\t");
   Serial.println();
}

void doNMeasurements(unsigned int N, float& averX, float& averY, boolean showLeds) 
{    float x, y, z;
     averX=0; averY =0;
     for (int i=1;i<=N;i++)
     {  digitalWrite(LED_BUILTIN, bitRead(millis(),7)&& showLeds );       // blink onboard led
        while (!IMU.magneticFieldAvailable());
        IMU.readMagneticField(x, y, z);
        averX += x/N;
        averY += y/N;
     } 
}
 
void calibrateMagnet(unsigned int duration)  // measure Offset and Slope of XY
{  float x, y, Xmin, Xmax, Ymin, Ymax  ;
   IMU.setMagnetOffset(0,0,0);                      // Offsets must be 0 when calibrating
   IMU.setMagnetSlope(1,1,1);                       // slopes must be 1 when calibrating
   doNMeasurements(averageCalibrationByN,Xmin, Ymin, true ); // find some starting values
   Xmax = Xmin; Ymax = Ymin;
   unsigned long  start = millis(); 
   while ((millis()-start) < duration)
   {    doNMeasurements(averageCompassByN ,x, y ,true);
        Xmax = max (Xmax, x); Xmin = min (Xmin, x);
        Ymax = max (Ymax, y); Ymin = min (Ymin, y);
   } 
   IMU.setMagnetOffset( (Xmax+Xmin)/2,(Ymax+Ymin)/2, 0 ) ;                                      // store offset
   IMU.setMagnetSlope ( (2*EarthMagField)/(Xmax-Xmin),(2*EarthMagField)/(Ymax-Ymin),1) ;        // store slope  
//   Serial.print("Xmin = ");Serial.print(Xmin); Serial.print("  Xmax = ");Serial.println(Xmax);
//   Serial.print("Ymin = ");Serial.print(Ymin); Serial.print("  Ymax = ");Serial.println(Ymax);
}   
