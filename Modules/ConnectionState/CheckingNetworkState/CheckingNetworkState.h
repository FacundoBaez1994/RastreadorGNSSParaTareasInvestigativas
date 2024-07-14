//=====[#include guards - begin]===============================================

#ifndef _CHECKING_NETWORK_STATE_H_
#define _CHECKING_NETWORK_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "Non_Blocking_Delay.h"

//=====[Declaration of public data types]======================================
class CellularModule; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class CheckingNetworkState : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    CheckingNetworkState ();
    CheckingNetworkState(CellularModule * mobileModule);
    virtual ~CheckingNetworkState ();
    virtual void connect (ATCommandHandler * handler, NonBlockingDelay * refreshTime);
private:
    CellularModule * mobileNetworkModule;
    bool readyToSend;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CHECKING_NETWORK_STATE_H_