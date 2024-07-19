//=====[Libraries]=============================================================

#include "IdleState.h"
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
IdleState::IdleState () {
    this->mobileNetworkModule = NULL;
    this->readyToSend = true;
}


/** 
* @brief
* 
* @param 
*/
IdleState::IdleState (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
}


/** 
* @brief 
* 
* 
* @returns 
*/
IdleState::~IdleState () {
    this->mobileNetworkModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
void IdleState::connect (ATCommandHandler * ATHandler, NonBlockingDelay * refreshTime) {
    char StringToSend [15] = "ATI";
    char StringToBeRead [256];
    char ExpectedResponse [15] = "OK";

    char StringToSendUSB [40] = "IDLE STATE";



    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToSend);
        this->readyToSend = false;
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
            char StringToSendUSB [40] = "Cambiando de estado";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////            
            this->mobileNetworkModule->changeConnectionState (new CheckingSignalStrength (this->mobileNetworkModule));
        }


    }

    if (refreshTime->read()) {
        this->readyToSend = true;
    }
    //
    //
    //

}



//=====[Implementations of private functions]==================================
