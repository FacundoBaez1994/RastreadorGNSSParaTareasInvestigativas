//=====[Libraries]=============================================================

#include "GoingToSleep.h"
#include "Slepping.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "SensingBatteryStatus.h"

//=====[Declaration of private defines]========================================

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
GoingToSleep::GoingToSleep (Tracker * tracker) {
    this->tracker = tracker;
}

/** 
* @brief
* 
* @param 
*/
GoingToSleep::~GoingToSleep () {
    this->tracker = nullptr;
}

void GoingToSleep::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

void GoingToSleep::goToSleep (CellularModule * cellularTransceiver ) {
    OperationMode_t operationMode = this->tracker->getOperationMode();
    if (operationMode  == PURSUIT_OPERATION_MODE) {
        this->tracker->changeState  (new SensingBatteryStatus (this->tracker));
        return;
    }
    if (cellularTransceiver->turnOff()) {
        this->tracker->changeState  (new Slepping (this->tracker));
        return;
    }
    return;
}





//=====[Implementations of private methods]==================================