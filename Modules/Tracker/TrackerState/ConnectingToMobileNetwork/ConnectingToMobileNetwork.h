//=====[#include guards - begin]===============================================

#ifndef _CONNECTING_TO_MOBILE_NETWORK_H_
#define _CONNECTING_TO_MOBILE_NETWORK_H_

//==================[Libraries]===============================================

#include "Non_Blocking_Delay.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "TrackerBaseState.h"
#include "TrackerStatus.h"
#include "Tracker.h"

//=====[Declaration of public data types]======================================
class Tracker; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class ConnectingToMobileNetwork : public TrackerBaseState {
public:
//=====[Declaration of public methods]=========================================
    ConnectingToMobileNetwork (Tracker * tracker, trackerStatus_t trackerStatus);
    virtual ~ConnectingToMobileNetwork ();
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);
    virtual void connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation);
private:
    Tracker * tracker;
    trackerStatus_t currentStatus;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _CONNECTING_TO_MOBILE_NETWORK_H_