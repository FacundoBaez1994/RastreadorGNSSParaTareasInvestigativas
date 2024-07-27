//=====[#include guards - begin]===============================================

#ifndef _TRANSMISSION_UNAVAILABLE_H_
#define _TRANSMISSION_UNAVAILABLE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransmissionState.h"
#include "Non_Blocking_Delay.h"


//=====[Declaration of public data types]======================================
class CellularModule; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class TransmissionUnavailable : public TransmissionState {
public:
//=====[Declaration of public methods]=========================================
    TransmissionUnavailable();
    TransmissionUnavailable (CellularModule * mobileModule);
    virtual ~TransmissionUnavailable ();
    virtual void send (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, char * message, char * ipDirection, int tcpPort);
private:
    CellularModule * mobileNetworkModule;
    bool readyToSend;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CONNECTED_STATE_H_