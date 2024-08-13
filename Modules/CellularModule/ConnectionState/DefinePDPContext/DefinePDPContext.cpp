//=====[Libraries]=============================================================

#include "DefinePDPContext.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define APN_MOVISTAR "AT+CGDCONT=1,\"IP\",\"internet.movistar.com.ar\"" //APN / username / password   internet.gprs.unifon.com.ar

//=====[Declaration of private sdata types]=====================================

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
DefinePDPContext::DefinePDPContext () {
    this->mobileNetworkModule = NULL;
    this->readyToSend = true;
}


/** 
* @brief
* 
* @param 
*/
DefinePDPContext::DefinePDPContext (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
}


/** 
* @brief 
* 
* 
* @returns 
*/
DefinePDPContext::~DefinePDPContext () {
    this->mobileNetworkModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
bool DefinePDPContext::connect (ATCommandHandler * ATHandler, NonBlockingDelay * refreshTime,
CellInformation * currentCellInformation) {
    char StringToBeRead [256];
    char ExpectedResponse [15] = "OK";
    char StringToSend [50] = APN_MOVISTAR;
    char StringToSendUSB [40] = "DEFINING PDP CONTEXT";


    if (this->readyToSend == true) {
        ATHandler->sendATCommand(APN_MOVISTAR);
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
            char StringToSendUSB [40] = "Cambiando de estado 7";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////    
            this->mobileNetworkModule->enableTransmission();        
            this->mobileNetworkModule->changeConnectionState (new ConnectedState (this->mobileNetworkModule));
            return true;
        }


    }

    if (refreshTime->read()) {
        this->readyToSend = true;
    }
    return false;

}



//=====[Implementations of private functions]==================================
