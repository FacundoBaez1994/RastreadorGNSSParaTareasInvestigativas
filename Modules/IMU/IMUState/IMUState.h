//=====[#include guards - begin]===============================================

#ifndef _IMU_STATE_H_
#define _IMU_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "I2CInterfaceHandler.h"


//=====[Declaration of public data types]======================================


//=====[Declaration of public classes]=========================================
/*
 *  Interface - State desing pattern
 * 
 */
class IMUState {
public:
//=====[Declaration of public methods]=========================================
    virtual void initialize(I2CInterfaceHandler * interface, NonBlockingDelay * refresh);
    virtual void calibrate (I2CInterfaceHandler * interface, NonBlockingDelay * refresh);
    virtual bool obtainInertialData(I2CInterfaceHandler * interface, char * newInertialData);
    virtual void obtainTemperature(I2CInterfaceHandler * interface, float * newTemperature);

    

//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _TRACKER_STATE_H_