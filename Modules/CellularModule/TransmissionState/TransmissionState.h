//=====[#include guards - begin]===============================================

#ifndef _TRANSMISSION_STATE_H_
#define _TRANSMISSION_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"

//=====[Declaration of public data types]======================================
struct TcpSocket;

//=====[Declaration of public classes]=========================================
/*
 *  Interface - State desing pattern
 * 
 */
class TransmissionState {
public:
//=====[Declaration of public methods]=========================================
    virtual bool send (ATCommandHandler * handler,
     NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted);
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CELLULAR_STATE_H_