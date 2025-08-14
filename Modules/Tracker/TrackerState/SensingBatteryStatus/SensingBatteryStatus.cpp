//=====[Libraries]=============================================================

#include "SensingBatteryStatus.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "GettingGNSSPosition.h"

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
SensingBatteryStatus::SensingBatteryStatus (Tracker * tracker) {
    this->tracker = tracker;
}

/** 
* @brief
* 
* @param 
*/
SensingBatteryStatus::~SensingBatteryStatus ( ) {
    this->tracker = nullptr;
}

void SensingBatteryStatus::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
    if (cellularTransceiver->measureBattery(currentBatteryStatus) == true){
        this->tracker->changeState  (new GettingGNSSPosition (this->tracker));
        return;
    }
    return;
 }


