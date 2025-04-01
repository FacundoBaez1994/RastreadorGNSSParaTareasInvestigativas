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

void Slepping::obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata) {
    return;
}

 void Slepping::connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation) {
    return; 
}


void Slepping::obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors ) {
    return;
}
    // IMU Method 1
    // IMU Methord 2

void Slepping::formatMessage (char * formattedMessage, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
    BatteryData  * batteryStatus) {
    return;
}

void Slepping::exchangeMessages (CellularModule * cellularTransceiver,
    char * message, TcpSocket * socketTargetted, char * receivedMessage ){

    return;
}
    // agregar LoRa // exchageMessages (Lora * LoRaModule);
void Slepping::goToSleep (CellularModule * cellularTransceiver ) {
    return;
}

void Slepping::awake (CellularModule * cellularTransceiver, NonBlockingDelay * latency ) {
    if (latency->read()) {
        cellularTransceiver->awake();
        this->tracker->changeState  (new SensingBatteryStatus (this->tracker));
    }
    return;
 }

void Slepping::calibrateIMU (IMU * inertialSensor) {
    return;
}

void Slepping::obtainInertialMeasures (IMU * inertialSensor, char * dataObtain, float * temperatureObtain) {
    return;
}





//=====[Implementations of private methods]==================================