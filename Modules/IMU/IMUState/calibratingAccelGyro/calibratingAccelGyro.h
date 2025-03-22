//=====[#include guards - begin]===============================================

#ifndef _CALIBRATING_ACCEL_GYRO_H_
#define _CALIBRATING_ACCEL_GYRO_H_

//==================[Libraries]===============================================

#include "Non_Blocking_Delay.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "IMUState.h"
#include "Debugger.h"
#include "MPU9250RegisterMap.h"
#include "Non_Blocking_Delay.h"
#include "calibratingMag.h"
//=====[Declaration of public data types]======================================
class IMU; //debido a declaracion adelantada
//enum class ACCEL_DLPF_CFG  : uint8_t;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class calibratingAccelGyro : public IMUState {
public:
//=====[Declaration of public methods]=========================================
    calibratingAccelGyro (IMU * imu);
     calibratingAccelGyro (IMU * imu,  uint8_t mpuI2CAddr);
    virtual ~calibratingAccelGyro ();
    virtual void initialize(I2CInterfaceHandler * interface, NonBlockingDelay * refresh);
    virtual void calibrate(I2CInterfaceHandler * interface, NonBlockingDelay * refresh);
    virtual bool obtainInertialData(I2CInterfaceHandler * interface, char * newInertialData);
    virtual void obtainTemperature(I2CInterfaceHandler * interface, float * newTemperature);
private:
//=====[Declaration of privates methods]=========================================
    //bool isConnectedMPU9250(I2CInterfaceHandler * interface);
     //bool initMPU9250(I2CInterfaceHandler * interface, NonBlockingDelay * refresh);
    bool set_acc_gyro_to_calibration(I2CInterfaceHandler * interface, 
    NonBlockingDelay * refresh);
    void collect_acc_gyro_data_to(I2CInterfaceHandler * interface);
    void write_accel_offset(I2CInterfaceHandler * interface);
    void write_gyro_offset(I2CInterfaceHandler * interface);
    bool initMPU9250(I2CInterfaceHandler * interface, NonBlockingDelay * refresh);

//=====[Declaration of privates atributes]=========================================;
    float a_bias[3] {0., 0., 0.};   // acc calibration value in ACCEL_FS_SEL: 2g
    float g_bias[3] {0., 0., 0.};  // gyro calibration value in GYRO_FS_SEL: 250dps

    IMU * imu;
    bool b_verbose;
    uint8_t mpuI2CAddr;
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _CALIBRATING_ACCEL_GYRO_H_