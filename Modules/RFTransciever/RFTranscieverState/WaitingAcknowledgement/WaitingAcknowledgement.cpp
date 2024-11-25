//=====[Libraries]=============================================================
#include "WaitingAcknowledgement.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

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
WaitingAcknowledgement::WaitingAcknowledgement (Tracker * tracker) {
    this->tracker = tracker;
}


/** 
* @brief
* 
* @param 
*/
WaitingAcknowledgement::~WaitingAcknowledgement() {
     this->tracker = NULL;
}

void WaitingAcknowledgement::sendMessage (LoRaClass * LoRaModule, char * messageToBeSend) {
    return; 
}

 bool WaitingAcknowledgement::getAcknowledgement (LoRaClass * LoRaModule, char * messageRecieved){
    char logMessage [50];
    char ACKMessage [64];

    LoRaModule->enableInvertIQ();    
    int packetSize = LoRaModule->parsePacket();
    if (packetSize) {
        uartUSB.write("Packet Received!\r\n", strlen("Packet Received!\r\n")); // Debug

        int maxIterations = 100; // Límite para evitar un ciclo infinito
        int iterations = 0;

        // Leer los datos disponibles
        while (LoRaModule->available() > 0 && iterations < maxIterations) {
            ssize_t bytesRead = LoRaModule->read(reinterpret_cast<uint8_t*>( ACKMessage), sizeof( ACKMessage));
            if (bytesRead > 0) {
                // Enviar los bytes leídos al puerto serie
                strcpy (messageRecieved, ACKMessage);
                 uartUSB.write(messageRecieved, bytesRead);
                if (bytesRead < sizeof( ACKMessage)) {
                    messageRecieved[bytesRead] = '\0'; // Asegurar el null-terminator
                } else {
                    messageRecieved[sizeof( ACKMessage) - 1] = '\0'; // Evitar overflow
                }
            }
            iterations++;
        }
        if (iterations >= maxIterations) {
            uartUSB.write("Warning: Exceeded max iterations\r\n", strlen("Warning: Exceeded max iterations\r\n"));
        }
        uartUSB.write ("\n", strlen("\n"));
        int packetRSSI = LoRaModule->packetRssi();
        snprintf(logMessage, sizeof(logMessage), "packet RSSI: %d\r\n", packetRSSI);
        uartUSB.write(logMessage, strlen(logMessage));

        this->tracker->changeState(new SendingMessage (this->tracker));
        return true;
    }
    return false;
 }
//=====[Implementations of private functions]==================================
