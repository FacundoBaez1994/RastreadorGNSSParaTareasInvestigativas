//=====[#include guards - begin]===============================================

#ifndef _ACQUIRING_DATA_H_
#define _ACQUIRING_DATA_H_

//==================[Libraries]===============================================

#include "Non_Blocking_Delay.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "IMUState.h"
#include "Debugger.h"
#include "MPU9250RegisterMap.h"
#include "Non_Blocking_Delay.h"
#include "QuaternionFilter.h"
#include <string.h>

//=====[Declaration of public data types]======================================
class IMU; //debido a declaracion adelantada


//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class acquiringData : public IMUState {
public:
//=====[Declaration of public methods]=========================================
    acquiringData (IMU * imu);
    acquiringData (IMU * imu,  uint8_t mpuI2CAddr);
    virtual ~acquiringData ();
    virtual void initialize(I2CInterfaceHandler * interface, NonBlockingDelay * refresh);
    virtual bool calibrate(I2CInterfaceHandler * interface, NonBlockingDelay * refresh);
    virtual bool obtainInertialData(I2CInterfaceHandler * interface, char * newInertialData);
    virtual void obtainTemperature(I2CInterfaceHandler * interface, float * newTemperature);
private:
//=====[Declaration of privates methods]=========================================
    void update_accel_gyro (I2CInterfaceHandler * interface);
    void read_accel_gyro (int16_t* destination, I2CInterfaceHandler * interface);
    void update_mag(I2CInterfaceHandler * interface);
    bool read_mag(int16_t* destination, I2CInterfaceHandler * interface);
    int16_t read_temperature_data (I2CInterfaceHandler * interface);
    bool update_rpy(float qw, float qx, float qy, float qz);
    void promAcumulatedMeasurement ();
    void clearAcumulatedMeasurement ();

    void print_roll_pitch_yaw();

//=====[Declaration of privates atributes]=========================================;

    IMU * imu;
    bool b_verbose;
    uint8_t mpuI2CAddr;



    float a[3] {0.f, 0.f, 0.f};
    float g[3] {0.f, 0.f, 0.f};
    float m[3] {0.f, 0.f, 0.f};
    float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};  // vector to hold quaternion

    // Temperature
    int16_t temperature_count {0};  // temperature raw count output
    float temperature {0.f};        // Stores the real internal chip temperature in degrees Celsius

    float acc_resolution {0.f};                // scale resolutions per LSB for the sensors
    float gyro_resolution {0.f};               // scale resolutions per LSB for the sensors
    float mag_resolution {0.f};                // scale resolutions per LSB for the sensors


    float mag_bias_factory[3] {0., 0., 0.};
    float mag_bias[3] {0., 0., 0.};  // mag calibration value in MAG_OUTPUT_BITS: 16BITS
    float mag_scale[3] {1., 1., 1.};


    QuaternionFilter * quat_filter;
    size_t n_filter_iter {1};
    bool b_ahrs {true};

    float rpy[3] {0.f, 0.f, 0.f};  // yaw // roll // pitch
    float lin_acc[3] {0.f, 0.f, 0.f};  // linear acceleration (acceleration with gravity component subtracted)

    ///// samples
    int samplesCounter = 0; 
    float aAccumlated [3] {0.f, 0.f, 0.f};
    float gAccumlated [3] {0.f, 0.f, 0.f};
    float mAccumlated [3] {0.f, 0.f, 0.f};
    float anglesAccumlated [3] {0.f, 0.f, 0.f};
    float temperatureAccumlated  {0.f}; 

};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //    _ACQUIRING_DATA_H_