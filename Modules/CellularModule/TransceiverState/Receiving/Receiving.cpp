//=====[Libraries]=============================================================

#include "Receiving.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20
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
Receiving::Receiving () {
    this->mobileNetworkModule = NULL;
}


/** 
* @brief
* 
* @param 
*/
Receiving::Receiving (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
}


/** 
* @brief 
* 
* 
* @returns 
*/
Receiving::~Receiving () {
    this->mobileNetworkModule = NULL;
}


void Receiving::enableTransceiver () {
    return;
}


/** 
* @brief 
* 
* 
* @returns 
*/
 CellularTransceiverStatus_t  Receiving::exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool newDataAvailable) {

    char ATcommandFirstPart [15] = "AT+QIRD= ";
    char StringToBeSend [16];
    char StringToBeSendUSB [20]  = "RECV Data";
    char noDataResponse [15] = "+QIRD: 0";
    char expectedResponse [15] = "OK";

    char retrivedMessage [200];
    char StringToBeRead [200];

    static bool readyToSend = false;
    static int attempts = 0; 
    static int maxConnectionAttempts = MAXATTEMPTS; 
    static bool dataRetrieved;

   
    //char protocol[] = "\"TCP\"";
    int noErrorCode = 0;
    int contextID = 1; // Usualmente 1
    int connectID = 0; // Puede ser entre 0 y 11
    int access_mode = 0; // Modo de acceso al buffer


    snprintf(StringToBeSend, sizeof(StringToBeSend ), "%s%d", ATcommandFirstPart, connectID);
    if (readyToSend == true) {
        ATHandler->sendATCommand(StringToBeSend);
        readyToSend  = false;
        ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToBeSendUSB , strlen (StringToBeSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToBeSend  , strlen (StringToBeSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ////   ////   ////   ////   ////   ////   
    }

    if ( ATHandler->readATResponse ( StringToBeRead) == true) {
        uartUSB.write (StringToBeRead  , strlen (StringToBeRead  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        if (dataRetrieved == true) {
            if (strcmp (StringToBeRead, expectedResponse) == 0) {

                newDataAvailable = true; 

                attempts = 0;
                char StringToSendUSB [40] = "Cambiando de estado 3";
                uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                this->mobileNetworkModule->changeTransceiverState
                 (new CloseSocket (this->mobileNetworkModule, true));
                return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
            }

        }
        if ( strcmp (StringToBeRead, noDataResponse) == 0) {
            // No data recv keep trying 
            attempts++;
            readyToSend  = false;
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
        }
        if (retrivMessage(StringToBeRead, retrivedMessage) == true ) {
            dataRetrieved = true;
            ////   ////   ////   ////   ////   ////    
            this->mobileNetworkModule->changeTransceiverState (new Sending (this->mobileNetworkModule));
        }
    }

    if (refreshTime->read()) {
        readyToSend = true;
        attempts++;
        if (attempts >= maxConnectionAttempts) {
            attempts = 0;
             this->mobileNetworkModule->changeTransceiverState (new CloseSocket (this->mobileNetworkModule, true));
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
        }
    }
    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
}


//=====[Implementations of private functions]==================================
bool Receiving::retrivMessage(char * response, char * retrivMessage) {
    char expectedFistPartResponse[15] = "+QIRD: ";
    
    // Verificar que la respuesta sea del tipo esperado
    if (strncmp(response, expectedFistPartResponse, strlen(expectedFistPartResponse)) == 0) {
        
        // Encontrar la posición después de '+QIRD: '
        char * pos = response + strlen(expectedFistPartResponse);
        
        // Parsear los primeros tres campos: read_actual_length, remoteIP, remote_port
        int read_actual_length;
        char remoteIP[16]; // Suponemos IPv4
        int remote_port;
        
        // Formato esperado: <read_actual_length>,<remoteIP>,<remote_port>
        // Ejemplo: +QIRD: 45,"192.168.1.10",12345
        if (sscanf(pos, "%d,\"%15[^\"]\",%d", &read_actual_length, remoteIP, &remote_port) == 3) {
            
            // Buscar el inicio del <data> que está después de un <CR><LF>
            char * dataStart = strstr(response, "\r\n");
            if (dataStart != nullptr) {
                dataStart += 2; // Saltar <CR><LF>
                
                // Copiar el mensaje de datos a retrivMessage
                strncpy(retrivMessage, dataStart, read_actual_length);
                retrivMessage[read_actual_length] = '\0'; // Añadir terminador de cadena
                
                return true; // Mensaje parseado correctamente
            }
        }
    }

    return false; // Si no coincide o hay algún error en el parseo
}
