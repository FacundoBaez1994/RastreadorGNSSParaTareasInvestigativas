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
CellularConnectionStatus_t ConnectedState::connect (ATCommandHandler * ATHandler, NonBlockingDelay * refreshTime,
    CellInformation * currentCellInformation) {
    return CELLULAR_CONNECTION_STATUS_CONNECTED_TO_NETWORK;
}



//=====[Implementations of private functions]==================================
