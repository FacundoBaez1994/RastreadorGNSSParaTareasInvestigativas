
#ifndef _IMU_MANAGER_H
#define _IMU_MANAGER_H

#include "mbed.h"
#include "Adafruit_I2CDevice.h"
#include "Adafruit_BNO08x.h"
#include "arm_book_lib.h"



//=====[Declaration of public defines]=========================================


//=====[Declaration of public data types]======================================
struct euler_t {
  float yaw;
  float pitch;
  float roll;
};

struct acceleration_t {
  float ax;
  float ay;
  float az;
};

struct IMUData_t {
  euler_t angles;
  acceleration_t acceleration;
  uint8_t status;
};


//=====[Declaration of public classes]=========================

/*!
 * @brief Clase para manejar el sensor BNO08x
 */
class IMUManager {
public:
    IMUManager();
    ~IMUManager();

    bool setReports(sh2_SensorId_t reportType, long report_interval);

    bool obtainInertialMeasures (IMUData_t * inertialMeasures);
    bool initialize (void);

private:
    inline float sq(float x);
    void quaternionToEuler(float qr, float qi, float qj, float qk, euler_t* ypr, bool degrees);
    void quaternionToEulerRV(sh2_RotationVectorWAcc_t* rotational_vector, euler_t* ypr, bool degrees);
    void quaternionToEulerGI(sh2_GyroIntegratedRV_t* rotational_vector, euler_t* ypr, bool degrees);

    /// prom measures
    void promEulerMeasurement(void);
    void promAccelMeasurement(void);
    void clearAcumulatedMeasurement (void);

    Adafruit_BNO08x  * bno08x;
    I2C * i2c;
    sh2_SensorValue_t sensorValue;
    sh2_SensorId_t reportType;
    long reportIntervalUs;

    euler_t eulerAngles;
    acceleration_t acceleration;

    //// prom measures
    euler_t promEulerAngles;
    acceleration_t promAcceleration;
    int samplesCounterAccel;
    int samplesCounterAngles;
};

//=====[#include guards - end]=================================================
#endif // _IMU_MANAGER_H