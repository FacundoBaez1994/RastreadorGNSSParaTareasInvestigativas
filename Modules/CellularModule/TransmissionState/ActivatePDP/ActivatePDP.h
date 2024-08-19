//=====[#include guards - begin]===============================================

#ifndef _ACTIVATE_PDP_H_
#define _ACTIVATE_PDP_H_

//==================[Libraries]===============================================

#include "CellularModule.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransmissionState.h"
#include "Non_Blocking_Delay.h"
#include "CreateSocket.h"
#include "TransmissionUnavailable.h"


//=====[Declaration of public data types]======================================
class CellularModule; //debido a declaracion adelantada
struct TcpSocket;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class ActivatePDP : public TransmissionState {
public:
//=====[Declaration of public methods]=========================================
    ActivatePDP();
    ActivatePDP (CellularModule * mobileModule);
    virtual ~ActivatePDP ();
    virtual void enableTransmission ();
    virtual CellularTransmissionStatus_t send (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted);
private:
    CellularModule * mobileNetworkModule;
    bool readyToSend;
    int Attempts; 
    int maxAttempts; 
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _ACTIVATE_PDP_H_