//=====[Libraries]=============================================================

#include "GatheringCellInformation.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "GatheringInertialData.h"

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
GatheringCellInformation::GatheringCellInformation (Tracker * tracker) {
    this->tracker = tracker;
}

/** 
* @brief
* 
* @param 
*/
GatheringCellInformation::~GatheringCellInformation () {
    this->tracker = NULL;
}

void GatheringCellInformation::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

void GatheringCellInformation::obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors ) {

    if (cellularTransceiver->retrivNeighborCellsInformation(neighborsCellInformation,
     numberOfNeighbors) == true){
         this->tracker->changeState (new GatheringInertialData (this->tracker, 
         TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK));
         return;
     }
    // Format message for LoRa 
    return; 
}






