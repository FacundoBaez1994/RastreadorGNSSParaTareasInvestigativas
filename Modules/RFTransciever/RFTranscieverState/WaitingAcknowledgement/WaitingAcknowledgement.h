//=====[#include guards - begin]===============================================

#ifndef _WAITING_ACKNOWLEDGEMENT_H_
#define _WAITING_ACKNOWLEDGEMENT_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "Tracker.h"
#include "Non_Blocking_Delay.h"
#include "RFTransicieverState.h"
#include "SendingMessage.h"
#include <algorithm> // Para std::find
#include <string>
#include <vector>
#include <cstring> 

//=====[Declaration of public data types]======================================
class Tracker; //debido a declaracion adelantada
//struct TcpSocket;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class WaitingAcknowledgement : public RFTransicieverState {
public:
//=====[Declaration of public methods]=========================================
    WaitingAcknowledgement  (Tracker * tracker);
    virtual ~WaitingAcknowledgement  ();
    virtual void sendMessage (LoRaClass * LoRaModule, char * messageToBeSend, NonBlockingDelay * backoffTime);
    virtual bool getAcknowledgement (LoRaClass * LoRaModule, char * messageRecieved, NonBlockingDelay * timeOut);
private:
    Tracker * tracker;
   // int IdDevice;
    //int messageNumber;
    //int connectionRetries;
    // char payload [50];
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _WAITING_ACKNOWLEDGEMENT_H_