//=====[Libraries]=============================================================
#include "CellularModule.h"
#include "ATCommandHandler.h"
#include "Debugger.h"
#include "IdleState.h"
#include "PowerOFFState.h"
#include "PowerState.h"
#include "TransmissionUnavailable.h"


//=====[Declaration of private defines]========================================
#define REFRESHTIME  1000
#define CELLULAR_MODULE_TX_UART PA_9
#define CELLULAR_MODULE_RX_UART PA_10
#define CELLULAR_MODULE_BAUD_RATE 115200
#define CELLULAR_MODULE_SIMCARD_SWITCH_OUTPUT PA_8


//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================s

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================
/** 
* @brief r
*/


//=====[Implementations of public methods]===================================
/** 
* 
* 
* 
*/
CellularModule::CellularModule () {
    this->refreshTime = new NonBlockingDelay (REFRESHTIME);
    this->ATHandler = new ATCommandHandler (new BufferedSerial  (CELLULAR_MODULE_TX_UART, 
     CELLULAR_MODULE_RX_UART, CELLULAR_MODULE_BAUD_RATE));
    this->currentConnectionState = new IdleState (this);
    this->currentTransmissionState = new TransmissionUnavailable (this);
    this->modulePowerManager = new PowerManager (this->ATHandler);

    this->simCardSwitchOutput =  new DigitalOut (CELLULAR_MODULE_SIMCARD_SWITCH_OUTPUT);
    this->currentPowerStatus = POWER_OFF;
}


/** 
* 
* 
* 
*/
CellularModule::~CellularModule () {
}


/** 
* 
* 
* SOFT HARDWARE START STOP
*/
void CellularModule::startStopUpdate () {
    powerStatus_t newPowerStatus;
    newPowerStatus = this->modulePowerManager->startStopUpdate();
    if (this->currentPowerStatus != newPowerStatus) {
       this->currentPowerStatus = newPowerStatus;
       if (this->currentPowerStatus != POWER_ON) {
            this->changeConnectionState (new IdleState (this));
            this->changeTransmissionState  (new TransmissionUnavailable (this));
       }
    }
}



/** 
* 
* 
* SOFT HARDWARE START STOP
*/
void CellularModule::reboot () {
    this->modulePowerManager->reboot ();
}

/** 
* 
* 
* 
*/
void CellularModule::connectToMobileNetwork () {
    this->currentConnectionState->connect(this->ATHandler, this->refreshTime);
}


/** 
* @brief 
* 
* @returns 
*/
bool CellularModule::sendMessage (char * message, char * ipDirection, int tcpPort ) {
    if (this->currentTransmissionState->send (this->ATHandler,
    this->refreshTime, message, ipDirection, tcpPort) == true) {
        this->changeConnectionState(new IdleState (this));
        this->changeTransmissionState(new TransmissionUnavailable (this));
        return true;
    }
    return false;
}


/** 
* @brief 
* 
* @returns 
*/
void CellularModule::changeConnectionState  (ConnectionState * newConnectionState) {
    delete this->currentConnectionState;
    this->currentConnectionState = newConnectionState;
}

/** 
* @brief 
* 
* @returns 
*/
void CellularModule:: enableTransmission () {
    delete this->currentTransmissionState;
    this->currentTransmissionState = new ActivatePDP (this);
}

/** 
* @brief 
* 
* @returns 
*/
void CellularModule::changeTransmissionState  (TransmissionState * newTransmissionState) {
    delete this->currentTransmissionState;
    this->currentTransmissionState = newTransmissionState;
}


/** 
* @brief 
* 
* @returns 
*/
 BufferedSerial* CellularModule::getUART () {
     return this->ATHandler->getUART();
}

/** 
* @brief
* @note 
*/
void CellularModule::switchSIMCARD () {
    *this->simCardSwitchOutput = ! *this->simCardSwitchOutput;
}

 PowerManager* CellularModule::getPowerManager (){
    return this->modulePowerManager;
}

ATCommandHandler* CellularModule::getATHandler (){
    return this->ATHandler;
}


//=====[Implementations of private functions]==================================

