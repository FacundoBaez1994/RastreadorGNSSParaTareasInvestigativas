//=====[Libraries]=============================================================

#include "ActivatePDP.h"
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
ActivatePDP::ActivatePDP () {
    this->mobileNetworkModule = NULL;
    this->readyToSend = true;
}


/** 
* @brief
* 
* @param 
*/
ActivatePDP::ActivatePDP (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
}


/** 
* @brief 
* 
* 
* @returns 
*/
ActivatePDP::~ActivatePDP () {
    this->mobileNetworkModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
bool ActivatePDP::send (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, char * ipDirection, int tcpPort) {
    char StringToBeRead [20];
    char ExpectedResponse [15] = "OK";
    char StringToSendUSB [15] =  "ACTIVATE PDP";
    char StringToSend [50] = "AT+CGACT=1,1";


    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToSend);
        this->readyToSend  = false;
        ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
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
            char StringToSendUSB [40] = "Cambiando de estado 1";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////     
            this->mobileNetworkModule->changeTransmissionState (new CreateSocket (this->mobileNetworkModule));
        }


    }

    if (refreshTime->read()) {
        this->readyToSend = true;
    }

    return false;
}



//=====[Implementations of private functions]==================================
