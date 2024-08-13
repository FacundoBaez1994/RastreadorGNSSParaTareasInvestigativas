//=====[Libraries]=============================================================

#include "CloseSocket.h"
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
CloseSocket::CloseSocket () {
    this->mobileNetworkModule = NULL;
    this->readyToSend = true;
}


/** 
* @brief
* 
* @param 
*/
CloseSocket::CloseSocket (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
}


/** 
* @brief 
* 
* 
* @returns 
*/
CloseSocket::~CloseSocket () {
    this->mobileNetworkModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
bool CloseSocket::send (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted) {
    char StringToBeSend [120];
    char StringToBeRead [20];
    char StringToBeSendUSB []  = "CLOSING TCP SOCKET"; 
    char ExpectedResponse [5] = "OK"; 
    char ATcommand[] = "AT+QICLOSE="; // AT+QICLOSE=<connectID>[,<timeout>] 
    // timeout default = 10 (seconds)
    int connectID = 0; // Puede ser entre 0 y 11

    snprintf(StringToBeSend, sizeof(StringToBeSend), "%s%d", ATcommand, connectID);

    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToBeSend);
        this->readyToSend  = false;
        ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToBeSendUSB , strlen (StringToBeSendUSB ));  // debug only
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
        if ((strcmp (StringToBeRead, ExpectedResponse) == 0) ) {
            ////   ////   ////   ////   ////   ////
            char StringToSendUSB [40] = "Cambiando de estado 4";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////    
            this->mobileNetworkModule->changeTransmissionState (new DeactivatePDP (this->mobileNetworkModule));
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
    }

    return false;
}



//=====[Implementations of private functions]==================================
