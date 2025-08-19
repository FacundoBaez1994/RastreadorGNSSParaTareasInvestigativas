//=====[#include guards - begin]===============================================

#ifndef _SENDING_MESSAGE_H_
#define _SENDING_MESSAGE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "Non_Blocking_Delay.h"
#include "RFTransicieverState.h"
//#include "WaitingAcknowledgement.h"

//=====[Declaration of public data types]======================================
class Tracker; //debido a declaracion adelantada
//struct TcpSocket;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class SendingMessage : public RFTransicieverState {
public:
//=====[Declaration of public methods]=========================================
    SendingMessage  (Tracker * tracker);
    virtual ~SendingMessage  ();
    virtual void addRFFormatToMessage (long long int deviceId, int messageNumber, char * messageToBeSend);
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

#endif //  _SENDING_MESSAGE_H_