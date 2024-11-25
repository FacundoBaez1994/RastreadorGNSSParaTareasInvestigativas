//=====[#include guards - begin]===============================================

#ifndef _RF_TRANSCIEVER_STATE_H_
#define _RF_TRANSCIEVER_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "LoRa.h"

//=====[Declaration of public data types]======================================
struct RFPacketMessage;

//=====[Declaration of public classes]=========================================
/*
 *  Interface - State desing pattern
 * 
 */
class RFTransicieverState {
public:
//=====[Declaration of public methods]=========================================
    virtual void sendMessage (LoRaClass * LoRaModule, char * messageToBeSend);
    virtual bool getAcknowledgement (LoRaClass * LoRaModule, char * messageRecieved);
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _RF_TRANSCIEVER_STATE_H_