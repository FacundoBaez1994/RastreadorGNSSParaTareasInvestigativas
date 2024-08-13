//=====[#include guards - begin]===============================================

#ifndef _SENDING_H_
#define _SENDING_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransmissionState.h"
#include "Non_Blocking_Delay.h"
#include "CloseSocket.h"


//=====[Declaration of public data types]======================================
class CellularModule; //debido a declaracion adelantada
struct TcpSocket;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class Sending : public TransmissionState {
public:
//=====[Declaration of public methods]=========================================
    Sending();
    Sending (CellularModule * mobileModule);
    virtual ~Sending ();
    virtual bool send (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted);
private:
    CellularModule * mobileNetworkModule;
    bool readyToSend;
    bool transmissionEnable;
    bool watingForConfirmation;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _SENDING_H_