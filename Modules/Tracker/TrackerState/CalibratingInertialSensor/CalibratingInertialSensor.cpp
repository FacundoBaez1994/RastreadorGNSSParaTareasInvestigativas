//=====[Libraries]=============================================================

#include "CalibratingInertialSensor.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "SensingBatteryStatus.h"

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============


//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of private methods]===================================
/** 
* @brief attachs the callback function to the ticker
*/


//=====[Implementations of public methods]===================================


/** 
* @brief
* 
* @param 
*/
CalibratingInertialSensor::CalibratingInertialSensor (Tracker * tracker) {
    this->tracker = tracker;
}

/** 
* @brief
* 
* @param 
*/
CalibratingInertialSensor::~CalibratingInertialSensor() {
    this->tracker = nullptr;
}


void CalibratingInertialSensor::calibrateIMU (IMUManager * inertialSensor) {
    if (inertialSensor->initialize () == false) {
        return;
    } else {
        this->tracker->changeState(new SensingBatteryStatus (this->tracker));
        return; 
    }

}




//=====[Implementations of private methods]==================================