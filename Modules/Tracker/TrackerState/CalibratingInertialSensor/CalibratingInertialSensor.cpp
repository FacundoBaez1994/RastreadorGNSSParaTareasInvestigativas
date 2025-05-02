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
    this->tracker = NULL;
}


void CalibratingInertialSensor::calibrateIMU (IMU * inertialSensor) {

     this->tracker->changeState(new SensingBatteryStatus (this->tracker));
     
    /*
    static bool firstcalibrationOK = false;
    char inertialData [100];
    float temp;

    if (firstcalibrationOK == false) {
        if (inertialSensor->calibrate() == true) {
            firstcalibrationOK = true;
            
        }
    }
    if (firstcalibrationOK == true ) { 
        // primera lectura de prueba
        if (inertialSensor->obtainInertialMeasures(inertialData, &temp) == true) {
            this->tracker->changeState(new SensingBatteryStatus (this->tracker));
            firstcalibrationOK = false;
            return;
        }
    }
    return;
    */
}




//=====[Implementations of private methods]==================================