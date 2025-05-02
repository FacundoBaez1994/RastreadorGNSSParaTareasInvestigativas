//=====[#include guards - begin]===============================================

#ifndef _CALIBRATING_INERTIAL_SENSOR_H_
#define _CALIBRATING_INERTIAL_SENSOR_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "TrackerBaseState.h"
#include "Tracker.h"

//=====[Declaration of public data types]======================================
class Tracker; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class CalibratingInertialSensor: public TrackerBaseState {
public:
//=====[Declaration of public methods]=========================================
    CalibratingInertialSensor (Tracker * tracker);
    virtual ~CalibratingInertialSensor ();
    virtual void calibrateIMU (IMU * inertialSensor);
private:
    Tracker * tracker;
  

//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CALIBRATING_INERTIAL_SENSOR_H_