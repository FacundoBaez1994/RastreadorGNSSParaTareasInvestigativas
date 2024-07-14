//=====[#include guards - begin]===============================================

#ifndef _TRACKER_H_
#define _TRACKER_H_

#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "CellularModule.h"



//=====[Declaration of public defines]=========================================


//=====[Declaration of public data types]======================================

typedef enum {
    TRACKER_STATE_ON,
    TRACKER_STATE_TURNING_ON,
    TRACKER_STATE_OFF,
} trackerState_t;

//=====[Declaration of public classes]=========================
/*
 * Class implementation for a GPS tracker
 * High hierarchy class
 * it will be instantiated and used from the main function
 */
class tracker {
public:
    tracker ();
    void update();
private:
    CellularModule* cellularTransmitter;
    NonBlockingDelay * latency;
};


//=====[#include guards - end]=================================================

#endif // _TRACKER_H_
