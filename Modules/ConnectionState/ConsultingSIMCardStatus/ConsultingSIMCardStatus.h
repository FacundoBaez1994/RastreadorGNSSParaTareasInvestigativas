//=====[#include guards - begin]===============================================

#ifndef _CONSULTING_SIMCARD_STATUS_H_
#define _CONSULTING_SIMCARD_STATUS_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "ConsultingNetworkStatus.h"
#include "Non_Blocking_Delay.h"
#include <string>

//=====[Declaration of public data types]======================================
class CellularModule; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class ConsultingSIMCardStatus : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    ConsultingSIMCardStatus();
    ConsultingSIMCardStatus(CellularModule * mobileModule);
    virtual ~ConsultingSIMCardStatus ();
    virtual void connect (ATCommandHandler * handler, NonBlockingDelay * refreshTime);
private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule;
    bool readyToSend;
    bool ATFirstResponseRead;
    bool simCardDetected;
    int rebootCounter;
//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CONSULTING_SIMCARD_STATUS_H_