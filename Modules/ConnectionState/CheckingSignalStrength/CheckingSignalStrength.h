//=====[#include guards - begin]===============================================

#ifndef _CHECKING_SIGNAL_STRENGTH_H_
#define _CHECKING_SIGNAL_STRENGTH_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "ConsultingIMEI.h"
#include "Non_Blocking_Delay.h"
#include <string>


//=====[Declaration of public data types]======================================
class CellularModule; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class CheckingSignalStrength : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    CheckingSignalStrength ();
    CheckingSignalStrength(CellularModule * mobileModule);
    virtual ~CheckingSignalStrength ();
    virtual void connect (ATCommandHandler * handler, NonBlockingDelay * refreshTime);
private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule;
    bool readyToSend;
    bool ATFirstResponseRead;
    bool signalLevelRetrived;
    float signalLevel;
//=====[Declaration of privates methods]=========================================
    bool checkExpectedResponse (char *response, float &value);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _CHECKING_SIGNAL_STRENGTH_H_