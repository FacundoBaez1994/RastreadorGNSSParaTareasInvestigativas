//=====[#include guards - begin]===============================================

#ifndef _CONSULTING_AVAILABLE_OPERATORS_H_
#define _CONSULTING_AVAILABLE_OPERATORS_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "Non_Blocking_Delay.h"
#include <string>

//=====[Declaration of public data types]======================================
class CellularModule; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class ConsultingAvailableOperators : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    ConsultingAvailableOperators();
    ConsultingAvailableOperators (CellularModule * mobileModule);
    virtual ~ConsultingAvailableOperators ();
    virtual void connect (ATCommandHandler * handler, NonBlockingDelay * refreshTime);
private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule;
    bool readyToSend;
    bool operatorsInformationRetrived;
   
    char * currentOperator;
//=====[Declaration of privates methods]=========================================
    bool retrivOperatorsInformation (char *response);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _CONSULTING_AVAILABLE_OPERATORS_H_