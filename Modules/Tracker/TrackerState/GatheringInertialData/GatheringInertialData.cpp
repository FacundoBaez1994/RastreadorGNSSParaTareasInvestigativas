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

void GatheringInertialData::obtainInertialMeasures (IMU * inertialSensor,
 char * dataObtain, float * temperatureObtain) {

     this->tracker->changeState (new FormattingMessage (this->tracker, this->currentStatus));

     /*
    float temperature;
    static char newInertialData [100];
    static char temporalBuffer [100];

    //inertialSensor->calibrate();
    if (this->currentStatus != TRACKER_STATUS_NONE_LOCALIZATION_MEAN_AVAILABLE) {
        if (inertialSensor->obtainInertialMeasures (newInertialData , &temperature)) {
            snprintf(temporalBuffer, sizeof(temporalBuffer), "%.2f,%s", temperature, newInertialData);
            strcpy (dataObtain, temporalBuffer );
            uartUSB.write("inertial data Obtain:", strlen("inertial data Obtain:"));
            uartUSB.write("\n\r", strlen("\n\r"));
            uartUSB.write (dataObtain, strlen (dataObtain ));  // debug only
            uartUSB.write("\n\r", strlen("\n\r"));
            this->tracker->changeState (new FormattingMessage (this->tracker, this->currentStatus));
            return;
        }
    }
    return;
    */
}





//=====[Implementations of private methods]==================================