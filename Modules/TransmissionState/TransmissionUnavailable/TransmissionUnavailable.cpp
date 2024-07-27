//=====[Libraries]=============================================================

#include "TransmissionUnavailable.h"
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
TransmissionUnavailable::TransmissionUnavailable () {
    this->mobileNetworkModule = NULL;
    this->readyToSend = true;
}


/** 
* @brief
* 
* @param 
*/
TransmissionUnavailable::TransmissionUnavailable (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
}


/** 
* @brief 
* 
* 
* @returns 
*/
TransmissionUnavailable::~TransmissionUnavailable () {
    this->mobileNetworkModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
void TransmissionUnavailable::send (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, char * message, char * ipDirection, int tcpPort) {
   // char StringToSendUSB [40] = "TRANSMISSION UNAVAILABLE STATE";
   // uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
    //uartUSB.write ( "\r\n",  3 );  // debug only
    return;
}



//=====[Implementations of private functions]==================================
