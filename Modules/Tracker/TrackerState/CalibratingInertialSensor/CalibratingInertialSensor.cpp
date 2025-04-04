//=====[Libraries]=============================================================

#include "CalibratingInertialSensor.h"
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
CalibratingInertialSensor::CalibratingInertialSensor (Tracker * tracker) {
    this->tracker = tracker;
}

/** 
* @brief
* 
* @param 
*/
CalibratingInertialSensor::~CalibratingInertialSensor() {
    this->tracker = NULL;
}

void CalibratingInertialSensor::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    //cellularTransceiver->startStopUpdate();
 }

void CalibratingInertialSensor::obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata) {
    return;
}

 void CalibratingInertialSensor::connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation) {
    return; 
}


void CalibratingInertialSensor::obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors ) {
    return;
}
    // IMU Method 1
    // IMU Methord 2

void  CalibratingInertialSensor::formatMessage (char * formattedMessage, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
     char * inertialData, BatteryData  * batteryStatus) {
    return;
}

void CalibratingInertialSensor::exchangeMessages (CellularModule * cellularTransceiver,
    char * message, TcpSocket * socketTargetted, char * receivedMessage ){

    return;
}
    // agregar LoRa // exchageMessages (Lora * LoRaModule);
void CalibratingInertialSensor::goToSleep (CellularModule * cellularTransceiver ) {
    return;
}

void CalibratingInertialSensor::awake (CellularModule * cellularTransceiver, NonBlockingDelay * latency ) {
    return;
 }

void CalibratingInertialSensor::calibrateIMU (IMU * inertialSensor) {
    static bool firstcalibrationOK = false;
    char inertialData [100];
    float temp;

    if (firstcalibrationOK == false) {
        if (inertialSensor->calibrate() == true) {
            firstcalibrationOK = true;
            
        }
    }
    if (firstcalibrationOK == true ) { 
        // primera lectura de prueba
        if (inertialSensor->obtainInertialMeasures(inertialData, &temp) == true) {
            this->tracker->changeState(new SensingBatteryStatus (this->tracker));
            firstcalibrationOK = false;
            return;
        }
    }
    return;
}

void CalibratingInertialSensor::obtainInertialMeasures (IMU * inertialSensor,
 char * dataObtain, float * temperatureObtain) {
    return;
}





//=====[Implementations of private methods]==================================