//=====[Libraries]=============================================================
#include "SendingMessage.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "WaitingAcknowledgement.h"

//=====[Declaration of private defines]========================================
#define BACKOFFTIME        250
#define MAX_CHUNK_SIZE     255
#define FLY_TIME           800
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

void SendingMessage::addRFFormatToMessage (long long int deviceId, int messageNumber, char * messageToBeSend) {
    return;
}

void SendingMessage::sendMessage (LoRaClass * LoRaModule, char * messageToBeSend, NonBlockingDelay * backoffTime) {
    //char buffer [1024] = "helloooooooooooooooooooooooooooowwwwwwwwwwhelloooooooooooooooooooowwwwwwwwwwwwwhelloooooooooooooooooooooooooooowwwwwwwwwwhelloooooooooooooooooooowwwwwwwwwwwwwhelloooooooooooooooooooooooooooowwwwwwwwwwhelloooooooooooooooooooowwwwwwwwwwwwwF-16";
    char buffer [2048];
    static bool firstChunkSent = false;
    static bool firstDelayPassed = false;
    static bool messageFormatted = false;
    static bool firstEntryOnThisMethod = true;
    static size_t stringIndex = 0;

    
    if (firstEntryOnThisMethod == true) {
        backoffTime->write(BACKOFFTIME);
        backoffTime->restart();
        firstEntryOnThisMethod = false;
    }


    if (firstDelayPassed == false) {
        if (backoffTime->read()) {
            firstDelayPassed = true;
            uartUSB.write("Backoff passed\r\n", strlen("Backoff passed\r\n"));
        } else {
            return;
        }
    }

    if (messageFormatted == false) {
        uartUSB.write ("Sending plaintext message:\r\n", strlen ("Sending plaintext message:\r\n"));  // debug only
        uartUSB.write ( messageToBeSend, strlen ( messageToBeSend));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        strcpy(buffer, messageToBeSend);
        if (this->tracker->prepareMessage ( buffer, strlen (buffer)) == false) {
            return;
        }
        uartUSB.write ("coded message:\r\n", strlen ("coded message:\r\n"));  // debug only
        uartUSB.write ( buffer, strlen ( buffer));  // debug only
        uartUSB.write("\r\n", strlen("\r\n"));

        size_t originalLength = strlen(buffer);

        // Copiar la cadena original y agregar '|'
        
        buffer[originalLength ] = '|';  // Agregar '|'
        buffer[originalLength + 1] = '|';      // Asegurar terminación nula
        buffer[originalLength + 2] = '\0';      // Asegurar terminación nula

        uartUSB.write("\r\n", strlen("\r\n"));
        uartUSB.write ( buffer, strlen ( buffer));  // debug only
        uartUSB.write("\r\n", strlen("\r\n"));
        messageFormatted = true; 
    }


    if (backoffTime->read() || firstChunkSent == false) {
        firstChunkSent = true;
        backoffTime->write(FLY_TIME);
        backoffTime->restart();

        size_t totalLength = strlen(buffer);
        size_t chunkSize = MAX_CHUNK_SIZE;  // Fragmentos de 50 bytes
        LoRaModule->idle();                          // set standby mode
        LoRaModule->disableInvertIQ();               // normal mode
        size_t currentChunkSize = (totalLength - stringIndex < chunkSize) ? (totalLength - stringIndex) : chunkSize;
        uartUSB.write("\r\n", strlen("\r\n"));
        uartUSB.write ( buffer, strlen (buffer));  // debug only
        uartUSB.write("\r\n", strlen("\r\n"));
        LoRaModule->beginPacket();
        LoRaModule->write((uint8_t*)(buffer + stringIndex), currentChunkSize);
        //LoRaModule->write(reinterpret_cast<const uint8_t*>(buffer), strlen(buffer));

        //LoRaModule->write((uint8_t*)(buffer), strlen (buffer));
        LoRaModule->endPacket();
        stringIndex += chunkSize;
        if (stringIndex  > totalLength) {
            uartUSB.write("\r\n", strlen("\r\n"));
            uartUSB.write ("Changing State to Waiting Acknowledgement:\r\n", 
            strlen ("Changing State to Waiting Acknowledgement:\r\n"));  // debug only
            uartUSB.write("\r\n", strlen("\r\n"));
            firstDelayPassed = false;
            messageFormatted = false;
            firstEntryOnThisMethod = true;
            messageFormatted = false; 
            stringIndex = 0;
            this->tracker->changeState(new WaitingAcknowledgement (this->tracker));
            return;
        }
    }
}

 bool SendingMessage::getAcknowledgement (LoRaClass * LoRaModule, char * messageRecieved, NonBlockingDelay * timeOut){
      return false;
 }
//=====[Implementations of private functions]==================================
