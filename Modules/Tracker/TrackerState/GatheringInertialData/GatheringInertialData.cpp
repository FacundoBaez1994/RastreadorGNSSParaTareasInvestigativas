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

#define LOG_MESSAGE_GATHERING_INERTIAL_DATA                     "\r\nGathering Inertial Data\r\n"
#define LOG_MESSAGE_GATHERING_INERTIAL_DATA_LEN                 (sizeof(LOG_MESSAGE_GATHERING_INERTIAL_DATA) - 1)

#define LOG_MESSAGE_PROM_RESULT                                 "\r\n\r\nPROM RESULT:\r\n\r\n"
#define LOG_MESSAGE_PROM_RESULT_LEN                             (sizeof(LOG_MESSAGE_PROM_RESULT) - 1)

#define LOG_MESSAGE_TIMESTAMP_RTC                               "\r\n\r\nTIMESTAMP RTC:\r\n\r\n"
#define LOG_MESSAGE_TIMESTAMP_RTC_LEN                           (sizeof(LOG_MESSAGE_TIMESTAMP_RTC) - 1)

#define LOG_MESSAGE_SAMPLE_ADDED_TO_VECTOR                      "Sample added to vector\r\n"
#define LOG_MESSAGE_SAMPLE_ADDED_TO_VECTOR_LEN                  (sizeof(LOG_MESSAGE_SAMPLE_ADDED_TO_VECTOR) - 1)

#define LOG_MESSAGE_ALL_IMU_SAMPLES_GATHERED                    "All IMU Samples Gather\r\n"
#define LOG_MESSAGE_ALL_IMU_SAMPLES_GATHERED_LEN                (sizeof(LOG_MESSAGE_ALL_IMU_SAMPLES_GATHERED) - 1)

#define LOG_MESSAGE_SH2_LINEAR_ACCELERATION  "SH2_LINEAR_ACCELERATION: ax: %.2f\t ay: %.2f\t az: %.2f\r\n"
#define LOG_MESSAGE_SH2_LINEAR_ACCELERATION_LEN  (sizeof(LOG_MESSAGE_SH2_LINEAR_ACCELERATION) - 1)

#define LOG_MESSAGE_SH2_ORIENTATION  "Status: %d\tYaw: %.2f\tPitch: %.2f\tRoll: %.2f\r\n"
#define LOG_MESSAGE_SH2_ORIENTATION_LEN  (sizeof(LOG_MESSAGE_SH2_ORIENTATION) - 1)

#define LOG_MESSAGE_VECTOR_SIZE "Vector size: %d\r\n"
#define LOG_MESSAGE_VECTOR_SIZE_LEN  (sizeof(LOG_MESSAGE_VECTOR_SIZE) - 1)



//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================

GatheringInertialData::GatheringInertialData (Tracker * tracker, trackerStatus_t trackerStatus) {
    this->tracker = tracker;
    this->currentStatus = trackerStatus;
    this->timeBeetwenSamples = new NonBlockingDelay (TIME_BETWEEN_SAMPLES * 100);
    this->auxiliarIMUData = new IMUData_t;
}


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

    if (inertialSensor == nullptr || imuData == nullptr) {
        this->tracker->changeState  (new GoingToSleep(this->tracker));
        return;
    }

    if (vectorCleared == false) {
        for (auto sample : IMUDataSamples) {
            delete sample;
        }
        uartUSB.write(LOG_MESSAGE_GATHERING_INERTIAL_DATA, LOG_MESSAGE_GATHERING_INERTIAL_DATA_LEN);
        IMUDataSamples.clear();
        vectorCleared = true;
    }


    if (mainIMUDataObtain == false) {
        if (inertialSensor->obtainInertialMeasures (imuData)) {
            uartUSB.write(LOG_MESSAGE_PROM_RESULT, LOG_MESSAGE_PROM_RESULT_LEN);
            snprintf(log, sizeof(log),
                LOG_MESSAGE_SH2_LINEAR_ACCELERATION,
                imuData->acceleration.ax, imuData->acceleration.ay,
                imuData->acceleration.az);
            uartUSB.write(log, strlen(log));
            snprintf(log, sizeof(log),
                LOG_MESSAGE_SH2_ORIENTATION,
                imuData->status, imuData->angles.yaw,
                imuData->angles.pitch, imuData->angles.roll);
            uartUSB.write(log, strlen(log));

            time_t seconds = time(NULL);
            uartUSB.write(LOG_MESSAGE_TIMESTAMP_RTC, LOG_MESSAGE_TIMESTAMP_RTC_LEN);
            char timestamp [TIMESTAMP_BUFFER_SIZE];
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

                uartUSB.write(LOG_MESSAGE_PROM_RESULT, LOG_MESSAGE_PROM_RESULT_LEN);
                snprintf(log, sizeof(log),
                    LOG_MESSAGE_SH2_LINEAR_ACCELERATION,
                    sample->acceleration.ax, sample->acceleration.ay,
                    sample->acceleration.az);
                uartUSB.write(log, strlen(log));
                snprintf(log, sizeof(log),
                    LOG_MESSAGE_SH2_ORIENTATION,
                    sample->status, sample->angles.yaw,
                    sample->angles.pitch, sample->angles.roll);
                uartUSB.write(log, strlen(log));

                IMUDataSamples.push_back(sample);
                uartUSB.write(LOG_MESSAGE_SAMPLE_ADDED_TO_VECTOR, LOG_MESSAGE_SAMPLE_ADDED_TO_VECTOR_LEN);
                snprintf(log, sizeof(log), LOG_MESSAGE_VECTOR_SIZE, IMUDataSamples.size());
                uartUSB.write(log, strlen(log));

                this->timeBeetwenSamples->restart ();
                readingNewSample = false;
                sampleCounter ++;
                if (sampleCounter >= MAX_SAMPLES) {
                    mainIMUDataObtain = false;
                    readingNewSample = false;
                    vectorCleared  = false;
                    sampleCounter = 0;
                    uartUSB.write(LOG_MESSAGE_ALL_IMU_SAMPLES_GATHERED, LOG_MESSAGE_ALL_IMU_SAMPLES_GATHERED_LEN);
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