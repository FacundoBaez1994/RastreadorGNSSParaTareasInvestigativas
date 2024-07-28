//=====[Libraries]=============================================================

#include "ConsultingAvailableOperators.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart 

//=====[Declaration of private defines]========================================

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
ConsultingAvailableOperators::ConsultingAvailableOperators (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->operatorsInformationRetrived = false;
    this->readyToSend = true;
}


/** 
* @brief 
* 
* 
* @returns 
*/
ConsultingAvailableOperators::~ConsultingAvailableOperators () {
    this->mobileNetworkModule = NULL;
    this->currentOperator = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
void ConsultingAvailableOperators::connect (ATCommandHandler * ATHandler, NonBlockingDelay * refreshTime) {

    static char StringToBeRead [256];
    char ExpectedResponse [15] = "OK";
    char StringToSend [15] = "AT+QNWINFO";
    char StringToSendUSB [40] = "CONSULTING AVAILABLE OPERATORS";

    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToSend);
        this->readyToSend = false;
        ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        refreshTime->restart();
        ////   ////   ////   ////   ////   ////   
    }

    if ( this->operatorsInformationRetrived == false) {
        if ( ATHandler->readATResponse ( StringToBeRead) == true ) {
        
            ////   ////   ////   ////   ////   ////
            uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////
             refreshTime->restart();
            if (this->retrivOperatorsInformation (StringToBeRead)) {
                this->operatorsInformationRetrived = true;
           }
        } 
    } 

    if (this->operatorsInformationRetrived  == true) {
        if  (ATHandler->readATResponse ( StringToBeRead) == true) {
            if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////     
                char StringToSendUSB [40] = "Cambiando de estado 5";
                uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////            
                this->mobileNetworkModule->changeConnectionState (new RetrievingTimeAndDate (this->mobileNetworkModule));
            }
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->operatorsInformationRetrived = false;
    }

}

//=====[Implementations of private functions]==================================
bool ConsultingAvailableOperators::retrivOperatorsInformation(char *response) {
    char StringToCompare[10] = "+QNWINFO:";

    if (strncmp(response, StringToCompare, strlen(StringToCompare)) == 0) {
        char accessTechnology[20] = {0};
        char operatorCode[10] = {0};
        char band[20] = {0};
        int channel;
        
        // Parsear la respuesta para obtener MCC, MNC, tecnología de acceso, banda y canal
        int n = sscanf(response, "+QNWINFO: \"%19[^\"]\",\"%9[^\"]\",\"%19[^\"]\",%d", accessTechnology, operatorCode, band, &channel);

        if (n == 4) {
            // Extraer MCC y MNC del operatorCode
            char mcc[4] = {0};
            char mnc[4] = {0};

            strncpy(mcc, operatorCode, 3);
            strncpy(mnc, operatorCode + 3, 3);

            // Enviar la información por UART para verificación
            uartUSB.write("Access Technology: ", strlen("Access Technology: "));
            uartUSB.write(accessTechnology, strlen(accessTechnology));
            uartUSB.write("\r\n", 2);

            uartUSB.write("MCC: ", strlen("MCC: "));
            uartUSB.write(mcc, strlen(mcc));
            uartUSB.write("\r\n", 2);

            uartUSB.write("MNC: ", strlen("MNC: "));
            uartUSB.write(mnc, strlen(mnc));
            uartUSB.write("\r\n", 2);

            uartUSB.write("Band: ", strlen("Band: "));
            uartUSB.write(band, strlen(band));
            uartUSB.write("\r\n", 2);

            char channelStr[10];
            sprintf(channelStr, "%d", channel);
            uartUSB.write("Channel: ", strlen("Channel: "));
            uartUSB.write(channelStr, strlen(channelStr));
            uartUSB.write("\r\n", 2);

            return true;
        }
    }

    return false;
}