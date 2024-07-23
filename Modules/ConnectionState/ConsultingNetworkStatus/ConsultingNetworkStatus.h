//=====[#include guards - begin]===============================================

#ifndef _CONSULTING_NETWORK_STATUS_H_
#define _CONSULTING_NETWORK_STATUS_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "ConsultingAvailableOperators.h"
#include "Non_Blocking_Delay.h"
#include <string>

//=====[Declaration of public data types]======================================
class CellularModule; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class ConsultingNetworkStatus : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    ConsultingNetworkStatus();
    ConsultingNetworkStatus(CellularModule * mobileModule);
    virtual ~ConsultingNetworkStatus ();
    virtual void connect (ATCommandHandler * handler, NonBlockingDelay * refreshTime);
private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule;
    bool readyToSend;
    bool ATFirstResponseRead;
    bool cellDataRetrived;
    int lac;
    unsigned long cellId; 
    int accessTechnology;
    int registrationStatus;
//=====[Declaration of privates methods]=========================================
    bool retrivIdCellData (char *response);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _CONSULTING_NETWORK_STATUS_H_