//=====[Libraries]=============================================================

#include "GoingToSleep.h"
#include "Slepping.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

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
    this->tracker = NULL;
}

void GoingToSleep::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

    // agregar LoRa // exchageMessages (Lora * LoRaModule);
void GoingToSleep::goToSleep (CellularModule * cellularTransceiver ) {
    if (cellularTransceiver->goToSleep()) {
        this->tracker->changeState  (new Slepping (this->tracker));
        return;
    }
    return;
}





//=====[Implementations of private methods]==================================