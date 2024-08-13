//=====[#include guards - begin]===============================================

#ifndef _CONNECTION_STATE_H_
#define _CONNECTION_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"

//=====[Declaration of public data types]======================================
struct CellInformation;

//=====[Declaration of public classes]=========================================
/*
 *  Interface - State desing pattern
 * 
 */
class ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    virtual bool connect (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, CellInformation * currentCellInformation);
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CELLULAR_STATE_H_