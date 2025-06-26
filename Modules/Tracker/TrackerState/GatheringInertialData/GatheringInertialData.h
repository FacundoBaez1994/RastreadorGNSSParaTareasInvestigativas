//=====[#include guards - begin]===============================================

#ifndef _GATHERING_INERTIAL_DATA_H_
#define _GATHERING_INERTIAL_DATA_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "TrackerBaseState.h"
#include "Tracker.h"
#include "Non_Blocking_Delay.h"
#include "timeStampTools.h"

//=====[Declaration of public data types]======================================
class Tracker; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class GatheringInertialData: public TrackerBaseState {
public:
//=====[Declaration of public methods]=========================================
    GatheringInertialData (Tracker * tracker, trackerStatus_t trackerStatus);
    virtual ~GatheringInertialData ();
    virtual void obtainInertialMeasures (IMUManager * inertialSensor, IMUData_t * imuData, std::vector<IMUData_t*> &IMUDataSamples);
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);
private:
    Tracker * tracker;
    NonBlockingDelay * timeBeetwenSamples;
    trackerStatus_t currentStatus;
    IMUData_t * auxiliarIMUData;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _GATHERING_INERTIAL_DATA_H_