//=====[Libraries]=============================================================

#include "GatheringInertialData.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
//#include "SensingBatteryStatus.h"
#include "FormattingMessage.h"

#include "initializeMPU9250.h"

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20
#define TIME_BETWEEN_SAMPLES 10000
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
GatheringInertialData::GatheringInertialData (Tracker * tracker, trackerStatus_t trackerStatus) {
    this->tracker = tracker;
    this->currentStatus = trackerStatus;
    this->timeBeetwenSamples = new NonBlockingDelay (TIME_BETWEEN_SAMPLES);
}

/** 
* @brief
* 
* @param 
*/
GatheringInertialData::~GatheringInertialData() {
    this->tracker = NULL;
    delete this->timeBeetwenSamples;
    this->timeBeetwenSamples  = nullptr;
}

void GatheringInertialData::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    //cellularTransceiver->startStopUpdate();
 }

void GatheringInertialData::obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata) {
    return;
}

 void GatheringInertialData::connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation) {
    return; 
}

void GatheringInertialData::obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors ) {
    return;
}

void GatheringInertialData::formatMessage (char * formattedMessage, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
    BatteryData  * batteryStatus) {
    return;
}

void GatheringInertialData::exchangeMessages (CellularModule * cellularTransceiver,
    char * message, TcpSocket * socketTargetted, char * receivedMessage ){

    return;
}
    // agregar LoRa // exchageMessages (Lora * LoRaModule);
void GatheringInertialData::goToSleep (CellularModule * cellularTransceiver ) {
    return;
}

void GatheringInertialData::awake (CellularModule * cellularTransceiver, NonBlockingDelay * latency ) {
    return;
 }

void GatheringInertialData::calibrateIMU (IMU * inertialSensor) {
    return;
}

void GatheringInertialData::obtainInertialMeasures (IMU * inertialSensor,
 char * dataObtain, float * temperatureObtain) {
    float temperature;
    //inertialSensor->calibrate();
    if (this->currentStatus != TRACKER_STATUS_NONE_LOCALIZATION_MEAN_AVAILABLE) {
        if (inertialSensor->obtainInertialMeasures (dataObtain, &temperature)) {
            uartUSB.write (dataObtain , strlen (dataObtain ));  // debug only
          //  inertialSensor->changeState(new initializeMPU9250 (inertialSensor));
            this->tracker->changeState (new FormattingMessage (this->tracker, this->currentStatus));
            return;
        }
    }
    return;
}





//=====[Implementations of private methods]==================================