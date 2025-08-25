//=====[Libraries]=============================================================
#include "ConnectingToMobileNetwork.h"
#include "FormattingMessage.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "GatheringCellInformation.h"
#include "GoingToSleep.h"
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

//=====[Implementations of public methods]===================================
ConnectingToMobileNetwork::ConnectingToMobileNetwork (Tracker * tracker, trackerStatus_t trackerStatus) {
    this->currentStatus = trackerStatus;
    this->tracker = tracker;
}

ConnectingToMobileNetwork::~ConnectingToMobileNetwork ( ) {
    this->tracker = nullptr;
}

void ConnectingToMobileNetwork::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

 void ConnectingToMobileNetwork::connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation) {
    static CellularConnectionStatus_t currentConnectionStatus;
    char logMessage [40];
    
    cellularTransceiver->enableConnection();
    currentConnectionStatus = cellularTransceiver->connectToMobileNetwork (currentCellInformation);
    
    /// test only
   //currentConnectionStatus = CELLULAR_CONNECTION_STATUS_UNAVAIBLE_TO_ATTACH_TO_PACKET_SERVICE;
   // this->currentStatus = TRACKER_STATUS_GNSS_OBTAIN;
    /// test only

    if (currentConnectionStatus == CELLULAR_CONNECTION_STATUS_CONNECTED_TO_NETWORK){
        if (this->currentStatus == TRACKER_STATUS_GNSS_OBTAIN) {
            this->tracker->changeState (new GatheringInertialData (this->tracker, TRACKER_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK));
            return;
        } else {
            this->tracker->changeState  (new GatheringCellInformation (this->tracker));
            return;
        }
    } else if (currentConnectionStatus != CELLULAR_CONNECTION_STATUS_UNAVAIBLE && 
        currentConnectionStatus != CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT) {
        char StringToSendUSB [50] = "Access to mobile network UNAVAILABLE!!!!";
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only}
        
        if (this->currentStatus == TRACKER_STATUS_GNSS_OBTAIN) {
            // NO MN, TRY TO SEND GNSS DATA THROUGH LORA after gather inertial data
            this->tracker->changeState  (new GatheringInertialData (this->tracker, TRACKER_STATUS_GNSS_OBTAIN_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_TRYING_LORA));
            return;
        } else {
             // NO MN AND NO GNSS, TRY TO SEND SOME DATA THROUGH LORA inertial data
            this->tracker->changeState  (new GatheringInertialData(this->tracker, TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_TRYING_LORA));
            return;
        }
        return;
    }
    return; 
}





