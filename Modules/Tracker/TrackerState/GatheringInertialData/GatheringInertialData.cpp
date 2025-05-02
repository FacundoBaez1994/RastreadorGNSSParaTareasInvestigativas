//=====[Libraries]=============================================================

#include "GatheringInertialData.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
//#include "SensingBatteryStatus.h"
#include "FormattingMessage.h"

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

void GatheringInertialData::obtainInertialMeasures (IMUManager * inertialSensor,
 IMUData_t * imuData) {

    char log [100];


    if (this->currentStatus != TRACKER_STATUS_NONE_LOCALIZATION_MEAN_AVAILABLE) {
        if (inertialSensor->obtainInertialMeasures (imuData)) {
            snprintf(log, sizeof(log), "\n\r \n\r PROM RESULT:\n\r \n\r");
            uartUSB.write(log, strlen(log));
            snprintf(log, sizeof(log),
                "SH2_LINEAR_ACCELERATION: ax: %.2f\t ay: %.2f\t az: %.2f\n\r",
                imuData->acceleration.ax, imuData->acceleration.ay,
                imuData->acceleration.az);
            uartUSB.write(log, strlen(log));
            snprintf(log, sizeof(log),
                "Status: %d\tYaw: %.2f\tPitch: %.2f\tRoll: %.2f\n\r",
                imuData->status, imuData->angles.yaw,
                imuData->angles.pitch, imuData->angles.roll);
            uartUSB.write(log, strlen(log));
            this->tracker->changeState (new FormattingMessage (this->tracker, 
            this->currentStatus));
            return;
        }
    }
    return;
    
}





//=====[Implementations of private methods]==================================