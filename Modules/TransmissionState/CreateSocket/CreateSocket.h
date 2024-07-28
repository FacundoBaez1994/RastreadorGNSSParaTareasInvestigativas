//=====[#include guards - begin]===============================================

#ifndef _CREATE_SOCKET_H_
#define _CREATE_SOCKET_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransmissionState.h"
#include "Non_Blocking_Delay.h"
#include "Sending.h"


//=====[Declaration of public data types]======================================
class CellularModule; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class CreateSocket : public TransmissionState {
public:
//=====[Declaration of public methods]=========================================
    CreateSocket();
    CreateSocket (CellularModule * mobileModule);
    virtual ~CreateSocket ();
    virtual bool send (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, char * ipDirection, int tcpPort);
private:
    CellularModule * mobileNetworkModule;
    bool readyToSend;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _ACTIVATE_PDP_H_