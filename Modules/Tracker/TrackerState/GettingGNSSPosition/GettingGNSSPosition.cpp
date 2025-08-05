//=====[Libraries]=============================================================

#include "GettingGNSSPosition.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "ConnectingToMobileNetwork.h"
#include "FormattingMessage.h"
#include "GatheringInertialData.h"

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
GettingGNSSPosition::GettingGNSSPosition (Tracker * tracker) {
    this->tracker = tracker;
}

/** 
* @brief
* 
* @param 
*/
GettingGNSSPosition::~GettingGNSSPosition () {
    this->tracker = nullptr;
}

void GettingGNSSPosition::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

void GettingGNSSPosition::obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata) {
   static GNSSState_t GnssCurrentStatus;
   char logMessage [40]; 
   OperationMode_t operationMode = this->tracker->getOperationMode();

    // SIN GNSS
    //this->tracker->changeState  (new ConnectingToMobileNetwork (this->tracker, TRACKER_STATUS_GNSS_UNAVAILABLE));
    //return;

    currentGNSSModule->enableGNSS();
    GnssCurrentStatus = currentGNSSModule->retrivGeopositioning(currentGNSSdata);
    if (GnssCurrentStatus == GNSS_STATE_CONNECTION_OBTAIN ) {
        snprintf(logMessage, sizeof(logMessage), "GNSS OBTAIN!!!!");
        uartUSB.write (logMessage , strlen (logMessage ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        if (operationMode == SILENT_OPERATION_MODE) {
            this->tracker->changeState (new FormattingMessage (this->tracker,TRACKER_STATUS_GNSS_OBTAIN_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_LORA_UNAVAILABLE_SAVING_MESSAGE));
            return;
        }
        this->tracker->changeState  (new ConnectingToMobileNetwork (this->tracker, TRACKER_STATUS_GNSS_OBTAIN));
        return;
    }
    if (GnssCurrentStatus == GNSS_STATE_CONNECTION_UNAVAILABLE ) {
        snprintf(logMessage, sizeof(logMessage), "GNSS UNAVAILABLE!!!!");
        uartUSB.write (logMessage , strlen (logMessage ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only}
        if (operationMode == SILENT_OPERATION_MODE) {
            this->tracker->changeState (new GatheringInertialData (this->tracker,TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_LORA_UNAVAILABLE_GATHERING_INERTIAL_INFO));
            return;
        }
        this->tracker->changeState  (new ConnectingToMobileNetwork (this->tracker, TRACKER_STATUS_GNSS_UNAVAILABLE));
        return;
    }

    return;
}

