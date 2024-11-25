//=====[Libraries]=============================================================
#include "SendingMessage.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "WaitingAcknowledgement.h"

//=====[Declaration of private defines]========================================
#define MAX_RETRIES 3
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============




//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of private methods]===================================
/** 
* @brief attachs the callback function to the ticker
*/


//=====[Implementations of public methods]===================================
/** 
* @brief
* 
* @param 
*/
SendingMessage::SendingMessage (Tracker * tracker) {
    this->tracker = tracker;
}


/** 
* @brief
* 
* @param 
*/
SendingMessage::~SendingMessage() {
     this->tracker = NULL;
}

void SendingMessage::sendMessage (LoRaClass * LoRaModule, char * messageToBeSend) {
    uartUSB.write ("Sending message:\r\n", strlen ("Sending message:\r\n"));  // debug only
    uartUSB.write (messageToBeSend, strlen (messageToBeSend));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only
    LoRaModule->idle();                          // set standby mode
    LoRaModule->disableInvertIQ();               // normal mode
    LoRaModule->beginPacket();
    LoRaModule->write((uint8_t *)messageToBeSend, strlen(messageToBeSend));
    LoRaModule->endPacket();

    uartUSB.write ("Changing State to Waiting Acknowledgement:\r\n", 
    strlen ("Changing State to Waiting Acknowledgement:\r\n"));  // debug only
    this->tracker->changeState(new WaitingAcknowledgement (this->tracker));
    return;
}

 bool SendingMessage::getAcknowledgement (LoRaClass * LoRaModule, char * messageRecieved){
      return false;
 }
//=====[Implementations of private functions]==================================
