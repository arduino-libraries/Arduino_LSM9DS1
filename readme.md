# Notes on version 2.0 of the LSM9DS1 library

## Written by [Femme Verbeek]( https://www.linkedin.com/in/femmeverbeek/ )
### Pijnacker, The Netherlands
### 11 june 2020

----------------------------------------------------------------------------
links:

[LSM9DS1 datasheet](https://www.st.com/resource/en/datasheet/lsm9ds1.pdf)

[DIY Calibration instruction video](https://youtu.be/BLvYFXoP33o)

[Information about Earth Magnetic field](https://en.wikipedia.org/wiki/Earth%27s_magnetic_field)

[Geomagnetic Calculator of the NOAA](https://www.ngdc.noaa.gov/geomag/calculators/magcalc.shtml?#igrfwmm)

-----------------------------------------------------------------------------
## Contents: 	
1. Introduction
1. Naming strategy
1. Output of Read method
1. Output Data Rate (ODR)
1. Full Scale setting (FS)
1. Band Width setting (BW)
1. Calibration
	* Offset
	* Slope
1. Overview of Code
1. Derivation of linear correction
	* Offset
	* Slope

--------------------------------------------------------------------
## 1 Introduction   
----------------------------------------------------------------------

The reason for writing this update is that the LSM9DS1 chip has several settings that can be used to tweak the measurement
results. The sensors are not calibrated and the output may vary per instance of the chip.
In my case the magnetic field offset was larger than the Earth magnetic field. The gyroscope offset of 3 deg/s does not sound 
like much, but when trying to track the orientation it corresponds to a full circle misalignment in two minutes. Without 
calibration it is impossible to make a working magnetic or gyro compass, artificial horizon etc.

This new version 2 provides three DIY calibration sketches, for the accelerometer, gyroscope and magnetometer. They give instructions
on what to do during a calibration measurement. For more clarity a [DIY Calibration instruction video](https://youtu.be/BLvYFXoP33o) was made.
The DIY calibration sketches return the results on screen as code that can be copy/pasted in a sketch. For a rough calibration you 
have to do this only once per instance of the chip. But the magnetic field measurement is very easily disturbed by the set-up
the chip is mounted in. So for the magnetometer it is advised to do an in-situ calibration. 

The calibration method in this library gives a rather basic linear correction, which will be sufficient in most cases. 
When used in combination with sensor fusion algorithms, quaternions, Euler transformations, those libraries usually come with 
their own calibration methods. In those cases it may be better not to use the built-in calibration of this library.  


New for all 9 DOF (degrees of freedom) is the possibility:
* to change the Output Data Rate (ODR), including fast rate magnetic sampling. The values returned by get...ODR are 
      now the actual values rather than those in the documentation.   
* to change the internal full scale setting of the chip (FS) giving it more accuracy at the expense of the range 
* to change the band width filtering of the chip
* to change the output unit
* to give it calibration zero offset and slope factors
* to change the operational mode (off, Acceleration only, Acceleration + Gyroscope

The values returned by the read... methods change according to the settings. If left to their default, the output will still
be the same as from version 1.1.0. One exception was made for the magnetometer. The default sample rate of 20Hz did not work on 
all set-ups. It has been changed to 40Hz. If you really need the 20Hz, the sketch call is *IMU.setMagnetODR(5);*

In theory all the settings and calibration factors are independent of each other. 

This means that 
* changing the FS does not change the read... value, giving it more accuracy at the expense of the range.
* calibration of slope and offset factors can be done combined or separately in any following order.
* calibration can be done in any chosen Unit, FS, ODR, 
* the calibration factors can be copy/pasted in a sketch. They dont need changing when the sketch
uses any other setting combination of Unit, FS or ODR.

In reality the gyroscope offset showed a small dependence on the FS setting. For this reason the possibility to change the settings
was added in the DIY calibration sketches. This improved the drift behavior of the gyro significantly.

-------------------------------------------------
## 2 Naming strategy
-------------------------------------------------

This new version 2.0 of the library supports al the function calls available in version 1.1.0
Keeping the same naming convention of version 1.1.0 resulted in very long names for the new functions, making formulas
difficult to read, increasing the chance of making typo's and it was not always clear what was meant. 
For this reason a number of shorter aliases were created, most of them following the [datasheet](https://www.st.com/resource/en/datasheet/lsm9ds1.pdf)

new  	|	alias
-----	| ---------------
Accel	|	Acceleration 
Gyro	|	Gyroscope 
Magnet	|	MagneticField 
ODR 	|	SampleRate
FS		|	FullScale 
BW		|	BandWidth 

Addition of "set" and "get" in the names of ODR, BW and FS functions. 

e.g. *magneticFieldSampleRate* still works, but in the library code it is now *getMagnetODR* reflecting that it is not longer 
a constant but a function corresponding to the LSM9DS1 chip setting. 

Not used were the datasheet's XL, M and G since it may confuse with gravity, Gauss and of course the size of clothes. :tshirt:

#### In the text below   ...   stands for any of the three measurement properties, Accel, Gyro, or Magnet. 

------------------------------------------
## 3 Output of Read method
-----------------------------------------

There are several ways in which a linear correction can be applied. The chosen method has the highest amount of advantages. 
The offset and slope are independent of each other and of any of the other settings in the program. They can be measured 
independently and counteract only the internal transducer differences. (For mathematical derivation see below)

There are two read methods for each of the properties ...
```
   readRaw... =  (FS / 32786) * Data
```   
 and  
 
```
   read... 	=	Unit * Slope *( readRaw... - Offset )
```  
Data = the measurement value showing up on the chip registers	
FS = the in Program Full Scale setting (float get...FS() ) counteracting the internal chip setting so that 
      the output result remains unchanged. 	
Unit = the unit the measured physical property is expressed in. 
Slope and Offset = calibration parameters.

readRaw... produces dimensionless uncalibrated output and is the method to be used when calibrating.
read... produces calibrated output in the unit of your choice. 

Note that when Unit, Slope and Offset are left to their default values, the read... and readRaw... methods 
produce the same output, identical to that of library version 1.01.

Measuring the offset shows that it scales roughly with the chip internal full scale setting (IFS). That means that it is caused 
by the internal transducer and not by the ADC. So it is sufficient to calibrate for just one of the full scale settings 
and compensate for the others by means of a multiplication (FS). The small dependency of the gyroscope offset on the FS setting 
was discovered only recently. Calibrating and running at the same FS setting improved the accuracy especially of the gyro drift.  

------------------------------------------
## 4 Output Data Rate (ODR)
-----------------------------------------

This library offers the possibility of changing the sample rate of the LSM9DS1 chip.
```
	set...ODR(); //changes the sample rate according to the table below
	get...ODR(); //returns the actual ODR value (Hz) that was previously measured
```
The ODR values According to the datasheet

| nr |setAccelODR(nr)| setGyroODR(nr)|setMagnetODR(nr) |
|:----:|:-------------:|:-------------:|:-----------------:|
|0   | Gyro&Accel off | Gyro off		| 0.625 Hz |
|1   | 10 Hz			| 10 Hz			| 1.25 Hz |
|2   | 50 Hz			| 50 Hz			| 2.5 Hz|
|3   | 119 Hz		| 119 Hz		| 5 Hz|
|4   | 238 Hz		| 238 Hz		| 10 Hz|
|5   | 476 Hz		| 476 Hz		| 20 Hz|
|6   | don't use		| don't use		| 40 Hz|
|7   |  | | 80 Hz|
|8   |  | | 400 Hz|

As it turned out the actual ODR values could deviate significantly from the ones in the datasheet. 
For that reason the set...ODR functions quickly measure the actual ODR value upon calling. This
actual value is returned by the get...ODR functions. 
Settings 6 of Accel and Gyro were supposed to deliver 952 Hz. The result was hardly 
faster than that of setting 5 but with a lot more noise. For this reason it is not adviced to use them.

The magnetometer settings 0..5 turned out not to be working on a separate LSM9DS1 connected to an Arduino Uno 
On the same set-up setAccelODR(1) and setGyroODR(1) did not work either.

To save on power demand the Gyro or both Gyro+Accel can be switched off by setting ...ODR(0). 
In any other setting Gyro and Accel share their ODR, so changing one, changes the other. 

-------------------------------------------------
## 5 Full Scale setting (FS)
--------------------------------------------------
This library offers the possibility of changing the Full Scale setting of the LSM9DS1 chip. It changes the 
chip's internal multiplier, assigning more bits to the measurement but limiting it's range.
```
	set...FS(); //changes the FS according to the table below
	get...FS(); //returns the FS multiplier
```
FS settings 

| nr |setAccelFS(nr)| setGyroFS(nr)|setMagnetFS(nr) |
|:----:|:-------------:|:-------------:|:-----------------:|
|0   | ±2 g	| ±245 °/s | ±400 µT|
|1   | ±24 g | ±500 °/s	| ±800 µT|
|2   | ±4 g	| ±1000 °/s	| ±1200 µT|
|3   | ±8 g	| ±2000 °/s	| ±1600 µT|

Datasheet anomalies
*  Upon  *setAccelFS(1);*  the needed multiplication factor should be 16 but turns out to be 24, 
   but the sensor maxes out at 20 g  
*  The   *setGyroFS(2);*  is *NA* according to the datasheet, but when tested it worked nicely at 1000 °/s 

-------------------------------------------------
## 6 Band Witdh setting (BW)
--------------------------------------------------

The possibility of setting the band-width filtering was added for the purpose of getting rid of a very nasty 
spike in the gyro/accel signal at the highest ODR. It turned out that there was no influence on the spike at all.
The methods are provided "as is". The set values can be found in the [datasheet](https://www.st.com/resource/en/datasheet/lsm9ds1.pdf)
tables 46, 47 and 67. 
```
	set...BW(); 
	get...BW(); 
```

-------------------------------------------------
## 7 Calibration
--------------------------------------------------

This library has a linear calibration possibility built in. 
For each property, Accel, Gyro and  Magnet and each of the three directions (x, y, z) 
a set of two factors (Slope and Offset) must be found. So in total 18 calibration factors. 
Setting their values correct, is all that is needed to change the output of the read...() methods from raw 
into calibrated values. 

An interactive DIY_Calibration program is provided with this library. See [instruction video](https://youtu.be/BLvYFXoP33o)
It explains how to measure each of the factors, collects data and presents the result on screen as 
copy/paste-able code. No special setup is required, but it helps to fix the board in a non-magnetic rectangular box. 

![](/images/Boxed_sensor.PNG)

The calibration factors may be different per instance of the LSM9DS1 chip. They will hardly vary in time, 
so it is sufficient to calibrate them only once. The magnetic field is easily disturbed by even the smallest
pieces of metal in the setup (car, drone, ...). An "in-situ" calibration is advised for the magnetic property.

The calibration values don't depend on any of the settings in the program, like ...Unit, 
...ODR, ...FS (full scale). You can change them in your sketch without the need to recalibrate. 

Sensor fusion programs usually have their own calibration methods. It is not advised to mix them unless you know 
what you are doing. You will loose the advantage of the independency of parameters though. 
If you want the fusion program to be using a changed ODR or FS setting, you may have to add some codelines 
in the library itself in the file LSM9DS1.CPP / in the method int LSM9DS1Class::begin()
No guarantee that it works. 

If you want to design your own calibration method, the text below gives more explanation on what to do.
It is vital that the calibration measurements are done with the raw... functions to get uncalibrated data

------------------------------------------------------------------------------------------
### Offset

In order to find the value of Offset we must do a zero point measurement. That means that the physical property ... 
we are trying to measure should actually result in a value of 0. E.g. keeping the board still should result in zero 
gyroscope values for x,y and z. For acceleration the two axes perpendicular to the Earth gravity should be zero. 

The offset calibration values are stored in the arrays 
```
    ...Offset[3]={0,0,0} 
```
The value ...Offset should get: 
```
     ...Offset = readRaw... (@zero-point) 
```
so the (average) output of the readRaw... functions during a zero point measurement. 
In the calibrated read... function its value gets subtracted from readRaw, so the average read... output will 
equal 0 in a zero point experiment.

Alternatively if it is difficult to get a zero property, you could try to find the maximum and minimum values and 
use the point in the middle

		readResult = ( Read_max + Read_min ) / 2

This is probably more complex than what it looks like. Both Read_min and Read_max can best be some sort of average.  
If you simply use the min() and max() values of a lot of measurements, the final results are still just from one measurement
each. Most probably these are the outlyers in both directions, so actually the worst measurements. 

Calculating separate averages of the min and max values is a better method but also like a chicken and egg problem. 
Before the calibration program can decide to which average a measurement belongs, it needs a rough calibration first.
If the offset is very large, like in case of the magnet, this is not straight forward. The DIY calibration program 
uses the fastrate magnet ODR and simply calculates a lot of averages and takes the min and max of that.

The best method is probably 3D elliptical regression, where the values of Offset correspond to the centre of the 
ellipsoid. So far I did not venture into the mathematics of this. :dizzy_face:

--------------------------------------------------------------------------------------------------------------------
### Slope 

Slope is a dimensionless number that compensates for the sensitivity of the chip's internal transducer. Ideally it's value 
should equal 1, meaning that the sensitivity is what it's supposed to be. I my case the accelerometer was very close, the 
gyro 17%, 13% and 4% to low, the magnetometer 5%, 4% and 7% too high. For the magnet and the Accel only their relative 
values matter, but for the Gyro it's the absolute value if you want to keep track of orientation. 17% means 61 degrees 
misalignment on a full turn.

In order to calibrate the Slope parameter at least two measurements (say 1 and 2) must be done where the physical quantity, 
(let's call it Q), has a known difference, so Q_1 - Q_2 = known value.

The value of slope follows from 
```
		Result = abs (( Q_1 - Q_2) / ( readRaw_1 - readRaw_2 ))      
```
The slope calibration values are stored in the arrays 
```
   ...Slope[3]={1,1,1} 
```
E.g. The local data of the magnetic field (in nT) can be found at [Wikipedia](https://en.wikipedia.org/wiki/Earth%27s_magnetic_field)
By pointing the sensor axes in both ways in the direction of the magnetic field-lines the difference in reading should be 
2x the given field intensity. So 
```
   magnetSlope[i]= (2*Intensity)/(NANOTESLA*(readRawMagnet_1 -readRawMagnet_2))
```

![](/images/Aim_axes_along_magnetic_field_lines.PNG)


E.g. the Earth gravity should produce a value of 1g. Holding the board upside down should measure -1g. So the difference
```	
	( Q_1 - Q_2) = 2g  
```
Lets assume that readAccel produces values of 1.3 and - 0.9 
```
		Offset = (1.3 + (-0.9)) /  2  = 0.2    
		Slope  =  (1 - (-1)) / (1.3 - (-0.9) = 2 / 2.2 = 0.90909
```

----------------------
## 8 Overview of Code
----------------------

(almost) unchanged 
```
    int begin();
    void end();
    void setContinuousMode();
    void setOneShotMode();
    int accelAvailable(); // Number of samples in the FIFO
    int gyroAvailable(); // Number of samples in the FIFO
    int magnetAvailable(); // Number of samples in the FIFO  
```

existing functions changed for new functionality. Results reflect calibration, full scale and unit settings 
```
    int readAccel(float& x, float& y, float& z); // Results are in G (earth gravity) or m/s2
    int readGyro(float& x, float& y, float& z); // Results are in degrees/second or rad/s
    int readMagnet(float& x, float& y, float& z); // default in uT (micro Tesla)  
```
new functions resembling the old read functions, use these for calibration purposes
```
    int rawAccel(float& x, float& y, float& z); // Return uncalibrated results  
    int rawGyro (float& x, float& y, float& z); // Return uncalibrated results 
    int rawMagnet(float& x, float& y, float& z); // Return uncalibrated results 
```
existing functions that now return a previously measured value 
```
    float getAccelODR(); // Sampling rate of the sensor (Hz)
    float getGyroODR(); // Sampling rate of the sensor (Hz)
    float magnetODR(); // Sampling rate of the sensor (Hz)  
```
New the possibility to change the ODR registers (Output Data Rate)	
```
    int setAccelODR(int8_t range); // 0:off, 1:10Hz, 2:50Hz, 3:119Hz, 4:238Hz, 5:476Hz, 6:952Hz
    int setGyroODR(int8_t range); // 0:off, 1:10Hz, 2:50Hz, 3:119Hz, 4:238Hz, 5:476Hz, 6:952Hz
    int setMagnetODR(int8_t range)  // range (0..7) corresponds to {0.625,1.25,2.5,5,10,20,40,80,400}Hz	 
```
Notes: 
1. The 952Hz Accel and Gyro ODR does not seem to work. It's true value is close to setting 5,
      but it produces a lot more noise in the form of spikes
1. The Magnetic 400Hz setting corresponds to the chip's fast rate setting
1. Changing any of the settings quickly measures the actual ODR value for the get...ODR to return.

New constants used in ...Unit setting
```
#define GAUSS             0.01           
#define MICROTESLA        1.0       // default
#define NANOTESLA         1000.0  
#define GRAVITY           1.0       // default
#define METERPERSECOND2   9.81  
#define DEGREEPERSECOND   1.0       //default
#define RADIANSPERSECOND  3.141592654/180
#define REVSPERMINUTE     60.0/360.0 
#define REVSPERSECOND     1.0/360.0  
```
Unit settings that read... returns measurement results in.
```
    float accelUnit = GRAVITY;          //  GRAVITY   OR  METERPERSECOND2 
    float gyroUnit = DEGREEPERSECOND;   // DEGREEPERSECOND,  RADIANSPERSECOND, REVSPERMINUTE, REVSPERSECOND
    float magnetUnit = MICROTESLA;      //  GAUSS   MICROTESLA  NANOTESLA 
```

methods for internal use 
```
    int getOperationalMode(); //0=off , 1= Accel only , 2= Gyro +Accel
    float measureAccelGyroODR(unsigned int duration);
    float measureMagnetODR(unsigned int duration);  
```

Calibration parameters Slope ans Offset : See Calibration.
```
    float accelOffset[3] = {0,0,0}; // zero point offset correction factor for calibration
    float gyroOffset[3] = {0,0,0};    // zero point offset correction factor for calibration
    float magnetOffset[3] = {0,0,0};// zero point offset correction factor for calibration
    float accelSlope[3] = {1,1,1};  // slope correction factor for calibration
    float gyroSlope[3] = {1,1,1};     // slope correction factor for calibration
    float magnetSlope[3] = {1,1,1}; // slope correction factor for calibration  
```

Methods for setting the calibration 
```
    void  setAccelOffset(float x, float y, float z);  
    void  setAccelSlope(float x, float y, float z);
    void  setGyroOffset(float x, float y, float z);  
    void  setGyroSlope(float x, float y, float z);
    void  setMagnetOffset(float x, float y, float z);
    void  setMagnetSlope(float x, float y, float z); 
```

New: changing the full scale sensitivity of the sensors.
The functions modify the FS (full scale) registers of the LSM9DS1 chip changing sensitivity at the expense of range.
Changing this setting does not change the x,y,z output of the read functions, but assigns just more or less bits
to the sensor measurement. 
```
    int setAccelFS(uint8_t range); // 0: ±2g ; 1: ±24g ; 2: ±4g ; 3: ±8g
    int setGyroFS(int8_t range); // 0= ±245 dps; 1= ±500 dps; 2= ±1000 dps; 3= ±2000 dps
    int setMagnetFS(int8_t range); // 0=400.0; 1=800.0; 2=1200.0 , 3=1600.0  (µT) 
```
*note* According to the data sheet gyroscope setting 2 = 1000 dps should not be available. For some reason it worked 
  like a charm on my BLE Sense board, so I added the possibility.

New functions return the Full Scale setting of the corresponding DOF as set with the corresponding set...FS functions
```    virtual float getAccelFS(); // Full Scale setting (output = 2.0,  24.0 , 4.0 , 8.0)  
    virtual float getGyroFS();   // output = 245.0,  500.0 , 1000.0, 2000.0) 
    virtual float getMagnetFS(); // output 400.0 ; 800.0 ; 1200.0 , 1600.0     
```
Note: According to the datasheet setAccelFS(1) should correspoond to 16g. Measurement showed that it is actually 24g
    but that the sensor maxes out at 20g. Since the actual value is used in a calculation the value of 24g is returned
	by getAccelFS for that setting.
	
The band with routines were added as an attempt to dampen a nasty spike that existed in the Gyro signal. It had no
observable effect. The methods are provided as is. 	
```    virtual float setAccelBW(uint8_t range); //0,1,2,3 Override autoBandwidth setting see doc.table 67
    virtual float getAccelBW();  //Bandwidth setting 0,1,2,3  see documentation table 67
    virtual int   setGyroBW(uint8_t range);  //Bandwidth setting 0,1,2,3  see documentation table 46 and 47
    virtual float getGyroBW();  //Bandwidth setting 0,1,2,3  see documentation table 46 and 47 
```	
-----------------------------------------
## 9 Derivation of linear correction method 
-------------------------------------------

Sorry for the very formal derivation below. It suits verifiability but probably only my own purpose :nerd_face:
It took a lot of puzzling to get it right. Don't read it if you don't want to. :dizzy_face:
The readRaw... method was introduced after this derivation was written. ReadRaw... produces output equal to Read... but with
boundary conditions Offset=0, Slope=1 and unit = 1 (default). 

-----------------
### Offset 

Assuming good linearity of the transducer, we can model the data output of the chip as

            Data = (32768 / IFS) *( A*Q + B)												(1)

Data = the measured value showing up on the chip registers
Q = the actual physical quantity we are trying to measure in any of the 9 DOF
IFS = the chip Internal Full Scale Setting.  
A,B unknown constants representing chip instance differences

Since the chip outputs dimensionless bits and bytes only, the dimensions of IFS and B must equal the dimension of Q.  

The challenge is to get rid of the unknown constants A and B and translate them into measurable quantities produced by the 
library's Read methods. Since the calibrated Read should produce a number equal to the actual physical quantity 
we can state  

			Read = Q	
			
Further, we do not want to recalibrate when we change the Full Scale setting or the Unit.

Define
FS = the in-program Full Scale function counteracting the IFS so that the output result remains unchanged. 
      (dimensionless, but its value corresponds to that of the chosen default unit) 
Slope = in-program correction factor for the sensor sensitivity
Offset = in-program correction factor for the sensor zero point offset

The output of the read methods is (see above)

		Read 	=	Unit * Slope * (FS / 32786 * Data - Offset )							(2)
			 
Substitute eq(1)

		Read	=	Unit * slope * (FS / IFS * 32786/32786 * ( A*Q + B ) - Offset )
	
		Read	=	Unit * slope * (FS / IFS * ( A*Q + B ) - Offset )						(3)
	
In case of the calibrated zero point measurement   Q = Read = 0   
Substitute  in eq.(3) we get

		0	=	Unit * slope * (FS / IFS * ( A*0 + B ) - Offset )	

		Unit * slope *	Offset  =	Unit * slope * FS / IFS * B 							(4)
		
For an uncalibrated measurement of a zero point (ZP) filling in  Offset=0 and Q=0 in eq(3) we get 
	
		Read_uncalibrated_ZP  =  Unit * slope * FS / IFS * B								(5)

The righthand terms in equation (4) and (5) are identical so it follows that 

		Offset  =  read_uncalibrated_ZP / (Unit * slope)									(6)

This defines how we should do the calibration measurements. In the library the methods called set...Offset 
use of eq(6) to assign the uncalibrated zero-point read values to the corresponding program parameters.
Eq.(6) suggests that Offset depends on Unit and Slope, but this is not the case as the Read method scales 
with the same value. To proof this we write eg(4) in a different form

		Offset	=  (Unit * Slope) / (Unit * Slope) * FS/IFS *  B   						(7)
  
Further	since the dimensionless FS counteracts IFS in size 
and the dimension of IFS equals that of Q, the ratio FS/IFS is 1/Unit.
or in other words  

		Unit * FS /IFS = 1																	(8)
			
eq(7) reduces to  

		Offset  = B	/ Unit	= B_dimensionless																(9)

So Offset equals the dimensionless form of B and does not depend on any other parameter.

-----------------
### Slope 

In order to calibrate the Slope at least two measurements (say 1 and 2) must be done where the measured quantity Q 
has a known difference. The calibrated Read equals Q so

		Q_1 - Q_2 	= Read_1 - Read_2 = known value											(10)

Substitute eq(2)

		Q_1 - Q_2	= Unit*Slope*(FS/32786*Data_1 - Offset) - Unit*Slope*(FS/32786*Data_2 - Offset)

					= Unit*Slope*(FS/32786*(Data_1 - Data_2)								(11)
						
So the difference between the measurements gets rid of the Offset
For an uncalibrated measurement we must set   Slope = 1. 
Eq(2) becomes 

		Read_uncalibrated  =  Unit * (FS / 32786 * Data - Offset )

In eq(11) we get

		Q_1 -Q_2  = Slope * ( Read_uncalibrated_1 - Read_uncalibrated_2)

		Slope 	=	(( Read_uncalibrated_1 - Read_uncalibrated_2) / (Q_1 -Q_2)				(12)

So in order to measure the slope we must first set the Slope parameter to 1. 

In order to prove that Slope is independent of all the other program parameters we substitute eq(1) in eq(11)

		Q_1 - Q_2	= Unit*Slope*(FS/32786)*(32786/IFS)*(A*Q1 +B - A*Q2 -B))
						
	   (Q_1 - Q_2)	= Slope*  Unit*(FS/IFS)* A *(Q1 - Q2)									(12)

and with eq(8) 			

		(	1	)	= Slope *      1        * A *(   1   )
			
			Slope 	= 1 / A
 
So Slope only depends on A, the (in)sensitivity of the sensor. The proportionality is reciprocal since Slope is the
compensation factor for the chip's insensitivity

QED
