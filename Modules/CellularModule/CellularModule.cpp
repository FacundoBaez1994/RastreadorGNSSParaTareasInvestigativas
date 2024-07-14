//=====[Libraries]=============================================================
#include "CellularModule.h"
#include "Debugger.h"
#include "ConnectionState.h" //debido a declaracion adelantada

//=====[Declaration of private defines]========================================
#define POWERCHANGEDURATION  700
#define REFRESHTIME  1000
#define CELLULAR_MODULE_TX_UART PA_9
#define CELLULAR_MODULE_RX_UART PA_10
#define CELLULAR_MODULE_BAUD_RATE 115200
#define CELLULAR_MODULE_POWER_STATUS_SIGNAL_PIN_INPUT PA_8
#define CELLULAR_MODULE_POWER_CONTROL_BUTTON_PIN_INPUT PA_0
#define CELLULAR_MODULE_POWER_KEY_PIN_OUTPUT PB_4
#define CELLULAR_MODULE_POWER_DOWN_PIN_OUTPUT PB_0


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
 
    this->powerStatusInput = new DigitalIn (CELLULAR_MODULE_POWER_STATUS_SIGNAL_PIN_INPUT); 
    this->powerControlButtonInput = new DigitalIn (CELLULAR_MODULE_POWER_CONTROL_BUTTON_PIN_INPUT);
    this->powerKeyOutput = new DigitalOut (CELLULAR_MODULE_POWER_KEY_PIN_OUTPUT);
    this->powerDownOutput = new DigitalOut (CELLULAR_MODULE_POWER_DOWN_PIN_OUTPUT);

    this->powerControlButtonInput->mode(PullUp);
    *this->powerKeyOutput = OFF;
    *this->powerDownOutput = ON; 
    this->turningPower = false;

    this->watingForResponse = false;
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
    if (this->powerControlButtonInput->read () == OFF  && this->turningPower == false ) {  
        this->turningPower = true;
        *this->powerKeyOutput = ON;
        this->powerChangeDurationtimer->restart ();
        ///////////////////////////////////////
        char StringToSend [15] = "ENCENDIDO ON";
        uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ///////////////////////////////////////
    }  
    if (this->turningPower == true && this->powerChangeDurationtimer->read() )  {
        this->turningPower = false;
        *this->powerKeyOutput = OFF;

        ///////////////////////////////////////
        char StringToSend [15] = "ENCENDIDO OFF";
        uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ///////////////////////////////////////
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
void CellularModule::changeConnectionState  (ConnectionState * newConnectionState) {
    delete this->currentConnectionState;
    this->currentConnectionState = newConnectionState;
}


/** 
* @brief 
* 
* @returns 
*/
 BufferedSerial* CellularModule::getUART () {
     return this->ATHandler->getUART();
}



//=====[Implementations of private functions]==================================
/** 
* @brief
* @note 
*/
