//=====[Libraries]=============================================================

#include "ConnectedState.h"
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
ConnectedState::ConnectedState () {
    this->mobileNetworkModule = NULL;
    this->enableTransmission = false;
}


/** 
* @brief
* 
* @param 
*/
ConnectedState::ConnectedState (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->enableTransmission = false;
}


/** 
* @brief 
* 
* 
* @returns 
*/
ConnectedState::~ConnectedState () {
    this->mobileNetworkModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
void ConnectedState::connect (ATCommandHandler * ATHandler, NonBlockingDelay * refreshTime) {
  //  char StringToSendUSB [40] = "CONNECTED STATE";
    char StringToSendUSB2 [40] = "ENABLING TRANSMISSION";
  //  uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
  //  uartUSB.write ( "\r\n",  3 );  // debug only

    if (this->enableTransmission == false) {

        uartUSB.write (StringToSendUSB2 , strlen (StringToSendUSB2 ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         //   this->mobileNetworkModule->changeTransmissionState (new ConnectedState (this->mobileNetworkModule));
        this->enableTransmission = true;
    }
    return;
}



//=====[Implementations of private functions]==================================
