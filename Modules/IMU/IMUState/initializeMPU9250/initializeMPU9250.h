//=====[#include guards - begin]===============================================

#ifndef _INITIALIZE_MPU9250_H_
#define _INITIALIZE_MPU9250_H_

//==================[Libraries]===============================================

#include "Non_Blocking_Delay.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "IMUState.h"
#include "Debugger.h"
#include "MPU9250RegisterMap.h"
#include "Non_Blocking_Delay.h"
#include "initializeAK8963.h"

//=====[Declaration of public data types]======================================
class IMU; //debido a declaracion adelantada
enum class ACCEL_DLPF_CFG  : uint8_t;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class initializeMPU9250 : public IMUState {
public:
//=====[Declaration of public methods]=========================================
    initializeMPU9250 (IMU * imu);
    virtual ~initializeMPU9250 ();
    virtual void initialize(I2CInterfaceHandler * interface, NonBlockingDelay * refresh);
    virtual bool calibrate (I2CInterfaceHandler * interface, NonBlockingDelay * refresh);
    virtual bool obtainInertialData(I2CInterfaceHandler * interface, char * newInertialData);
    virtual void obtainTemperature(I2CInterfaceHandler * interface, float * newTemperature);
private:
//=====[Declaration of privates methods]=========================================
    bool isConnectedMPU9250(I2CInterfaceHandler * interface);
    bool initMPU9250(I2CInterfaceHandler * interface, NonBlockingDelay * refresh);

//=====[Declaration of privates atributes]=========================================
    float acc_resolution {0.f};                // scale resolutions per LSB for the sensors
    float gyro_resolution {0.f};               // scale resolutions per LSB for the sensors
    float mag_resolution {0.f};                // scale resolutions per LSB for the sensors
    uint8_t mpu_config;
    uint8_t sample_rate;
    uint8_t gyro_fchoice;
    ACCEL_DLPF_CFG accel_dlpf_cfg;
    uint8_t accel_fchoice;

    IMU * imu;
    bool b_verbose;
    uint8_t mpuI2CAddr;
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _INITIALIZE_MPU9250_H_