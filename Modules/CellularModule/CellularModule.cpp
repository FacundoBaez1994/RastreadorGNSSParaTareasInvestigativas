//=====[Libraries]=============================================================
#include "CellularModule.h"
#include "Debugger.h"
#include "IdleState.h"
#include "TransmissionUnavailable.h"


//=====[Declaration of private defines]========================================
#define POWERCHANGEDURATION  700
#define REFRESHTIME  1000
#define CELLULAR_MODULE_TX_UART PA_9
#define CELLULAR_MODULE_RX_UART PA_10
#define CELLULAR_MODULE_BAUD_RATE 115200
#define CELLULAR_MODULE_POWER_STATUS_SIGNAL_PIN_INPUT PB_5
#define CELLULAR_MODULE_POWER_CONTROL_BUTTON_PIN_INPUT PA_0
#define CELLULAR_MODULE_POWER_KEY_PIN_OUTPUT PB_4
#define CELLULAR_MODULE_POWER_DOWN_PIN_OUTPUT PB_0
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
    this->powerChangeDurationtimer = new NonBlockingDelay (POWERCHANGEDURATION  );
    this->refreshTime = new NonBlockingDelay (REFRESHTIME);
    this->ATHandler = new ATCommandHandler (new BufferedSerial  (CELLULAR_MODULE_TX_UART, 
     CELLULAR_MODULE_RX_UART, CELLULAR_MODULE_BAUD_RATE));
    this->currentConnectionState = new IdleState (this);
    this->currentTransmissionState = new TransmissionUnavailable (this);
 
    this->powerStatusInput = new DigitalIn (CELLULAR_MODULE_POWER_STATUS_SIGNAL_PIN_INPUT); 
    this->powerControlButtonInput = new DigitalIn (CELLULAR_MODULE_POWER_CONTROL_BUTTON_PIN_INPUT);
    this->powerKeyOutput = new DigitalOut (CELLULAR_MODULE_POWER_KEY_PIN_OUTPUT);
    this->powerDownOutput = new DigitalOut (CELLULAR_MODULE_POWER_DOWN_PIN_OUTPUT);
    this->simCardSwitchOutput =  new DigitalOut (CELLULAR_MODULE_SIMCARD_SWITCH_OUTPUT);

    this->powerControlButtonInput->mode(PullUp);
    *this->powerKeyOutput = OFF;
    *this->powerDownOutput = ON; 
    *this->simCardSwitchOutput = ON; 

    this->turningPowerManual = false;
    this->turningPowerAutomatic = false;
    this->watingForResponse = false;
    this->wasManualyTurnOff = false;
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
     //////////////////// MANUAL TURN ON // OFF ///////////////////
    if (this->powerControlButtonInput->read () == OFF  && this->turningPowerManual == false
    && turningPowerAutomatic == false ) {  
        if (this->powerStatusInput->read () == OFF) {
            this->wasManualyTurnOff = true;
        } else{
            this->wasManualyTurnOff = false;
        }
        this->turningPowerManual = true;
        *this->powerKeyOutput = ON;
        this->powerChangeDurationtimer->restart ();
        ///////////////////////////////////////
        char StringToSend [30] = "MANUAL POWER CHANGE INIT";
        uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ///////////////////////////////////////
    }  
    if (this->turningPowerManual == true && this->powerChangeDurationtimer->read() )  {
        this->turningPowerManual = false;
        *this->powerKeyOutput = OFF;

        ///////////////////////////////////////
        char StringToSend [30] = "MANUAL POWER CHANGE END";;
        uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ///////////////////////////////////////
        this->changeConnectionState(new IdleState (this));
        this->changeTransmissionState(new TransmissionUnavailable (this));
    } 

     //////////////////// AUTOMATIC TURN ON // OFF ///////////////////
        if ( this->powerStatusInput->read () == ON && this->turningPowerManual == false
    && turningPowerAutomatic == false &&  this->wasManualyTurnOff == false ) {  
        this->turningPowerAutomatic = true;
        *this->powerKeyOutput = ON;
        this->powerChangeDurationtimer->restart ();
        ///////////////////////////////////////
        char StringToSend [40] = "AUTOMATIC POWER CHANGE INIT";
        uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ///////////////////////////////////////
    }  
    if (this->turningPowerAutomatic == true && this->powerChangeDurationtimer->read() )  {
        this->turningPowerAutomatic = false;
        *this->powerKeyOutput = OFF;

        ///////////////////////////////////////
        char StringToSend [40] = "AUTOMATIC POWER CHANGE END";
        uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ///////////////////////////////////////
        this->changeConnectionState(new IdleState (this));
        this->changeTransmissionState(new TransmissionUnavailable (this));
    } 

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
void CellularModule::sendMessage (char * message, char * ipDirection, int tcpPort ) {
    this->currentTransmissionState->send (this->ATHandler,
    this->refreshTime, message, ipDirection, tcpPort);
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

//=====[Implementations of private functions]==================================

