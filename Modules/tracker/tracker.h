//=====[#include guards - begin]===============================================

#ifndef _TRACKER_H_
#define _TRACKER_H_

#include "non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "cellularModule.h"



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
    cellularModule* cellularTransmitter;
    nonBlockingDelay * latency;
};


//=====[#include guards - end]=================================================

#endif // _TRACKER_H_
