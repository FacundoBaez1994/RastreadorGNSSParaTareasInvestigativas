//=====[#include guards - begin]===============================================

#ifndef _CALIBRATING_MAG_H_
#define _CALIBRATING_MAG_H_

//==================[Libraries]===============================================

#include "Non_Blocking_Delay.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "IMUState.h"
#include "Debugger.h"
#include "MPU9250RegisterMap.h"
#include "Non_Blocking_Delay.h"
#include "acquiringData.h"

//=====[Declaration of public data types]======================================
class IMU; //debido a declaracion adelantada
//enum class ACCEL_DLPF_CFG  : uint8_t;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class calibratingMag : public IMUState {
public:
//=====[Declaration of public methods]=========================================
    calibratingMag (IMU * imu);
    calibratingMag (IMU * imu,  uint8_t mpuI2CAddr);
    virtual ~calibratingMag  ();
    virtual void initialize(I2CInterfaceHandler * interface, NonBlockingDelay * refresh);
    virtual void calibrate(I2CInterfaceHandler * interface, NonBlockingDelay * refresh);
    virtual bool obtainInertialData(I2CInterfaceHandler * interface, char * newInertialData);
    virtual void obtainTemperature(I2CInterfaceHandler * interface, float * newTemperature);
private:
//=====[Declaration of privates methods]=========================================
    bool initAK8963(I2CInterfaceHandler * interface, NonBlockingDelay * refresh);
    bool collect_mag_data_to(I2CInterfaceHandler * interface, NonBlockingDelay * refresh);
    bool read_mag(int16_t* destination, I2CInterfaceHandler * interface);

//=====[Declaration of privates atributes]=========================================
    float mag_bias[3] {0., 0., 0.};  // mag calibration value in MAG_OUTPUT_BITS: 16BITS
    float mag_scale[3] {1., 1., 1.};
    float mag_bias_factory[3] {0., 0., 0.};

    IMU * imu;
    bool b_verbose;
    uint8_t mpuI2CAddr;
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _CALIBRATING_ACCEL_GYRO_H_