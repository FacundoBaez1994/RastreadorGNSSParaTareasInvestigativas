//=====[Libraries]=============================================================

#include "ConnectingToMobileNetwork.h"
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
/** 
* @brief attachs the callback function to the ticker
*/


//=====[Implementations of public methods]===================================


/** 
* @brief
* 
* @param 
*/
ConnectingToMobileNetwork::ConnectingToMobileNetwork (Tracker * tracker, trackerStatus_t trackerStatus) {
    this->currentStatus = trackerStatus;
    this->tracker = tracker;
}

/** 
* @brief
* 
* @param 
*/
ConnectingToMobileNetwork::~ConnectingToMobileNetwork ( ) {
    this->tracker = NULL;
}

void ConnectingToMobileNetwork::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

void ConnectingToMobileNetwork::obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata) {

    return;
}
void ConnectingToMobileNetwork::obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors ) { 
    return; 
}


 void ConnectingToMobileNetwork::connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation) {
    static CellularConnectionStatus_t currentConnectionStatus;
    char logMessage [40];
    
    cellularTransceiver->enableConnection();
    currentConnectionStatus = cellularTransceiver->connectToMobileNetwork (currentCellInformation);
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
        // Go to sleep
        // USE IMU PLUS MEMORY INSTED 
        this->tracker->changeState  (new GoingToSleep (this->tracker));
        return;
    }


    return; 
}

void ConnectingToMobileNetwork::formatMessage (char * formattedMessage, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
     char * inertialData, BatteryData  * batteryStatus) {

    return;
}

void ConnectingToMobileNetwork::exchangeMessages (CellularModule * cellularTransceiver,
    char * message, TcpSocket * socketTargetted, char * receivedMessage ){
    return;
}
    // agregar LoRa // exchageMessages (Lora * LoRaModule);
void ConnectingToMobileNetwork::goToSleep (CellularModule * cellularTransceiver ) {
    return;
}

void ConnectingToMobileNetwork::awake (CellularModule * cellularTransceiver, 
NonBlockingDelay * latency ) {
    return;
}

void ConnectingToMobileNetwork::calibrateIMU (IMU * inertialSensor) {
    return;
}

void ConnectingToMobileNetwork::obtainInertialMeasures (IMU * inertialSensor,
 char * dataObtain, float * temperatureObtain) {
    return;
}




