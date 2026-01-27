//=====[Libraries]=============================================================
#include "ExchangingLoRaMessages.h"
#include "Non_Blocking_Delay.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "GoingToSleep.h"
#include "SavingMessage.h"
#include "FormattingMessage.h"
#include "GatheringInertialData.h"

//=====[Declaration of private defines]========================================
#define BACKOFFTIME        25
#define MAX_CHUNK_SIZE     255
#define FLY_TIME           800
#define TIMEOUT          5000
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
ExchangingLoRaMessages::ExchangingLoRaMessages (Tracker * tracker, trackerStatus_t trackerStatus) {
    this->tracker = tracker;
    this->currentStatus = trackerStatus;
    this->backoffTime = new NonBlockingDelay (BACKOFFTIME);
    this->currentExchangingLoRaMessagesStatus = INITIALIZE_TRANSCIEVER;
}

ExchangingLoRaMessages::~ExchangingLoRaMessages() {
     this->tracker = nullptr;
     delete this->backoffTime;
     this->backoffTime = nullptr;
}

void ExchangingLoRaMessages::exchangeMessages (LoRaClass * LoRaModule, char * message, char * receivedMessage ) {
    //char buffer [1024] = "helloooooooooooooooooooooooooooowwwwwwwwwwhelloooooooooooooooooooowwwwwwwwwwwwwhelloooooooooooooooooooooooooooowwwwwwwwwwhelloooooooooooooooooooowwwwwwwwwwwwwhelloooooooooooooooooooooooooooowwwwwwwwwwhelloooooooooooooooooooowwwwwwwwwwwwwF-16";
    Watchdog &watchdog = Watchdog::get_instance(); // singletom
    char buffer [2100];
    static bool firstChunkSent = false;
    //static bool firstDelayPassed = false;
    static size_t stringIndex = 0;

    char logMessage [100];
    static char processedMessageReceived  [2100];
    static bool messageReceived = false; 
    static std::vector<char> accumulatedBuffer; // Acumulador de fragmentos
    static std::string fullMessage;
    static int stringInsertCount = 0;
    static int timeoutCounter = 0;

    //uartUSB.write ("switch\r\n", strlen ("switch\r\n"));

    switch ( this->currentExchangingLoRaMessagesStatus) {
    case  INITIALIZE_TRANSCIEVER:
        watchdog.kick();
        if (!LoRaModule->begin (915E6)) {
            uartUSB.write ("LoRa Module Failed to Start!", strlen ("LoRa Module Failed to Start"));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            // AGREGAR contandor si supera limite mandarlo a guardar el mensaje en la EEPROM
            
            return;
        }
        strcpy(buffer, message);
        this->backoffTime->write(BACKOFFTIME);
        this->backoffTime->restart();
        this->currentExchangingLoRaMessagesStatus = SENDING_MESSAGE;
        break;   

    case  SENDING_MESSAGE:

        watchdog.kick();
        if (this->backoffTime->read() || firstChunkSent == false) {
            uint8_t testPayload[32];
            for (int i = 0; i < 32; i++) {
                testPayload[i] = 0xAA;
            }



            firstChunkSent = true;
            this->backoffTime->write(FLY_TIME);
            this->backoffTime->restart();

            //size_t totalLength = strlen(testPayload);
            //size_t chunkSize = MAX_CHUNK_SIZE;  // Fragmentos de 50 bytes
            LoRaModule->idle();                          // set standby mode
            LoRaModule->disableInvertIQ();               // normal mode
            //size_t currentChunkSize = (totalLength - stringIndex < chunkSize) ? (totalLength - stringIndex) : chunkSize;
            uartUSB.write("\r\n", strlen("\r\n"));
            uartUSB.write ( buffer, strlen (buffer));  // debug only
            uartUSB.write("\r\n", strlen("\r\n"));
            LoRaModule->beginPacket();
            LoRaModule->write(testPayload, sizeof (testPayload));
            //LoRaModule->write(reinterpret_cast<const uint8_t*>(buffer), strlen(buffer));

            //LoRaModule->write((uint8_t*)(buffer), strlen (buffer));
            LoRaModule->endPacket();
            uartUSB.write ("PacketSent\r\n", strlen ("PacketSent\r\n"));
            watchdog.kick();

            /*
            stringIndex += chunkSize;
            if (stringIndex  > totalLength) {
                uartUSB.write("\r\n", strlen("\r\n"));
                uartUSB.write ("Waiting Acknowledgement:\r\n", 
                strlen ("Waiting Acknowledgement:\r\n"));  // debug only
                uartUSB.write("\r\n", strlen("\r\n"));
                //firstDelayPassed = false;
            // messageFormatted = false; 
                firstChunkSent = false; 

                this->backoffTime->write(TIMEOUT);
                this->backoffTime->restart();
                stringIndex = 0;
                this->currentExchangingLoRaMessagesStatus = WAITING_FOR_ACK;
                //this->tracker->changeState(new WaitingAcknowledgement (this->tracker));
                return;
            }*/
        }
        break;

    case WAITING_FOR_ACK:
    watchdog.kick();
    if (messageReceived  == false) {
        LoRaModule->enableInvertIQ();    
        int packetSize = LoRaModule->parsePacket();
        if (packetSize) {
            uartUSB.write("Packet Received!\r\n", strlen("Packet Received!\r\n")); // Debug

             //restart timeOut Timer
             /*
            if (firstPacketReceived == false) {
                timeOut->restart ();
                uartUSB.write("time out restart\r\n", strlen("time out restart\r\n")); // Debug
                firstPacketReceived = true;
            } */

            int maxIterations = 100; // Límite para evitar un ciclo infinito
            int iterations = 0;

            // Leer los datos disponibles
            
            while (LoRaModule->available() > 0 && iterations < maxIterations) {
                ssize_t bytesRead = LoRaModule->read(reinterpret_cast<uint8_t*>(buffer), sizeof(buffer));
               // ssize_t bytesRead = LoRaModule->read(reinterpret_cast<uint8_t*>(buffer), strlen (buffer));
                if (bytesRead > 0) {
                    // Enviar los bytes leídos al puerto serie
                   // uartUSB.write(buffer, bytesRead);
                    uartUSB.write(buffer, strlen(buffer));
                    uartUSB.write ("\r\n", strlen("\r\n"));
                    accumulatedBuffer.insert(accumulatedBuffer.end(), buffer, buffer + bytesRead);
                    stringInsertCount ++;

                    if (stringInsertCount > 51) {
                        // Eliminar todos los elementos del vector
                        accumulatedBuffer.clear();
                        stringInsertCount = 0;

                        // Acción adicional, si es necesario
                        uartUSB.write("Buffer cleared size reach limit\r\n", strlen("Buffer cleared size reach limit\r\n"));
                        messageReceived = false;
                        return;
                    }

                    // Buscar el delimitador `|`
                    auto delimiterPos = std::search(accumulatedBuffer.begin(), accumulatedBuffer.end(), "||", "||" + 2);
                    if (delimiterPos != accumulatedBuffer.end()) {
                        // Reconstruir el mensaje completo
                        fullMessage.assign(accumulatedBuffer.begin(), delimiterPos);
                        accumulatedBuffer.erase(accumulatedBuffer.begin(), delimiterPos + 2); // Eliminar procesado
                        stringInsertCount = 0;

                        // Debug del mensaje completo
                        uartUSB.write("Full Message: ", strlen("Full Message: "));
                        uartUSB.write(fullMessage.c_str(), fullMessage.length());
                        uartUSB.write("\r\n", strlen("\r\n"));
                        messageReceived = true;

                        int packetRSSI = LoRaModule->packetRssi();
                        snprintf(logMessage, sizeof(logMessage), "packet RSSI: %d\r\n", packetRSSI);
                        uartUSB.write(logMessage, strlen(logMessage));
                    }
                    iterations++;
                    if (iterations >= maxIterations) {
                        uartUSB.write("Warning: Exceeded max iterations\r\n", strlen("Warning: Exceeded max iterations\r\n"));
                        messageReceived = false;
                        return;
                    }
                } //  if (bytesRead > 0) end
            } // while (LoRaModule->available() > 0 && iterations < maxIterations)  end
        } //  if (packetSize)  end
    }  else {   // messageReceived  == false if end
        if (fullMessage.empty()) {
            accumulatedBuffer.clear();
            messageReceived = false;
            uartUSB.write("Fail to process received message\r\n", strlen("Fail to process received message\r\n"));
            return;
        }

        const char* constCharPtr = fullMessage.c_str(); 
        strncpy(processedMessageReceived, constCharPtr, sizeof(processedMessageReceived) - 1);
        processedMessageReceived[sizeof(processedMessageReceived) - 1] = '\0';

        if (this->tracker->processLoRaMessage(processedMessageReceived, sizeof (processedMessageReceived) ) == false) {
            uartUSB.write("Fail to process received message\r\n", strlen("Fail to process received message\r\n")); // Debug
            messageReceived = false;
            return;
        }
        processedMessageReceived;
        uartUSB.write("Recepted message\r\n", strlen("Recepted message\r\n"));
        uartUSB.write( processedMessageReceived, strlen( processedMessageReceived));
        messageReceived  = false;
        accumulatedBuffer.clear(); // Elimina todos los elementos del vector
        stringInsertCount = 0;
        fullMessage.clear();       // Elimina todo el contenido de la cadena
      
        if (this->tracker->checkMessageIntegrity ( processedMessageReceived)) {
             //wait_us(3000000); //backoff Nota este back eliminalo luego porque con la rutina principal no tiene sentido
            uartUSB.write("ACK OK!!\r\n", strlen("ACK OK!!\r\n"));;
            //firstPacketReceived = false;
            this->backoffTime->write(BACKOFFTIME);
             this->backoffTime->restart();
             strcpy(buffer, message);
            this->currentExchangingLoRaMessagesStatus = SENDING_MESSAGE;
            this->tracker->changeState  (new GoingToSleep (this->tracker));
            return;
        }
    }

    if (this->backoffTime->read()) {
        accumulatedBuffer.clear();
        fullMessage.clear();
        stringInsertCount = 0;
        messageReceived = false;

        uartUSB.write ("Timeout for ACK\r\n", strlen ("Timeout for ACK\r\n"));  // debug only
        uartUSB.write("Changing To AddingRFFormat\r\n", strlen("Changing To AddingRFFormat\r\n"));
        //firstPacketReceived = false;
        this->backoffTime->write(BACKOFFTIME);
        this->backoffTime->restart();
        strcpy(buffer, message);
        this->currentExchangingLoRaMessagesStatus = SENDING_MESSAGE;
        timeoutCounter++;
        if (timeoutCounter >= 5) {
            uartUSB.write ("LoRa Unavailable\r\n" , strlen ("LoRa Unavailable\r\n"));  // debug only
            if (this->currentStatus == TRACKER_STATUS_GNSS_OBTAIN_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_TRYING_LORA ){
                timeoutCounter = 0;
                this->tracker->changeState (new FormattingMessage (this->tracker, TRACKER_STATUS_GNSS_OBTAIN_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_LORA_UNAVAILABLE_SAVING_MESSAGE));
                return;
            }
            timeoutCounter = 0;
            this->tracker->changeState (new GatheringInertialData (this->tracker, TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_LORA_UNAVAILABLE_GATHERING_INERTIAL_INFO));
            return;
        }
    }

        break;
    default:
        return;
    }

}




//=====[Implementations of private functions]==================================
