//=====[#include guards - begin]===============================================

#ifndef _INITIALIZE_AK8963_H_
#define _INITIALIZE_AK8963_H_

//==================[Libraries]===============================================
#include "Non_Blocking_Delay.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "IMUState.h"
#include "Debugger.h"
#include "MPU9250RegisterMap.h"
#include "Non_Blocking_Delay.h"
#include "calibratingAccelGyro.h"

//=====[Declaration of public data types]======================================
class IMU; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class initializeAK8963 : public IMUState {
public:
//=====[Declaration of public methods]=========================================
    initializeAK8963 (IMU * imu);
    initializeAK8963 (IMU * imu, uint8_t mpuI2CAddr);
    virtual ~initializeAK8963 ();
    virtual void initialize(I2CInterfaceHandler * interface, NonBlockingDelay * refresh);
    virtual bool calibrate(I2CInterfaceHandler * interface, NonBlockingDelay * refresh);
    virtual bool obtainInertialData(I2CInterfaceHandler * interface, char * newInertialData);
    virtual void obtainTemperature(I2CInterfaceHandler * interface, float * newTemperature);
private:
//=====[Declaration of privates methods]=========================================
    bool isConnectedAK8963(I2CInterfaceHandler * interface);
    bool initAK8963(I2CInterfaceHandler * interface, NonBlockingDelay * refresh);

//=====[Declaration of privates atributes]=========================================
    float mag_bias_factory[3] {0., 0., 0.};


    IMU * imu;
    bool b_verbose;
    uint8_t mpuI2CAddr;
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _INITIALIZE_AK8963_H_