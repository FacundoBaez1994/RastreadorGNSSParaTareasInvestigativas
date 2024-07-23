//=====[#include guards - begin]===============================================

#ifndef _DEFINE_PDP_CONTEXT_H_
#define _DEFINE_PDP_CONTEXT_H_
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
class DefinePDPContext : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    DefinePDPContext ();
    DefinePDPContext (CellularModule * mobileModule);
    virtual ~DefinePDPContext ();
    virtual void connect (ATCommandHandler * handler, NonBlockingDelay * refreshTime);
private:
    CellularModule * mobileNetworkModule;
    bool readyToSend;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _DEFINE_PDP_CONTEXT_H_