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

void AddingRFFormat::addRFFormatToMessage (int deviceId, int messageNumber, char * messageToBeSend) {
    char tempBuffer[256];
    char trialMessage [7] = "hello"; // ELIMINAR CUANDO SE HAGA MERGE CON RUTINA PRINCIPAL

    // Limpiar el contenido del buffer
    std::fill_n(messageToBeSend, 256, '\0'); // Cambia a fill_n para limpiar el buffer dinámico

   // snprintf(tempBuffer, sizeof(tempBuffer), "%d,%d,%s", deviceId, messageNumber, messageToBeSend); USAR EN RUTINA PRINCIPAL
    snprintf(tempBuffer, sizeof(tempBuffer), "%d,%d,%s", deviceId, messageNumber, trialMessage); // ELIMINAR CUANDO SE HAGA MERGE CON RUTINA PRINCIPAL
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
