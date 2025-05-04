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
#include "GNSSUnavailable.h"
#include "GNSSState.h"

//=====[Declaration of public data types]======================================

 struct GNSSData {
    char timestamp [20];    // <UTC> en formato HHMMSS.SS + <date> en formato DDMMYY
    float latitude;        // <latitude> en formato decimal
    float longitude;       // <longitude> en formato decimal
    float hdop;            // <hdop>
    float altitude;        // <altitude>
    int fix;               // <fix>
    float cog;             // <cog>
    float spkm;            // <spkm>
    float spkn;            // <spkn>

    int nsat;              // <nsat>
 };

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
    GNSSState_t retrivGeopositioning (GNSSData * Geodata);
    void changeGeopositioningState  (GeopositioningState* newGeopositioningState);
    void enableGNSS ();
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