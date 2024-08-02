//=====[#include guards - begin]===============================================

#ifndef _GNSS_MODULE_H_
#define _GNSS_MODULE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "GeopositioningState.h"
#include "PowerManager.h"
#include "TurningOnGNSS.h"

//=====[Declaration of public data types]======================================
// class ConnectionState; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  
 * 
 */
class GNSSModule {
public:

//=====[Declaration of public methods]=========================================
    GNSSModule (PowerManager * aPowerManager, ATCommandHandler* ATHandler);
    virtual ~GNSSModule ();
    void startStopUpdate ();
    void reboot ();
    bool retrivGeopositioning (char * message);
    void changeGeopositioningState  (GeopositioningState* newGeopositioningState);
    PowerManager* getPowerManager ();
    BufferedSerial* getUART ();
private:
//=====[Declaration of privates atributes]=========================================
    NonBlockingDelay* refreshTime;
    ATCommandHandler* ATHandler;
    PowerManager * modulePowerManager;
    powerStatus_t currentPowerStatus;
    GeopositioningState * currentGeopositioningState;

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _GNSS_MODULE_H_