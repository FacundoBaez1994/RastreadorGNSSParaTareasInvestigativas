//=====[#include guards - begin]===============================================

#ifndef _IDLE_STATE_H_
#define _IDLE_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "non_Blocking_Delay.h"

//=====[Declaration of public data types]======================================
class cellularModule; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class IdleState : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    IdleState (cellularModule * mobileModule);
    virtual ~IdleState ();
    virtual void connect (ATCommandHandler * handler, nonBlockingDelay * refreshTime);
private:
    cellularModule * mobileNetworkModule;
    bool readyToSend;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CELLULAR_STATE_H_