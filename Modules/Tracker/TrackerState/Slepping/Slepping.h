//=====[#include guards - begin]===============================================

#ifndef _SLEEPING_H_
#define _SLEEPING_H_

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
class Slepping : public TrackerBaseState {
public:
//=====[Declaration of public methods]=========================================
    Slepping (Tracker * tracker);
    virtual ~Slepping ();
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);
    virtual void awake (CellularModule * cellularTransceiver, NonBlockingDelay * latency,
    NonBlockingDelay * silentTimer);
private:
    Tracker * tracker;
  

//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _SLEEPING_H_