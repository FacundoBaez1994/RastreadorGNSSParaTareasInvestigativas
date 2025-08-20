//=====[#include guards - begin]===============================================

#ifndef _EXCHANGING_LORA_MESSAGE_H_
#define _EXCHANGING_LORA_MESSAGE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "Non_Blocking_Delay.h"
#include "TrackerBaseState.h"
#include "Tracker.h"
//#include "WaitingAcknowledgement.h"

//=====[Declaration of public data types]======================================
class Tracker; //debido a declaracion adelantada
//struct TcpSocket;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class ExchangingLoRaMessages  : public TrackerBaseState {
public:
//=====[Declaration of public methods]=========================================
    ExchangingLoRaMessages  (Tracker * tracker, trackerStatus_t trackerStatus);
    virtual ~ExchangingLoRaMessages  ();

    void exchangeMessages (LoRaClass * LoRaModule, char * message, char * receivedMessage );
private:
    Tracker * tracker;
    trackerStatus_t currentStatus; ///< Current tracker status
    NonBlockingDelay * backoffTime;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _EXCHANGING_LORA_MESSAGE_H_