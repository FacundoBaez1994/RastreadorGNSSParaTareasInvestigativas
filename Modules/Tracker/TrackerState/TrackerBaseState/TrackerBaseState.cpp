//=====[Libraries]=============================================================

#include "TrackerBaseState.h"
//#include "Tracker.h" //debido a declaracion adelantada
//#include "Debugger.h" // due to global usbUart
//#include "SensingBatteryStatus.h"

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



void TrackerBaseState::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    return;
 }

void TrackerBaseState::obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata) {
    return;
}

 void TrackerBaseState::connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation) {
    return; 
}


void TrackerBaseState::obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors ) {
    return;
}


void TrackerBaseState::formatMessage (char * formattedMessage, const CellInformation* aCellInfo,
    const GNSSData* GNSSInfo, const std::vector<CellInformation*> &neighborsCellInformation,
    const IMUData_t * imuData, const BatteryData  * batteryStatus) {
    return;
}

void TrackerBaseState::exchangeMessages (CellularModule * cellularTransceiver,
    char * message, TcpSocket * socketTargetted, char * receivedMessage ){

    return;
}
    // agregar LoRa // exchageMessages (Lora * LoRaModule);
void TrackerBaseState::goToSleep (CellularModule * cellularTransceiver ) {
    return;
}

void TrackerBaseState::awake (CellularModule * cellularTransceiver, NonBlockingDelay * latency ) {
    return;
 }

void TrackerBaseState::calibrateIMU (IMUManager * inertialSensor) {
    return;
}

void TrackerBaseState::obtainInertialMeasures (IMUManager * inertialSensor,  IMUData_t * inertialData, std::vector<IMUData_t*> &IMUDataSamples) {
    return;
}

void TrackerBaseState::saveMessage (EEPROMManager * memory, char * message) {
    return;
}

void TrackerBaseState::loadMessage (EEPROMManager * memory, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
    IMUData_t * imuData,  BatteryData  * batteryStatus) {
    return;
}

//=====[Implementations of private methods]==================================