//=====[#include guards - begin]===============================================

#ifndef _GATHERING_CELL_INFORMATION_H_
#define _GATHERING_CELL_INFORMATION_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "TrackerBaseState.h"
#include "Tracker.h"

//=====[Declaration of public data types]======================================
class Tracker; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class GatheringCellInformation : public TrackerBaseState {
public:
//=====[Declaration of public methods]=========================================
    GatheringCellInformation (Tracker * tracker);
    virtual ~GatheringCellInformation ();
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);
    virtual void obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors );
    virtual void checkStabillity (IMUManager * inertialSensor, deviceMotionStatus_t * newMotionStatus);

private:
    Tracker * tracker;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _GATHERING_CELL_INFORMATION_H_