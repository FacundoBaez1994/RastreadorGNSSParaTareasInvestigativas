//=====[Libraries]=============================================================

#include "DeactivatePDP.h"
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
DeactivatePDP::DeactivatePDP () {
    this->mobileNetworkModule = NULL;
    this->readyToSend = true;
}


/** 
* @brief
* 
* @param 
*/
DeactivatePDP::DeactivatePDP (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
}


/** 
* @brief 
* 
* 
* @returns 
*/
DeactivatePDP::~DeactivatePDP () {
    this->mobileNetworkModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
void DeactivatePDP::send (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, char * ipDirection, int tcpPort) {
    char StringToBeRead [20];
    char ExpectedResponse [15] = "OK";
    char StringToSendUSB [15] =  "DEACTIVATE PDP";
    char StringToBeSend [50];
    char ATcommand[] = "AT+QIDEACT="; 
    int connectID = 0; 
    int contextID = 1; 

    snprintf(StringToBeSend, sizeof(StringToBeSend), "%s%d", ATcommand, contextID );


    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToBeSend);
        this->readyToSend  = false;
        ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToBeSend  , strlen (StringToBeSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ////   ////   ////   ////   ////   ////   
    }

    if ( ATHandler->readATResponse ( StringToBeRead) == true) {
         ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         ////   ////   ////   ////   ////   ////

        if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
            ////   ////   ////   ////   ////   ////
            char StringToSendUSB [40] = "Cambiando de estado 5";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////     
            this->mobileNetworkModule->changeTransmissionState (new TransmissionUnavailable (this->mobileNetworkModule));
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
    }
}



//=====[Implementations of private functions]==================================
