//=====[Libraries]=============================================================
#include "ExchangingLoRaMessages.h"
#include "Non_Blocking_Delay.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

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
ExchangingLoRaMessages::ExchangingLoRaMessages (Tracker * tracker, trackerStatus_t trackerStatus) {
    this->tracker = tracker;
    this->currentStatus = trackerStatus;
    this->backoffTime = new NonBlockingDelay (BACKOFFTIME);
}


/** 
* @brief
* 
* @param 
*/
ExchangingLoRaMessages::~ExchangingLoRaMessages() {
     this->tracker = nullptr;
     delete this->backoffTime;
     this->backoffTime = nullptr;
}


void ExchangingLoRaMessages::exchangeMessages (LoRaClass * LoRaModule, char * message, char * receivedMessage ) {
    //char buffer [1024] = "helloooooooooooooooooooooooooooowwwwwwwwwwhelloooooooooooooooooooowwwwwwwwwwwwwhelloooooooooooooooooooooooooooowwwwwwwwwwhelloooooooooooooooooooowwwwwwwwwwwwwhelloooooooooooooooooooooooooooowwwwwwwwwwhelloooooooooooooooooooowwwwwwwwwwwwwF-16";
    char buffer [2048];
    static bool firstChunkSent = false;
    static bool firstDelayPassed = false;
    static bool messageFormatted = false;
    static bool firstEntryOnThisMethod = true;
    static size_t stringIndex = 0;

    
    if (firstEntryOnThisMethod == true) {
        if (!LoRaModule->begin (915E6)) {
            uartUSB.write ("LoRa Module Failed to Start!", strlen ("LoRa Module Failed to Start"));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            return;
        }
        this->backoffTime->write(BACKOFFTIME);
        this->backoffTime->restart();
        firstEntryOnThisMethod = false;
    }

    if (firstDelayPassed == false) {
        if (this->backoffTime->read()) {
            firstDelayPassed = true;
            uartUSB.write("Backoff passed\r\n", strlen("Backoff passed\r\n"));
        } else {
            return;
        }
    }

    if (messageFormatted == false) {
        uartUSB.write ("Sending plaintext message:\r\n", strlen ("Sending plaintext message:\r\n"));  // debug only
        uartUSB.write ( message, strlen ( message));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        strcpy(buffer, message);
        if (this->tracker->prepareLoRaMessage ( buffer, strlen (buffer)) == false) {
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


    if (this->backoffTime->read() || firstChunkSent == false) {
        firstChunkSent = true;
        this->backoffTime->write(FLY_TIME);
        this->backoffTime->restart();

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
            //this->tracker->changeState(new WaitingAcknowledgement (this->tracker));
            return;
        }
    }
}




/*

bool WaitingAcknowledgement::getAcknowledgement (LoRaClass * LoRaModule, char * messageRecieved,
 NonBlockingDelay * timeOut){
    static bool messageReceived = false; 
    static std::vector<char> accumulatedBuffer; // Acumulador de fragmentos
    static std::string fullMessage;

    char processedMessageReceived  [2048];
    static char buffer[2048] = {0};
    static char message[1024];

    int deviceId = 0;
    int messageNumber = 0;

    static bool firstPacketReceived = false;
    static bool firstEntryOnThisMethod = true;
    static int stringInsertCount = 0;

    uint8_t receivedBuffer[64];

    if (firstEntryOnThisMethod == true) {
        timeOut->write(TIMEOUT);
        timeOut->restart();
        uartUSB.write("time out restart\r\n", strlen("time out restart\r\n")); // Debug
        firstEntryOnThisMethod = false;
    }

    if (messageReceived  == false) {
        LoRaModule->enableInvertIQ();    
        int packetSize = LoRaModule->parsePacket();
        if (packetSize) {
            uartUSB.write("Packet Received!\r\n", strlen("Packet Received!\r\n")); // Debug

             //restart timeOut Timer
            if (firstPacketReceived == false) {
                timeOut->restart ();
                uartUSB.write("time out restart\r\n", strlen("time out restart\r\n")); // Debug
                firstPacketReceived = true;
            }

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
                        return false;
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
                        snprintf(message, sizeof(message), "packet RSSI: %d\r\n", packetRSSI);
                        uartUSB.write(message, strlen(message));
                    }
                    iterations++;
                    if (iterations >= maxIterations) {
                        uartUSB.write("Warning: Exceeded max iterations\r\n", strlen("Warning: Exceeded max iterations\r\n"));
                        messageReceived = false;
                        return false;
                    }
                } //  if (bytesRead > 0) end
            } // while (LoRaModule->available() > 0 && iterations < maxIterations)  end
        } //  if (packetSize)  end
    }  else {   // messageReceived  == false if end
        if (fullMessage.empty()) {
            accumulatedBuffer.clear();
            messageReceived = false;
            uartUSB.write("Fail to process received message\r\n", strlen("Fail to process received message\r\n"));
            return false;
        }

        const char* constCharPtr = fullMessage.c_str(); 
        strncpy(processedMessageReceived, constCharPtr, sizeof(processedMessageReceived) - 1);
        processedMessageReceived[sizeof(processedMessageReceived) - 1] = '\0';

        if (this->tracker->processMessage(processedMessageReceived, sizeof (processedMessageReceived) ) == false) {
            uartUSB.write("Fail to process received message\r\n", strlen("Fail to process received message\r\n")); // Debug
            messageReceived = false;
            return false;
        }
        strcpy ( messageRecieved, processedMessageReceived);
        uartUSB.write("Recepted message\r\n", strlen("Recepted message\r\n"));
        uartUSB.write(messageRecieved, strlen(messageRecieved));
        messageReceived  = false;
        accumulatedBuffer.clear(); // Elimina todos los elementos del vector
        stringInsertCount = 0;
        fullMessage.clear();       // Elimina todo el contenido de la cadena
      
        if (this->tracker->checkMessageIntegrity (messageRecieved)) {
             wait_us(3000000); //backoff Nota este back eliminalo luego porque con la rutina principal no tiene sentido
            uartUSB.write("Changing To AddingRFFormat\r\n", strlen("Changing To AddingRFFormat\r\n"));
            firstEntryOnThisMethod  = true;
            firstPacketReceived = false;
            this->tracker->changeState(new AddingRFFormat (this->tracker));
            return true;
        }
    }

    if (timeOut->read() && firstEntryOnThisMethod == false) {
        uartUSB.write ("Timeout for ACK\r\n", strlen ("Timeout for ACK\r\n"));  // debug only
        uartUSB.write("Changing To AddingRFFormat\r\n", strlen("Changing To AddingRFFormat\r\n"));
        firstEntryOnThisMethod  = true;
        firstPacketReceived = false;
        this->tracker->changeState(new AddingRFFormat (this->tracker));
    }
    return false;
}
*/
//=====[Implementations of private functions]==================================
