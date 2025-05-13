//=====[#include guards - begin]===============================================

#ifndef _GETTING_GNSS_POSITION_H_
#define _GETTING_GNSS_POSITION_H_

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
class GettingGNSSPosition : public TrackerBaseState {
public:
//=====[Declaration of public methods]=========================================
    GettingGNSSPosition (Tracker * tracker);
    virtual ~GettingGNSSPosition ();
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);
    virtual void obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata);
private:
    //bool checkResponse (char * response, char * retrivMessage);
    Tracker * tracker;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _GETTING_GNSS_POSITION_H_