//=====[#include guards - begin]===============================================

#ifndef _GATHERING_INERTIAL_DATA_H_
#define _GATHERING_INERTIAL_DATA_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "TrackerBaseState.h"
#include "Tracker.h"
#include "Non_Blocking_Delay.h"
#include "timeStampTools.h"
#include "BufferSizes.h"

//=====[Declaration of public data types]======================================
class Tracker; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/**
 * @class GatheringInertialData
 * @brief State responsible for collecting inertial measurements from the IMU sensor.
 * @details This state acquires IMU data, either a single set of measurements or a sequence of 
 * samples at a defined time interval. The collected information is stored for later 
 * processing (e.g., message formatting) and may include acceleration, orientation, 
 * status, and a timestamp. 
 * The sampling behavior depends on the tracker’s current operational status:
 * - In normal conditions, only one measurement is obtained before transitioning.
 * - If GNSS and network connectivity are unavailable, multiple samples are collected 
 *   at defined intervals before transitioning.
 * @note Part of the State design pattern for tracker device operation.
 */
class GatheringInertialData: public TrackerBaseState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructor.
     * @param tracker Pointer to the tracker context that owns this state.
     * @param trackerStatus Operational status of the tracker to guide sampling behavior.
     */
    GatheringInertialData (Tracker * tracker, trackerStatus_t trackerStatus);

    /**
     * @brief Destructor.
     * 
     * Frees allocated memory for auxiliary data buffers and timers.
    */
    virtual ~GatheringInertialData ();

        /**
     * @brief Acquire inertial measurements.
     * @details If configured for single-shot mode, obtains one measurement from the IMU and 
     * immediately transitions to the next state.  
     * If configured for multi-sample mode, collects a predefined number of samples 
     * spaced by a fixed interval.
     * @param inertialSensor Pointer to the IMU manager interface.
     * @param imuData Pointer to store the primary IMU measurement.
     * @param IMUDataSamples Reference to a vector to store additional sample sets.
     */
    virtual void obtainInertialMeasures (IMUManager * inertialSensor, IMUData_t * imuData, std::vector<IMUData_t*> &IMUDataSamples);

    /**
     * @brief Update the power status of the cellular transceiver.
     * @param cellularTransceiver Pointer to the cellular module interface.
     * @param currentBatteryStatus Pointer to the current battery status structure.
    */
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);
private:
    Tracker * tracker;                /**< Pointer to the tracker context that owns this state. */
    NonBlockingDelay * timeBeetwenSamples; /**< Timer for controlling the delay between samples. */
    trackerStatus_t currentStatus;     /**< Current operational status guiding sampling behavior. */
    IMUData_t * auxiliarIMUData;       /**< Buffer for temporarily storing an IMU sample. */
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _GATHERING_INERTIAL_DATA_H_