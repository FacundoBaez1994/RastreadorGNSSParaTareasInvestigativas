//=====[Libraries]=============================================================

#include "GatheringInertialData.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
//#include "SensingBatteryStatus.h"
#include "FormattingMessage.h"
#include "GoingToSleep.h"

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20
#define TIME_BETWEEN_SAMPLES 10000
#define TICKS_PER_SECOND 100
#define MAX_SAMPLES 10

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
    this->timeBeetwenSamples = new NonBlockingDelay (TIME_BETWEEN_SAMPLES * 100);
    this->auxiliarIMUData = new IMUData_t;
}

/** 
* @brief
* 
* @param 
*/
GatheringInertialData::~GatheringInertialData() {
    this->tracker = nullptr;
    delete this->timeBeetwenSamples;
    this->timeBeetwenSamples  = nullptr;
    delete  this->auxiliarIMUData;
    this->auxiliarIMUData  = nullptr;
}

void GatheringInertialData::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    //cellularTransceiver->startStopUpdate();
 }

void GatheringInertialData::obtainInertialMeasures (IMUManager * inertialSensor,
 IMUData_t * imuData, std::vector<IMUData_t*> &IMUDataSamples) {

    char log [100];
    static bool mainIMUDataObtain = false;
    static bool readingNewSample = false;
    static bool vectorCleared  = false;
    static int sampleCounter = 0;

    if (vectorCleared == false) {
        for (auto sample : IMUDataSamples) {
            delete sample;
        }
        snprintf(log, sizeof(log), "\n\r Gathering Inertial Data \n\r");
        uartUSB.write(log, strlen(log));
        IMUDataSamples.clear();
        vectorCleared = true;
    }


    if (mainIMUDataObtain == false) {
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

            time_t seconds = time(NULL);
            snprintf(log, sizeof(log), "\n\r \n\r TIMESTAMP RTC: \n\r \n\r");
            uartUSB.write(log, strlen(log));
            char timestamp [20];
            epochToTimestamp(seconds, timestamp, sizeof(timestamp));
            uartUSB.write(timestamp, strlen(timestamp));
            uartUSB.write("\n\r", strlen("\n\r"));
            strcpy (imuData->timestamp, timestamp);

            
            if (this->currentStatus != TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_LORA_UNAVAILABLE_GATHERING_INERTIAL_INFO) {
                mainIMUDataObtain = false;
                readingNewSample = false;
                vectorCleared  = false;
                this->tracker->changeState (new FormattingMessage (this->tracker, 
                this->currentStatus));
                return;
            } else {
                mainIMUDataObtain = true;
                if (imuData->timeBetweenSamples > 0) {
                    this->timeBeetwenSamples->write(imuData->timeBetweenSamples * TICKS_PER_SECOND);
                }
            }
        }
    }
    if (mainIMUDataObtain == true && this->currentStatus == TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_LORA_UNAVAILABLE_GATHERING_INERTIAL_INFO) {
        if (this->timeBeetwenSamples->read()) {
            readingNewSample = true;
        }
        if ( readingNewSample == true) {
            if (inertialSensor->obtainInertialMeasures (this->auxiliarIMUData)) {
                IMUData_t* sample = new IMUData_t;
                sample->acceleration.ax = this->auxiliarIMUData->acceleration.ax;
                sample->acceleration.ay = this->auxiliarIMUData->acceleration.ay;
                sample->acceleration.az = this->auxiliarIMUData->acceleration.az;
                sample->status = this->auxiliarIMUData->status;
                sample->angles.yaw = this->auxiliarIMUData->angles.yaw;
                sample->angles.pitch = this->auxiliarIMUData->angles.pitch;
                sample->angles.roll = this->auxiliarIMUData->angles.roll;

                snprintf(log, sizeof(log), "\n\r \n\r PROM RESULT:\n\r \n\r");
                uartUSB.write(log, strlen(log));
                snprintf(log, sizeof(log),
                    "SH2_LINEAR_ACCELERATION: ax: %.2f\t ay: %.2f\t az: %.2f\n\r",
                    sample->acceleration.ax, sample->acceleration.ay,
                    sample->acceleration.az);
                uartUSB.write(log, strlen(log));
                snprintf(log, sizeof(log),
                    "Status: %d\tYaw: %.2f\tPitch: %.2f\tRoll: %.2f\n\r",
                    sample->status, sample->angles.yaw,
                    sample->angles.pitch, sample->angles.roll);
                uartUSB.write(log, strlen(log));

                IMUDataSamples.push_back(sample);
                uartUSB.write("Sample added to vector\r\n", strlen("Sample added to vector\r\n"));
                snprintf(log, sizeof(log), "Vector size: %d\r\n", IMUDataSamples.size());
                uartUSB.write(log, strlen(log));

                this->timeBeetwenSamples->restart ();
                readingNewSample = false;
                sampleCounter ++;
                if (sampleCounter >= MAX_SAMPLES) {
                    mainIMUDataObtain = false;
                    readingNewSample = false;
                    vectorCleared  = false;
                    sampleCounter = 0;
                    uartUSB.write("All IMU Samples Gather\r\n", strlen("All IMU Samples Gather\r\n"));
                    // this->tracker->changeState (new GoingToSleep (this->tracker));
                    this->tracker->changeState (new FormattingMessage (this->tracker, TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_LORA_UNAVAILABLE_GATHERED_INERTIAL_INFO_SAVING_MESSAGE));
                    return;
                }
            }
        }
    }
    return;
    
}





//=====[Implementations of private methods]==================================