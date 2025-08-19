//=====[#include guards - begin]===============================================

#ifndef _ADDING_RF_FORMAT_H_
#define _ADDING_RF_FORMAT_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "Non_Blocking_Delay.h"
#include "RFTransicieverState.h"
#include "string.h"

//=====[Declaration of public data types]======================================
class Tracker; //debido a declaracion adelantada
//struct TcpSocket;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class AddingRFFormat : public RFTransicieverState {
public:
//=====[Declaration of public methods]=========================================
    AddingRFFormat  (Tracker * tracker);
    virtual ~AddingRFFormat ();
    virtual void addRFFormatToMessage (long long int deviceId, int messageNumber, char * messageToBeSend);
    virtual void sendMessage (LoRaClass * LoRaModule, char * messageToBeSend, NonBlockingDelay * backoffTime);
    virtual bool getAcknowledgement (LoRaClass * LoRaModule, char * messageRecieved, NonBlockingDelay * timeOut);
private:
    Tracker * tracker;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _ADDING_RF_FORMAT_H_