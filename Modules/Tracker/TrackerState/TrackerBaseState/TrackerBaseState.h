//=====[#include guards - begin]===============================================

#ifndef _TRACKER_BASE_STATE_H_
#define _TRACKER_BASE_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "TrackerState.h"
#include "Tracker.h"

//=====[Declaration of public data types]======================================
class Tracker; ///< Forward declaration of the Tracker class.


//=====[Declaration of public classes]=========================================
/**
 * @brief Abstract base class for tracker states (State design pattern).
 * This class provides default (empty) implementations for all methods declared
 * in the `TrackerState` interface. It allows derived state classes to override
 * only the methods they need, avoiding the need to implement unused functionality.
 * The only non-empty default method is `checkStabillity()`, which delegates the
 * stability check to the provided IMUManager instance.
 */
class TrackerBaseState : public TrackerState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @copydoc TrackerState::calibrateIMU()
     * Default implementation: does nothing.
     */
    virtual void calibrateIMU (IMUManager * inertialSensor);

    /**
     * @copydoc TrackerState::obtainInertialMeasures()
     * Default implementation: does nothing.
     */
    virtual void obtainInertialMeasures (IMUManager * inertialSensor, IMUData_t * inertialData, 
    std::vector<IMUData_t*> &IMUDataSamples);

    /**
     * @copydoc TrackerState::updatePowerStatus()
     * Default implementation: does nothing.
     */
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);

    /**
     * @copydoc TrackerState::obtainGNSSPosition()
     * Default implementation: does nothing.
     */
    virtual void obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata);


    /**
     * @copydoc TrackerState::connectToMobileNetwork()
     * Default implementation: does nothing.
     */
    virtual void connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation);

    /**
     * @copydoc TrackerState::obtainNeighborCellsInformation()
     * Default implementation: does nothing.
     */
    virtual void obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors );

    /**
     * @copydoc TrackerState::formatMessage()
     * Default implementation: does nothing.
    */
    virtual void formatMessage (char * formattedMessage, const CellInformation* aCellInfo,
    const GNSSData* GNSSInfo, const std::vector<CellInformation*> &neighborsCellInformation,
    const IMUData_t * imuData, const std::vector<IMUData_t*> &IMUDataSample, const BatteryData  * batteryStatus); 

    
    /**
     * @brief Exchanges messages with a remote server via the cellular transceiver.
     * @param cellularTransceiver Pointer to the CellularModule instance.
     * @param message Pointer to the message to send.
     * @param serverTargetted a struct with infomation over the remote server
     * @param receivedMessage Pointer to the buffer where the received message will be stored.
     */
    virtual void exchangeMessages (CellularModule* cellularTransceiver,
    char* message,  RemoteServerInformation* serverTargetted, char* receivedMessage );

    /**
     * @copydoc TrackerState::exchangeMessages(LoRaClass * LoRaModule, char * message, 
     * char * receivedMessage)
     * Default implementation: does nothing.
     */
    virtual void exchangeMessages (LoRaClass * LoRaModule, char * message, char * receivedMessage );
    
    /**
     * @copydoc TrackerState::goToSleep()
     * Default implementation: does nothing.
     */
    virtual void goToSleep (CellularModule * cellularTransceiver);

    /**
     * @copydoc TrackerState::awake()
     * Default implementation: does nothing.
     */
    virtual void awake (CellularModule * cellularTransceiver, NonBlockingDelay * latency, NonBlockingDelay * silentTimer);

    /**
     * @copydoc TrackerState::checkStabillity()
     * Default implementation: calls `IMUManager::checkStability()` on the given inertial sensor.
     */
    virtual void checkStabillity (IMUManager * inertialSensor, deviceMotionStatus_t * newMotionStatus);

    /**
     * @copydoc TrackerState::saveMessage()
     * Default implementation: does nothing.
     */
    virtual void saveMessage (EEPROMManager * memory, char * message);

    /**
     * @copydoc TrackerState::loadMessage()
     * Default implementation: does nothing.
     */
    virtual void loadMessage (EEPROMManager * memory, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
    IMUData_t * imuData, std::vector<IMUData_t*> &IMUDataSamples, BatteryData  * batteryStatus, char * buffer);
private:
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _TRACKER_BASE_STATE_H_