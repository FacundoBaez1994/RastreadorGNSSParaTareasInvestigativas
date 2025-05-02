//=====[#include guards - begin]===============================================

#ifndef _INITIALIZAING_H_
#define _INITIALIZAING_H_

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
class SensingBatteryStatus : public TrackerBaseState{
public:
//=====[Declaration of public methods]=========================================
    SensingBatteryStatus (Tracker * tracker);
    virtual ~SensingBatteryStatus ();
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);
private:
    //bool checkResponse (char * response, char * retrivMessage);
    Tracker * tracker;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _INITIALIZAING_H_