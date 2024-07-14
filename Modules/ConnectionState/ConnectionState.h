//=====[#include guards - begin]===============================================

#ifndef _CONNECTION_STATE_H_
#define _CONNECTION_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "cellularModule.h"

//=====[Declaration of public data types]======================================


//=====[Declaration of public classes]=========================================
/*
 *  Interface - State desing pattern
 * 
 */
class ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    virtual void connect (ATCommandHandler * handler,
     nonBlockingDelay * refreshTime);
private:
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CELLULAR_STATE_H_