//=====[#include guards - begin]===============================================
#ifndef INERTIALSENSOR_H
#define INERTIALSENSOR_H
 
//==================[Libraries]===============================================

#include "mbed.h"
#include "math.h"
#include "Non_Blocking_Delay.h"

//=====[Declaration of public data types]======================================

// Set initial input parameters
enum Ascale {
  AFS_2G = 0,
  AFS_4G,
  AFS_8G,
  AFS_16G
};

enum Gscale {
  GFS_250DPS = 0,
  GFS_500DPS,
  GFS_1000DPS,
  GFS_2000DPS
};

enum Mscale {
  MFS_14BITS = 0, // 0.6 mG per LSB
  MFS_16BITS      // 0.15 mG per LSB
};

// parameters for 6 DoF sensor fusion calculations


//=====[Declaration of public classes]=========================================

class InertialSensor {
    //=====[Declaration of public methods]=========================================
public:
    InertialSensor ();
    virtual ~InertialSensor ();

    // mover a I2C interface handler
    void writeByte(uint8_t address, uint8_t subAddress, uint8_t data);
    char readByte(uint8_t address, uint8_t subAddress);
    void readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest);
       
    
    bool initializeSensors ();
    bool checkCommunicationWithModule();   
    
    // mover a accelgyro
    bool initMPU9250();
    bool calibrateMPU9250();
    bool resetMPU9250();

    //mover a mag
    bool initAK8963();

    void readAccelData();
    void readGyroData();
    void readMagData();
    void readTempData();
    void MadgwickQuaternionUpdate(float deltat);
    void obtainYawPitchRoll( );
    /*
;
    void MPU9250SelfTest(float * destination);
    void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, 
    float gy, float gz, float mx, float my, float mz);
    void MahonyQuaternionUpdate(float ax, float ay, float az, float gx,
      float gy, float gz, float mx, float my, float mz);
    */

 private:
    //=====[Declaration of private methods]=========================================
    void getMres();
    void getGres();
    void getAres();

    //=====[Declaration of private atributtes]=========================================
    I2C * i2cInterface;
    NonBlockingDelay* refreshTime;

    uint8_t Ascale;   // AFS_2G, AFS_4G, AFS_8G, AFS_16G
    uint8_t Gscale;  // GFS_250DPS, GFS_500DPS, GFS_1000DPS, GFS_2000DPS
    uint8_t Mscale;  // MFS_14BITS or MFS_16BITS, 14-bit or 16-bit magnetometer resolution
    uint8_t Mmode;         // Either 8 Hz 0x02) or 100 Hz (0x06) magnetometer data ODR  
    float aRes, gRes, mRes;

    //int16_t accelCount[3] = {0, 0, 0};  // Stores the 16-bit signed accelerometer sensor output
    //int16_t gyroCount[3] = {0, 0, 0};   // Stores the 16-bit signed gyro sensor output
    //int16_t magCount[3] = {0, 0, 0};    // Stores the 16-bit signed magnetometer sensor output
    float magCalibration[3] = {0, 0, 0};  // Factory mag calibration
    float magbias[3] = {0, 0, 0};  // Factory mag bias

    float gyroBias [3] = {0, 0, 0}; // Factory mag calibration and mag bias
    float accelBias [3] = {0, 0, 0}; // Bias corrections for gyro and accelerometer

    float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values 

    float temperature;
   
    //float SelfTest[6];


    float GyroMeasError;     // gyroscope measurement error in rads/s (start at 60 deg/s), then reduce after ~10 s to 3
    float beta;  // compute beta
    float GyroMeasDrift;      // gyroscope measurement drift in rad/s/s (start at 0.0 deg/s/s)
    float zeta;  // compute zeta, the other free parameter in the Madgwick scheme usually set to a small or zero value

    float pitch, yaw, roll;
    float deltat = 0.0f;                 // integration interval for both filter schemes
    int lastUpdate  = 0, firstUpdate = 0, Now = 0;   // used to calculate integration interval                               // used to calculate integration interval
    float q[4] = {1.0f, 0.0f, 0.0f, 0.0f}; ;      // vector to hold quaternion
   // float eInt[3] = {0.0f, 0.0f, 0.0f};  // vector to hold integral error for Mahony method
};

//  INERTIALSENSOR_H
#endif