//=====[#include guards - begin]===============================================

#ifndef _GOING_TO_SLEEP_H_
#define _GOING_TO_SLEEP_H_

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
class GoingToSleep : public TrackerBaseState {
public:
//=====[Declaration of public methods]=========================================
    GoingToSleep (Tracker * tracker);
    virtual ~GoingToSleep ();
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);
    virtual void goToSleep (CellularModule * cellularTransceiver);
private:
    Tracker * tracker;
  

//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _GOING_TO_SLEEP_H_