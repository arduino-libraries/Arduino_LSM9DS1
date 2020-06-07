Notes on version 2.0 of the LSM9DS1 library
Written by Femme Verbeek, Pijnacker, The Netherlands
https://www.linkedin.com/in/femmeverbeek/
5 june 2020


Contents: 	
			Introduction
			Naming strategy
			Output of Read method
			Calibration
				Offset
				Slope
				Offset and Slope Combined
			Overview of Code
			Derivation of linear correction
				Offset
				Slope

*****************************************************************************************************************************
*****************************************            Introduction              **********************************************
*****************************************************************************************************************************

The reason for writing this update is that the LSM9DS1 chip has several settings that can be used to tweak the measurement
results. The sensors are not calibrated and the output may vary per instance of the chip.
In my case the magnetic field offset was larger than the Earth magnetic field. The gyroscope offset of 3 deg/s does not sound 
like much, but when trying to track the orientation it corresponds to a full circle misalignment in two minutes. Without 
calibration it is impossible to make a working magnetic or gyro compass, artificial horizon etc.

This new version 2 provides a simple DIY calibration program that gives instructions on how to do measurements. It returns
the parameters on screen so that they can be copy/pasted directly into the code of your sketch. For a rough calibration you 
have to do this only once per instance of the chip. But the magnetic field is very easily disturbed. In a different set-up 
the chip may return completely different magnetic values.

In spite of some examples, this library does not prescribe the methods for measuring or calibrating, provide quaternions or
Euler transformations. The development of these is still the task of the user of this library. The examples are just for 
learning, but they are not part of the library code itself. 

New for all 9 DOF (degrees of freedom) is the possibility:
   to set and get the Output Data Rate (ODR), including fast rate magnetic sampling. The values returned by get...ODR are 
      now the actual values rather than those in the documentation.   
   to change the internal full scale setting of the chip (FS) giving it more accuracy at the expence of the range 
   to change the band width filtering of the chip
   to change the output unit
   to give it calibration zero offset and slope factors
   to change the operational mode (off, Acceleration only, Acceleration + Gyroscope

The values returned by the read... methods change according to the settings. If left to their default, the output will still
be the same as from version 1.1.0. 
The way this is organised is rather unique. (mathematical derivation at the end of this document)

               All the settings and calibration factors are independent of each other. 

This means that 
- changing the FS does not change the read... value, giving it more accuracy at the expense of the range.
- calibration of slope and offset factors can be done combined or separately in any following order.
- calibration can be done in any chosen Unit, FS, ODR, the factors copy/pasted in a sketch which on turn can use the LSM9DS1
  in any other combination of Unit, FS or ODR.

 
*****************************************************************************************************************************
*****************************************            Naming strategy           **********************************************
*****************************************************************************************************************************

Keeping the same naming convention of version 1.1.0 resulted in very long names for the new functions, making formulas
difficult to read, increasing the chance of making typo's and it was not always clear what was meant. 
For this reason a number of shorter aliases were created most of them following the datasheet: 
https://www.st.com/resource/en/datasheet/lsm9ds1.pdf

Accel 	for Accelleration, 
Gyro 	for Gyroscope, 
Magnet 	for MagneticField, 
ODR 	for SampleRate (= Output Data Rate), 
FS 		for FullScale, 
BW 		for BandWdth, 
addition of "set" and "get" in the names of ODR, BW and FS functions. 

e.g. magneticFieldSampleRate still works, but in the library code it is now getMagnetODR reflecting that it is not longer 
a constant but a function corresponding to the LSM9DS1 chip setting. 

Not used were the documentation's XL, M and G since it may confuse with gravity, Gauss and of course the size of clothes. :) 

 
*****************************************************************************************************************************
**********************************            Output of Read method              ****************************************
*****************************************************************************************************************************

Measuring the offset shows that it scales with the chip internal full scale setting (IFS). That means that it is caused 
by the internal transducer and not by the DAC. So it is sufficient to calibrate for just one of the full scale settings 
and compensate for the others by means of a multiplication.  

The output of the read methods for each of the 9 DOF is now

	Read 	=	Unit * Slope *(  (FS / 32786) * Data - Offset )

Data = the measurement value showing up on the chip registers	
FS = the in Program Full Scale setting (float LSM9DS1Class::get...FS() ) counteracting the internal chip setting so that 
      the output result remains unchanged. 	
Unit = the unit the measured physical property is expressed in. 
Slope and Offset = calibration parameters.
Note that the read method still produces the same output as in library version 1.01 on the condition that Slope = 1; 
Offset=0; and Unit set to its default value.

There are several ways in which a linear correction can be applied. The chosen method has the highest amount of advantages. 
The offset and slope are independent of each other and of any of the other settings in the program. They can be measured 
independently and really counteract the internal transducer differences. (For mathematical derivation see below)
All Unit factors equal 1 for their default settings. 


*****************************************************************************************************************************
**********************************                   Calibration                     ****************************************
*****************************************************************************************************************************

In the text below ... stands for any of the three measurement properties, Accell, Gyro, Magnet. 

For a full calibration we must find a set of two factors (Slope and Offset) for each property ...  and each of the three
directions (x, y, z). So in total 18 calibration factors. 

Following the procedures below, the calibration values are stored in the arrays ...Offset[3]={0,0,0} and ...Slope[3]={1,1,1} 
Should you want to use these values again in a sketch, it's up to you to print them and put them in program code. 
The library is designed in such a way that the stored values don't depend on any of the settings in the program, like ...Unit, 
...SR  (sample rate), ...FS (full scale). So changing any of the settings before or after restoring their values in the
arrays has no effect on the status of being calibrated. 

*********************************************              Offset               *********************************************

In order to find the value of Offset we must do a zero point measurement. That means that the physical property ... 
we are trying to measure should actually result in a value of 0. E.g. keeping the board still should result in zero 
gyroscope values for x,y and z. For acceleration the two axes perpendicular to the Earth gravity should be zero. 

An offset calibration involves three steps
1 set the offset factors to 0, using the set...Offset method
2 do zero-point measurements with the read... method 
3 store the measured ReadResults using the same set...Offset method to make the calibration effective.

ad.1: in your sketch: IMO.set...Offset(0,0,0); 
ad.2: The readResult should probably be the average value of a number of measurements
ad.3: in your sketch: IMO.set...Offset(resultX,resultY,resultZ); 

Alternatively if it is difficult to get a zero property, you could try to find the maximum and minimum values and 
use the point in the middle

		readResult = ( Read_max + Read_min ) / 2

This is probably more complex than what it looks like. Both Read_min and Read_max can best be some sort of average, but 
if you simply use the min() and max() procedures, the final results are just one measurement each. The chances are high 
that these are the worst measurements. 
Calculating separate averages of the min and max values, is like a chicken and egg. Before the calibration program can
decide to which average a measurement belongs, it needs a rough calibration first.
The best method is probably 3D elliptical regression, where the values of Offset correspond to the centre of the 
elliptoid. So far I did not venture into the mathematics of this. :0 

*********************************************              Slope               *********************************************
  	
Slope is a dimensionless number that compensates for the sensitivity of the chip's internal transducer. In case it is not
possible to measure this, best leave it at the value 1. For Magnet and Accel this is probably allright, but for Gyro the 
sensitivity matters if you want to keep track of orientation.

A slope calibration involves three steps
1 set the offset factor to 1, using the set...Slope method
2 do slope measurements with the read... method 
3 store the readResult using the same set...Slope method.

ad.1: in your sketch: IMO.set...Slope(1,1,1); 
ad.2: The read result should probably be two different averages of a number of measurements each
ad.3: in your sketch: IMO.set...Slope(resultX,resultY,resultZ) 

In order to calibrate the Slope parameter at least two measurements (say 1 and 2) must be done where the physical quantity, 
(let's call it Q), has a known difference, so Q_1 - Q_2 = known value.

The value of slope follows from 

		Result = abs (( Q_1 - Q_2) / ( Read_1 - Read_2 ))      

E.g. the Earth gravity should produce a value of 1g. Holding the board upside down should measure -1g. So the difference
		( Q_1 - Q_2) = 2g
The local strength of the magnetic field (in nT) can be found at
https://en.wikipedia.org/wiki/Earth%27s_magnetic_field


***************************************        Offset and Slope Combined           *********************************************

With the above methods Slope and Offset can be measured independently. It does not matter which procedure comes first, 
as long as you don't change any of the settings between measuring and assigning. This could accidently happen in case 
of a combined measurement by setting the value of Slope before that of Offset.
Note that the dimensionless stored Offset values differ from the Read measurements by a factor (Unit * Slope).


A safe following order of steps is
1 set the Slope factors to 1,          set...Slope(1,1,1); 
2 set the Offset factors to 0,         set...Offset(0,0,0);
3 do the measurements with the         read... method 
4 First store the Offset results using set...Offset
5 Store the Slope results second using set...Slope

Example
Lets assume that readAccel produces values of 1.3 and - 0.9 

		Offset = (1.3 + (-0.9)) /  2  = 0.2    
		Slope  =  (1 - (-1)) / (1.3 - (-0.9) = 2 / 2.2 = 0.90909

Note that set...Offset stores the measured values as ReadResult/(Unit * Slope).
If you store Slope first, the Offset becomes 0.2/0.90909 = 0.22

*****************************************************************************************************************************
**********************************                Overview of Code                   ****************************************
*****************************************************************************************************************************

(almost) unchanged 
    int begin();
    void end();
    void setContinuousMode();
    void setOneShotMode();
    virtual int accelAvailable(); // Number of samples in the FIFO.
    virtual int gyroAvailable(); // Number of samples in the FIFO.
    virtual int magnetAvailable(); // Number of samples in the FIFO.

existing functions changed for new functionality. Results reflect calibration, full scale and unit settings 
    virtual int readAccel(float& x, float& y, float& z); // Results are in G (earth gravity) or m/s2.
    virtual int readGyro(float& x, float& y, float& z); // Results are in degrees/second or rad/s.
    virtual int readMagnet(float& x, float& y, float& z); // Results are in uT (micro Tesla).

existing functions that return a previously measured value 
    virtual float getAccelODR(); // Sampling rate of the sensor (Hz).
    virtual float getGyroODR(); // Sampling rate of the sensor (Hz).
    virtual float magnetODR(); // Sampling rate of the sensor (Hz).
	
New the possibility to change the ODR registers (Output Data Rate)	
    virtual int setAccelODR(int8_t range); // 0:off, 1:10Hz, 2:50Hz, 3:119Hz, 4:238Hz, 5:476Hz, 6:952Hz
    virtual int setGyroSR(int8_t range); // 0:off, 1:10Hz, 2:50Hz, 3:119Hz, 4:238Hz, 5:476Hz, 6:952Hz
    virtual int setMagnetSR(int8_t range)  // range (0..7) corresponds to {0.625,1.25,2.5,5,10,20,40,80,400}Hz	

Note: The 952Hz Accel and Gyro ODR does not seem to work. It's true value is close to setting 5.
Note: The Magnetic 400Hz setting corresponds to the chip's fast rate setting
Note: Changing any of the settings quickly measures the actual ODR value for the get...ODR to return.

New constants used in ..Unit setting
#define GAUSS             0.01           
#define MICROTESLA        1.0       // default
#define NANOTESLA         1000.0  
#define GRAVITY           1.0       // default
#define METERPERSECOND2   9.81  
#define DEGREEPERSECOND   1.0       //default
#define RADIANSPERSECOND  3.141592654/180
#define REVSPERMINUTE     60.0/360.0 
#define REVSPERSECOND     1.0/360.0

Unitsettings that read returns measurement results in.
	float accelUnit = GRAVITY;          //  GRAVITY   OR  METERPERSECOND2 
    float gyroUnit = DEGREEPERSECOND;   // DEGREEPERSECOND,  RADIANSPERSECOND, REVSPERMINUTE, REVSPERSECOND
    float magnetUnit = MICROTESLA;      //  GAUSS   MICROTESLA  NANOTESLA

methods for internal use 
    int getOperationalMode(); //0=off , 1= Accel only , 2= Gyro +Accel
    float measureAccelGyroODR(unsigned int duration);
    float measureMagnetODR(unsigned int duration);

Calibration parameters Slope ans Offset : See Calibration.
    float accelOffset[3] = {0,0,0}; // zero point offset correction factor for calibration
	float gyroOffset[3] = {0,0,0};    // zero point offset correction factor for calibration
	float magnetOffset[3] = {0,0,0};// zero point offset correction factor for calibration
	float accelSlope[3] = {1,1,1};  // slope correction factor for calibration
	float gyroSlope[3] = {1,1,1};     // slope correction factor for calibration
	float magnetSlope[3] = {1,1,1}; // slope correction factor for calibration

Methods for setting the calibration 
	virtual void  setAccelOffset(float x, float y, float z);  
	virtual void  setAccelSlope(float x, float y, float z);
	virtual void  setGyroOffset(float x, float y, float z);  
	virtual void  setGyroSlope(float x, float y, float z);
	virtual void  setMagnetOffset(float x, float y, float z);
	virtual void  setMagnetSlope(float x, float y, float z);


New: changing the full scale sensitivity of the sensors.
The functions modify the FS (full scale) registers of the LSM9DS1 chip changing sensitivity at the expence of range.
Changing this setting does not change the x,y,z output of the read functions, but assigns just more or less bits
to the sensor measurement. 
    virtual int   setAccelFS(uint8_t range); // 0: ±2g ; 1: ±24g ; 2: ±4g ; 3: ±8g
    virtual int setGyroFS(int8_t range); // 0= ±245 dps; 1= ±500 dps; 2= ±1000 dps; 3= ±2000 dps
    virtual int setMagnetFS(int8_t range); // 0=400.0; 1=800.0; 2=1200.0 , 3=1600.0  (µT)

*note* According to the data sheet gyroscope setting 2 = 1000 dps should not be available. For some reason it worked 
  like a charm on my BLE Sense board, so I added the possibility.

New functions return the Full Scale setting of the corresponding DOF as set with the corresponding set...FS functions
    virtual float getAccelFS(); // Full Scale setting (output = 2.0,  24.0 , 4.0 , 8.0)  
    virtual float getGyroFS();   // output = 245.0,  500.0 , 1000.0, 2000.0) 
    virtual float getMagnetFS(); // output 400.0 ; 800.0 ; 1200.0 , 1600.0      

Note: According to the datasheet setAccelFS(1) should correspoond to 16g. Measurement showed that it is actually 24g
    but that the sensor maxes out at 20g. Since the actual value is used in a calculation the value of 24g is returned
	by getAccelFS for that setting.
	
The band with routines were added as an attempt to dampen a nasty spike that existed in the Gyro signal. It had no
observable effect. The methods are provided as is. 	
    virtual float setAccelBW(uint8_t range); //0,1,2,3 Override autoBandwidth setting see doc.table 67
    virtual float getAccelBW();  //Bandwidth setting 0,1,2,3  see documentation table 67
    virtual int   setGyroBW(uint8_t range);  //Bandwidth setting 0,1,2,3  see documentation table 46 and 47
    virtual float getGyroBW();  //Bandwidth setting 0,1,2,3  see documentation table 46 and 47
	
	
	

*****************************************************************************************************************************
**********************************       Derivation of  linear correction            ****************************************
*****************************************************************************************************************************

Sorry for the very formal derivation below. It suits verifiability but probably only my own purpose :)
It took a lot of puzzling to get it right. Don't read it if you don't want to. :(

*******************************************             Offset            ***************************************************

Assuming good linearity of the transducer, we can model the data output of the chip as

            Data = (32768 / IFS) *( A*Q + B)												(1)

Data = the measured value showing up on the chip registers
Q = the actual physical quantity we are trying to measure in any of the 9 DOF
IFS = the chip Internal Fullscale Setting.  
A,B unknown constants representing chip instance differences

Since the chip outputs dimensionless bits and bytes only, the dimensions of IFS and B must equal the dimension of Q.  

The challenge is to get rid of the unknown constants A and B and translate them into measurable quantities produced by the 
library's Read methods. Since a good calibrated Read should produce a number equal to the actual physical quantity 
we can state  

			Read = Q	
			
Further, we do not want to recalibrate when we change the Full Scale setting or the Unit.

Define
PFS = the in-program Full Scale function counteracting the IFS so that the output result remains unchanged. 
      (dimensionless, but its value corresponds to that of the chosen default unit) 
Slope = in-program correction factor for the sensor sensitivity
Offset = in-program correction factor for the sensor zero point offset

The output of the read methods is (see above)

		Read 	=	Unit * Slope * (PFS / 32786 * Data - Offset )							(2)
			 
Substitute eq(1)

		Read	=	Unit * slope * (PFS / IFS * 32786/32786 * ( A*Q + B ) - Offset )
	
		Read	=	Unit * slope * (PFS / IFS * ( A*Q + B ) - Offset )						(3)
	
In case of the calibrated zero point measurement   Q = Read = 0   
Substitute  in eq.(3) we get

		0	=	Unit * slope * (PFS / IFS * ( A*0 + B ) - Offset )	

		Unit * slope *	Offset  =	Unit * slope * PFS / IFS * B 							(4)
		
For an uncalibrated measurement of a zero point (ZP) filling in  Offset=0 and Q=0 in eq(3) we get 
	
		Read_uncalibrated_ZP  =  Unit * slope * PFS / IFS * B								(5)

The righthand terms in equation (4) and (5) are identical so it follows that 

		Offset  =  read_uncalibrated_ZP / (Unit * slope)									(6)

This defines how we should do the calibration measurements. In the library the methods called set...Offset 
use of eq(6) to assign the uncalibrated zero-point read values to the corresponding program parameters.
Eq.(6) suggests that Offset depends on Unit and Slope, but this is not the case as the Read method scales 
with the same value. To proof this we write eg(4) in a different form

		Offset	=  (Unit * Slope) / (Unit * Slope) * PFS/IFS *  B   						(7)
  
Further	since the dimensionless PFS counteracts IFS in size 
and the dimension of IFS equals that of Q, the ratio PFS/IFS is 1/Unit.
or in other words  

		Unit * PFS /IFS = 1																	(8)
			
eq(7) reduces to  

		Offset  = B	/ Unit	= B_dimensionless																(9)

So Offset equals the dimensionless form of B and does not depend on any other parameter.


********************************************             Slope            ***************************************************

In order to calibrate the Slope at least two measurements (say 1 and 2) must be done where the measured quantity Q 
has a known difference. The calibrated Read equals Q so

		Q_1 - Q_2 	= Read_1 - Read_2 = known value											(10)

Substitute eq(2)

		Q_1 - Q_2	= Unit*Slope*(PFS/32786*Data_1 - Offset) - Unit*Slope*(PFS/32786*Data_2 - Offset)

					= Unit*Slope*(PFS/32786*(Data_1 - Data_2)								(11)
						
So the difference between the measurements gets rid of the Offset
For an uncalibrated measurement we must set   Slope = 1. 
Eq(2) becomes 

		Read_uncalibrated  =  Unit * (PFS / 32786 * Data - Offset )

In eq(11) we get

		Q_1 -Q_2  = Slope * ( Read_uncalibrated_1 - Read_uncalibrated_2)

		Slope 	=	(( Read_uncalibrated_1 - Read_uncalibrated_2) / (Q_1 -Q_2)				(12)

So in order to measure the slope we must first set the Slope parameter to 1. 

In order to prove that Slope is independent of all the other program parameters we substitute eq(1) in eq(11)

		Q_1 - Q_2	= Unit*Slope*(PFS/32786)*(32786/IFS)*(A*Q1 +B - A*Q2 -B))
						
	   (Q_1 - Q_2)	= Slope*  Unit*(PFS/IFS)* A *(Q1 - Q2)									(12)

and with eq(8) 			

		(	1	)	= Slope *      1        * A *(   1   )
			
			Slope 	= 1 / A
 
So Slope only depends on A, the (in)sensitivity of the sensor. The proportionality is inverse since Slope is the
compensation factor for the chip's insensitivity

QED
