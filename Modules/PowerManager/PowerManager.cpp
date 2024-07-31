//=====[Libraries]=============================================================
#include "PowerManager.h"
#include "Debugger.h"


//=====[Declaration of private defines]========================================
#define POWERCHANGEDURATION  700
//#define REFRESHTIME  1000
//#define CELLULAR_MODULE_TX_UART PA_9
//#define CELLULAR_MODULE_RX_UART PA_10
//#define CELLULAR_MODULE_BAUD_RATE 115200
#define CELLULAR_MODULE_POWER_STATUS_SIGNAL_PIN_INPUT PB_5
#define CELLULAR_MODULE_POWER_CONTROL_BUTTON_PIN_INPUT PA_0
#define CELLULAR_MODULE_POWER_KEY_PIN_OUTPUT PB_4
#define CELLULAR_MODULE_POWER_DOWN_PIN_OUTPUT PB_0
//#define CELLULAR_MODULE_SIMCARD_SWITCH_OUTPUT PA_8


//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================s

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================


//=====[Implementations of public methods]===================================
/** 
* 
* 
* 
*/
PowerManager::PowerManager (ATCommandHandler * AThandler) {
    this->powerChangeDurationTimer = new NonBlockingDelay (POWERCHANGEDURATION  );
    this->ATHandler = AThandler;
    this->currentPowerState = new PowerOFFState (this);

    this->powerStatusInput = new DigitalIn (CELLULAR_MODULE_POWER_STATUS_SIGNAL_PIN_INPUT); 
    this->powerControlButtonInput = new DigitalIn (CELLULAR_MODULE_POWER_CONTROL_BUTTON_PIN_INPUT);
    this->powerKeyOutput = new DigitalOut (CELLULAR_MODULE_POWER_KEY_PIN_OUTPUT);
    this->powerDownOutput = new DigitalOut (CELLULAR_MODULE_POWER_DOWN_PIN_OUTPUT);

    this->powerControlButtonInput->mode(PullUp);
    *this->powerKeyOutput = OFF;
    *this->powerDownOutput = ON; 
}


/** 
* 
* 
* 
*/
PowerManager::~PowerManager () {
    delete this->powerChangeDurationTimer;
    this->powerChangeDurationTimer = NULL;
    this->ATHandler = NULL;
}

/** 
* @brief 
* 
* 
* @returns 
*/
powerStatus_t PowerManager::startStopUpdate ( ) {
    return this->currentPowerState->startStopUpdate(this->ATHandler, this->powerChangeDurationTimer);
}

/** 
* @brief 
* 
* 
* @returns 
*/
void PowerManager::goToSleep ( ) {
    this->currentPowerState->goToSleep(this->ATHandler, this->powerChangeDurationTimer);
}

/** 
* @brief 
* 
* 
* @returns 
*/
void PowerManager::awake ( ) {
    this->currentPowerState->awake(this->ATHandler, this->powerChangeDurationTimer);
}

/** 
* @brief 
* 
* 
* @returns 
*/
void PowerManager::reboot ( ) {
    this->currentPowerState->reboot(this->ATHandler, this->powerChangeDurationTimer);
}

/** 
* @brief 
* 
* @returns 
*/
void PowerManager::changePowerState (PowerState * newPowerState){
    delete this->currentPowerState;
    this->currentPowerState =  newPowerState;
} 

/** 
* @brief 
* 
* @returns 
*/
bool PowerManager::readInputControlButton ( ) {
    return this->powerControlButtonInput->read();
}

/** 
* @brief 
* 
* @returns 
*/
bool PowerManager::readPowerStatus ( ) {
    return this->powerStatusInput->read();
}

/** 
* @brief 
* 
* @returns 
*/
void PowerManager::changeKeyDigitalSignal (bool newStatus) {
    *this->powerKeyOutput = newStatus; 
}

/** 
* @brief 
* 
* @returns 
*/
 BufferedSerial* PowerManager::getUART () {
     return this->ATHandler->getUART();
}

/** 
* 
* 
* SOFT HARDWARE START STOP
*/
/*void PowerManager::startStopUpdate () {
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
       // this->changeConnectionState(new IdleState (this));
       // this->changeTransmissionState(new TransmissionUnavailable (this));
    } 

}
*/



//=====[Implementations of private functions]==================================

