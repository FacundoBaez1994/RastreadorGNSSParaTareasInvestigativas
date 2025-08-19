//=====[Libraries]=============================================================
#include "SendingMessage.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "AddingRFFormat.h"

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
AddingRFFormat::AddingRFFormat (Tracker * tracker) {
    this->tracker = tracker;
}


/** 
* @brief
* 
* @param 
*/
AddingRFFormat::~AddingRFFormat () {
     this->tracker = NULL;
}

void AddingRFFormat::addRFFormatToMessage (long long int deviceId, int messageNumber, char * messageToBeSend) {
    char tempBuffer[1024];
    char trialMessage [1024] = "PARK,2,88,3,1.1,0,2.8,-112.33,4.44,71"; // ELIMINAR CUANDO SE HAGA MERGE CON RUTINA PRINCIPAL

   // snprintf(tempBuffer, sizeof(tempBuffer), "%d,%d,%s", deviceId, messageNumber, messageToBeSend); USAR EN RUTINA PRINCIPAL
    snprintf(tempBuffer, sizeof(tempBuffer), "LORALORA,%lld,%d,%s", deviceId, messageNumber, trialMessage); // ELIMINAR CUANDO SE HAGA MERGE CON RUTINA PRINCIPAL
    strcpy(messageToBeSend, tempBuffer);

    uartUSB.write("Sending Message:\r\n", strlen("Sending Message:\r\n"));
    uartUSB.write(messageToBeSend, strlen(messageToBeSend));
    uartUSB.write("\r\n", strlen("\r\n"));
    uartUSB.write("Changing To  SendingMessage State\r\n", strlen("Changing To  SendingMessage State\r\n"));
    this->tracker->changeState(new SendingMessage (this->tracker));
    return;
}

void AddingRFFormat::sendMessage (LoRaClass * LoRaModule, char * messageToBeSend, NonBlockingDelay * backoffTime) {
    return;
}

 bool AddingRFFormat::getAcknowledgement (LoRaClass * LoRaModule, char * messageRecieved, NonBlockingDelay * timeOut){
      return false;
 }
//=====[Implementations of private functions]==================================
