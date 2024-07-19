//=====[#include guards - begin]===============================================

#ifndef _CONSULTING_IMEI_H_
#define _CONSULTING_IMEI_H_

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
class ConsultingIMEI : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    ConsultingIMEI();
    ConsultingIMEI(CellularModule * mobileModule);
    virtual ~ConsultingIMEI ();
    virtual void connect (ATCommandHandler * handler, NonBlockingDelay * refreshTime);
private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule;
    bool readyToSend;
    bool ATFirstResponseRead;
    bool IMEIRetrived;
    long long int IMEI;
//=====[Declaration of privates methods]=========================================
    bool RetrivIMEI (char *response, long long int &value);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _CONSULTING_IMEI_H_