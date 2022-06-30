# ArduinoLSM9DS1 library

The ArduinoLSM9DS1 library allows you to use the inertial measurement unit (IMU) available on the Arduino&reg; Nano 33 BLE board. The IMU is a [LSM9DS1](https://www.st.com/resource/en/datasheet/lsm9ds1.pdf), it is a 3-axis accelerometer, 3-axis gyroscope, and 3-axis magnetometer. The IMU is connected to the Nano 33 BLE board's microcontroller through I2C. The values returned are signed floats.

To use this library:

```
#include <Arduino_LSM9DS1.h>
```

The ArduinoLSM9DS1 library takes care of the sensor initialization and sets its values as follows:

- Accelerometer range is set at ±4 g with a resolution of 0.122 mg.
- Gyroscope range is set at ±2000 dps with a resolution of 70 mdps.
- Magnetometer range is set at ±400 uT with a resolution of 0.014 uT.
- Accelerometer and gyrospcope output data rate is fixed at 119 Hz.
- Magnetometer output data rate is fixed at 20 Hz.
