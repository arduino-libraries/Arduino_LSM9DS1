/*
 * LSM9DS1_Registers.h
 *
 * Environment specifics:
 *     IDE: Arduino 1.8.10
 *     Platform: Arduino Nano 33 BLE
 */

#ifndef LSM9DS1_Register_H
#define LSM9DS1_Register_H

// LSM9DS1 Accel/Gyro (XL/G) Registers  ADDR     Default
#define ACT_THS                       0x04 // 00000000
#define ACT_DUR                       0x05 // 00000000
#define INT_GEN_CFG_XL                0x06 // 00000000
#define INT_GEN_THS_X_XL              0x07 // 00000000
#define INT_GEN_THS_Y_XL              0x08 // 00000000
#define INT_GEN_THS_Z_XL              0x09 // 00000000
#define INT_GEN_DUR_XL                0x0A // 00000000
#define REGERENCE_G                   0x0B // 00000000
#define INT1_CTRL                     0x0C // 00000000
#define INT2_CTRL                     0x0D // 00000000
#define WHO_AM_I                      0x0F // 01101000
#define CTRL_REG1_G                   0x10 // 00000000
#define CTRL_REG2_G                   0x11 // 00000000
#define CTRL_REG3_G                   0x12 // 00000000
#define ORIENT_CFG_G                  0x13 // 00000000
#define INT_GEN_SRC_G                 0x14 // output
#define OUT_TEMP_L                    0x15 // output
#define OUT_TEMP_H                    0x16 // output
#define STATUS_REG_0                  0x17 // output
#define OUT_X_L_G                     0x18 // output
#define OUT_X_H_G                     0x19 // output
#define OUT_Y_L_G                     0x1A // output
#define OUT_Y_H_G                     0x1B // output
#define OUT_Z_L_G                     0x1C // output
#define OUT_Z_H_G                     0x1D // output
#define CTRL_REG4                     0x1E // 00111000
#define CTRL_REG5_XL                  0x1F // 00111000
#define CTRL_REG6_XL                  0x20 // 00000000
#define CTRL_REG7_XL                  0x21 // 00000000
#define CTRL_REG8                     0x22 // 00000100
#define CTRL_REG9                     0x23 // 00000000
#define CTRL_REG10                    0x24 // 00000000
#define INT_GEN_SRC_XL                0x26 // output
#define STATUS_REG_1                  0x27 // output
#define OUT_X_L_XL                    0x28 // output
#define OUT_X_H_XL                    0x29 // output
#define OUT_Y_L_XL                    0x2A // output
#define OUT_Y_H_XL                    0x2B // output
#define OUT_Z_L_XL                    0x2C // output
#define OUT_Z_H_XL                    0x2D // output
#define FIFO_CTRL                     0x2E // 00000000
#define FIFO_SRC                      0x2F // output
#define INT_GEN_CFG_G                 0x30 // 00000000
#define INT_GEN_THS_XH_G              0x31 // 00000000
#define INT_GEN_THS_XL_G              0x32 // 00000000
#define INT_GEN_THS_YH_G              0x33 // 00000000
#define INT_GEN_THS_YL_G              0x34 // 00000000
#define INT_GEN_THS_ZH_G              0x35 // 00000000
#define INT_GEN_THS_ZL_G              0x36 // 00000000
#define INT_GEN_DUR_G                 0x37 // 00000000

// LSM9DS1 Magneto Registers          ADDR    Default
#define OFFSET_X_REG_L_M              0x05 // 00000000
#define OFFSET_X_REG_H_M              0x06 // 00000000
#define OFFSET_Y_REG_L_M              0x07 // 00000000
#define OFFSET_Y_REG_H_M              0x08 // 00000000
#define OFFSET_Z_REG_L_M              0x09 // 00000000
#define OFFSET_Z_REG_H_M              0x0A // 00000000
#define CTRL_REG1_M                   0x20 // 00010000
#define CTRL_REG2_M                   0x21 // 00000000
#define CTRL_REG3_M                   0x22 // 00000011
#define CTRL_REG4_M                   0x23 // 00000000
#define CTRL_REG5_M                   0x24 // 00000000
#define STATUS_REG_M                  0x27 // output
#define OUT_X_L_M                     0x28 // output
#define OUT_X_H_M                     0x29 // output
#define OUT_Y_L_M                     0x2A // output
#define OUT_Y_H_M                     0x2B // output
#define OUT_Z_L_M                     0x2C // output
#define OUT_Z_H_M                     0x2D // output
#define INT_CFG_M                     0x30 // 00001000
#define INT_SRC_M                     0x31 // 00000000
#define INT_THS_L_M                   0x32 // 00000000
#define INT_THS_H_M                   0x33 // 00000000

// LSM9DS1 Accelerometer/Gyroscope & Magneto Address
#define LSM9DS1_ADDRESS               0x6B
#define LSM9DS1_ADDRESS_M             0x1E

#endif
