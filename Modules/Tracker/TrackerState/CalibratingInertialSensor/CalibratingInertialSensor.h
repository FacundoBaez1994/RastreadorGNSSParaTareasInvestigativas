//=====[#include guards - begin]===============================================
#ifndef _CALIBRATING_INERTIAL_SENSOR_H_
#define _CALIBRATING_INERTIAL_SENSOR_H_

//==================[Libraries]===============================================
#include "mbed.h"
#include "arm_book_lib.h"
#include "TrackerBaseState.h"
#include "Tracker.h"

//=====[Declaration of public data types]======================================
class Tracker; ///< Forward declaration of Tracker class

//=====[Declaration of public classes]=========================================
/**
 * @brief State class implementing the calibration of the inertial measurement unit (IMU)
 * @details This class implements the "CalibratingInertialSensor" state in the Tracker
 * state machine. It attempts to initialize and calibrate the inertial sensor hardware.
 * Upon successful calibration, it transitions the Tracker to the next state.
 */
class CalibratingInertialSensor: public TrackerBaseState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructor
     * @param tracker Pointer to the main Tracker object, used for state transitions and accessing device data.
     */
    CalibratingInertialSensor (Tracker * tracker);

    /**
     * @brief Destructor
     */
    virtual ~CalibratingInertialSensor ();

    /**
     * @brief Attempts to initialize and calibrate the inertial sensor (IMU)
     * @param inertialSensor Pointer to the IMUManager object that handles the IMU hardware
     * @details If initialization is successful, the state transitions to SensingBatteryStatus.
     * Otherwise, it remains in this state until calibration is successful.
     */
    virtual void calibrateIMU (IMUManager * inertialSensor);
private:
    Tracker * tracker; ///< Pointer to the Tracker instance for state management
  

//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CALIBRATING_INERTIAL_SENSOR_H_