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

void SendingMessage::addRFFormatToMessage (int deviceId, int messageNumber, char * messageToBeSend) {
    return;
}

void SendingMessage::sendMessage (LoRaClass * LoRaModule, char * messageToBeSend, NonBlockingDelay * backoffTime) {
        char buffer [256];
        uartUSB.write ("Sending plaintext message:\r\n", strlen ("Sending plaintext message:\r\n"));  // debug only
        uartUSB.write ( messageToBeSend, strlen ( messageToBeSend));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only

        if (this->tracker->prepareMessage ( messageToBeSend) == false) {
            return;
        }
        uartUSB.write ("coded message:\r\n", strlen ("coded message:\r\n"));  // debug only
        uartUSB.write ( messageToBeSend, strlen ( messageToBeSend) + 4);  // debug only
        uartUSB.write("\r\n", strlen("\r\n"));

        size_t originalLength = strlen(messageToBeSend);

        // Copiar la cadena original y agregar '|'
        strcpy(buffer, messageToBeSend);
        buffer[originalLength ] = '|';  // Agregar '|'
        buffer[originalLength + 1] = '|';      // Asegurar terminación nula
        buffer[originalLength + 2] = '\0';      // Asegurar terminación nula

        uartUSB.write("\r\n", strlen("\r\n"));
        uartUSB.write ( buffer, strlen ( buffer));  // debug only
        uartUSB.write("\r\n", strlen("\r\n"));

        size_t totalLength = strlen(buffer);
        size_t chunkSize = 5;  // Fragmentos de 50 bytes

        for (size_t i = 0; i < totalLength; i += chunkSize) {
            LoRaModule->idle();                          // set standby mode
            LoRaModule->disableInvertIQ();               // normal mode

            size_t currentChunkSize = (totalLength - i < chunkSize) ? (totalLength - i) : chunkSize;
            uartUSB.write("\r\n", strlen("\r\n"));
            uartUSB.write ( buffer + i, currentChunkSize);  // debug only
            uartUSB.write("\r\n", strlen("\r\n"));
            LoRaModule->beginPacket();
            LoRaModule->write((uint8_t*)(buffer + i), currentChunkSize);
            LoRaModule->endPacket();
          
            wait_us(3000000); // bloqueo eliminar luego!
        }


        uartUSB.write("\r\n", strlen("\r\n"));
        uartUSB.write ("Changing State to Waiting Acknowledgement:\r\n", 
        strlen ("Changing State to Waiting Acknowledgement:\r\n"));  // debug only
        uartUSB.write("\r\n", strlen("\r\n"));
        this->tracker->changeState(new WaitingAcknowledgement (this->tracker));
        return;
}

 bool SendingMessage::getAcknowledgement (LoRaClass * LoRaModule, char * messageRecieved, NonBlockingDelay * timeOut){
      return false;
 }
//=====[Implementations of private functions]==================================
