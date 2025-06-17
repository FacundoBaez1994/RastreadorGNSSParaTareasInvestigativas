//=====[Libraries]=============================================================

#include "Slepping.h"
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
Slepping::Slepping (Tracker * tracker) {
    this->tracker = tracker;
}

/** 
* @brief
* 
* @param 
*/
Slepping::~Slepping () {
    this->tracker = NULL;
}

void Slepping::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

void Slepping::awake (CellularModule * cellularTransceiver, NonBlockingDelay * latency ) {
    bool timeToWakeUp = false;

    if (latency->read()) {
        timeToWakeUp = true;
    }
    if (timeToWakeUp == true) {
        if (cellularTransceiver->turnOn () ) {
            timeToWakeUp = false;
            this->tracker->changeState  (new SensingBatteryStatus (this->tracker));
            return;
        }
    }
    return;
 }






//=====[Implementations of private methods]==================================