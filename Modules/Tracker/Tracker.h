//=====[#include guards - begin]===============================================

#ifndef _TRACKER_H_
#define _TRACKER_H_

#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "CellularModule.h"
#include "GNSSModule.h"
#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "string.h"
#include "TrackerState.h"
#include "TrackerStatus.h"
#include "IMUManager.h"
#include <CustomJWT.h>





//=====[Declaration of public defines]=========================================


//=====[Declaration of public data types]======================================

//=====[Declaration of public classes]=========================
/*
 * Class implementation for a GPS tracker
 * High hierarchy class
 * it will be instantiated and used from the main function
 */
class Tracker {
public:
    Tracker ();
    virtual ~Tracker ();
    void update();
    void changeState  (TrackerState * newTrackerState);
    void encodeJWT(char * payloadToJWT, char * jwtEncoded);
    void decodeJWT (char * jwtToDecode, char * payloadRetrived);
    
private:
    TrackerState * currentState;
    IMUManager * inertialSensor;
    
    CellularModule* cellularTransceiver;
    TcpSocket * socketTargetted;
    CellInformation * currentCellInformation; 

    GNSSModule* currentGNSSModule;
    GNSSData * currentGNSSdata;
    NonBlockingDelay * latency;
    BatteryData  * batteryStatus;

    CustomJWT * jwt;
    char JWTKey [40] = "a-string-secret-at-least-256-bits-long";

};


//=====[#include guards - end]=================================================

#endif // _TRACKER_H_
