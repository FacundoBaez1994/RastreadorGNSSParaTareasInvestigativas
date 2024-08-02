//=====[#include guards - begin]===============================================

#ifndef _OBTAINING_POSITION_INFORMATION_H_
#define _OBTAINING_POSITION_INFORMATION_H_

//==================[Libraries]===============================================

#include "GNSSModule.h"
#include "GeopositioningState.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "Non_Blocking_Delay.h"
#include "TurningOnGNSS.h"


//=====[Declaration of public data types]======================================
class GNSSModule; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class ObtainingPositionInformation : public GeopositioningState {
public:
//=====[Declaration of public methods]=========================================
    ObtainingPositionInformation ();
    ObtainingPositionInformation  (GNSSModule * aGNSSModule);
    virtual ~ObtainingPositionInformation   ();
    bool retrivGeopositioning (char * message, ATCommandHandler * ATHandler,
     NonBlockingDelay * refreshTime) ;
private:
    GNSSModule * currentGNSSModule;
    bool readyToSend;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _OBTAINING_POSITION_INFORMATION_H_