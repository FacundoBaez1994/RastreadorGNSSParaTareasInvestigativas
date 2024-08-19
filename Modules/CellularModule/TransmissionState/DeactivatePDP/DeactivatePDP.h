//=====[#include guards - begin]===============================================

#ifndef _ACTIVATE_PDP_H_
#define _ACTIVATE_PDP_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransmissionState.h"
#include "Non_Blocking_Delay.h"
#include "TransmissionUnavailable.h"


//=====[Declaration of public data types]======================================
class CellularModule; //debido a declaracion adelantada
struct TcpSocket;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class DeactivatePDP : public TransmissionState {
public:
//=====[Declaration of public methods]=========================================
    DeactivatePDP (CellularModule * mobileModule, bool transmissionWasASuccess);
    virtual ~DeactivatePDP ();
    virtual CellularTransmissionStatus_t send (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted);
    virtual void enableTransmission ();
private:
    CellularModule * mobileNetworkModule;
    bool readyToSend;
    bool transmissionWasASuccess;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _ACTIVATE_PDP_H_