//=====[#include guards - begin]===============================================

#ifndef _GATHERING_CELL_INFORMATION_H_
#define _GATHERING_CELL_INFORMATION_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "TrackerBaseState.h"
#include "Tracker.h"

//=====[Declaration of public data types]======================================
class Tracker; 

//=====[Declaration of public classes]=========================================
/**
 * @class GatheringCellInformation
 * @brief State responsible for acquiring neighboring cell data.
 * @details This state interacts with the cellular transceiver to retrieve 
 * information about nearby cells and stores it in the provided 
 * container. If the acquisition is successful, it transitions 
 * the tracker to the next state in the sequence (e.g., 
 * GatheringInertialData). 
 * It also manages periodic power status updates for the cellular module 
 * and can perform motion stability checks through the IMUManager.
 * @note Part of the State design pattern for tracker device operation.
 */
class GatheringCellInformation : public TrackerBaseState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructor.
     * @param tracker Pointer to the tracker context that owns this state.
     */
    GatheringCellInformation (Tracker * tracker);

    /**
     * @brief Destructor.
     */
    virtual ~GatheringCellInformation ();

    /**
     * @brief Update the cellular module's power status.
     * @param cellularTransceiver Pointer to the cellular module interface.
     * @param currentBatteryStatus Pointer to the current battery status structure.
     */
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);

    /**
     * @brief Retrieve information about neighboring cells.
     * @details Attempts to obtain cell information for the given number of neighbors.
     * If successful, triggers a state change to gather inertial data.
     * @param cellularTransceiver Pointer to the cellular module interface.
     * @param neighborsCellInformation Reference to the vector that will store the retrieved cell information.
     * @param numberOfNeighbors Number of neighboring cells to retrieve.
    */
    virtual void obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors );

    /**
     * @brief Check device motion stability.
     * Uses the IMU sensor manager to determine whether the device is stable or moving.
     *  Not allowed in this state.
     * @param inertialSensor Pointer to the IMU sensor manager.
     * @param newMotionStatus Pointer to store the detected motion status.
     */
    virtual void checkStabillity (IMUManager * inertialSensor, deviceMotionStatus_t * newMotionStatus);

private:
    Tracker * tracker;  /**< Pointer to the tracker context that owns this state. */
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _GATHERING_CELL_INFORMATION_H_