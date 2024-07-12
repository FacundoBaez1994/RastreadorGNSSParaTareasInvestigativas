//=====[Libraries]=============================================================
#include "cellularModule.h"

//=====[Declaration of private defines]========================================
#define POWERCHANGEDURATION  700
#define CELLULAR_MODULE_TX_UART PA_9
#define CELLULAR_MODULE_RX_UART PA_10
#define CELLULAR_MODULE_BAUD_RATE 115200
#define CELLULAR_MODULE_POWER_STATUS_SIGNAL_PIN_INPUT PA_8
#define CELLULAR_MODULE_POWER_CONTROL_BUTTON_PIN_INPUT PA_1
#define CELLULAR_MODULE_POWER_KEY_PIN_OUTPUT PB_4
#define CELLULAR_MODULE_POWER_DOWN_PIN_OUTPUT PB_4


//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================

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
cellularModule::cellularModule () {
    this->powerChangeDurationtimer = new nonBlockingDelay (POWERCHANGEDURATION  );
    this->serialComunicationUART = new BufferedSerial  (CELLULAR_MODULE_TX_UART, 
     CELLULAR_MODULE_RX_UART, CELLULAR_MODULE_BAUD_RATE);
    this->turningPower = false;
    this->powerStatusInput = new DigitalIn (CELLULAR_MODULE_POWER_STATUS_SIGNAL_PIN_INPUT); 
    this->powerControlButtonInput = new DigitalIn (CELLULAR_MODULE_POWER_CONTROL_BUTTON_PIN_INPUT);
    this->powerKeyOutput = new DigitalOut (CELLULAR_MODULE_POWER_KEY_PIN_OUTPUT);
    this->powerDownOutput = new DigitalOut (CELLULAR_MODULE_POWER_DOWN_PIN_OUTPUT);
    *powerKeyOutput = OFF;
    *powerDownOutput = ON; 
}

cellularModule::~cellularModule () {
}

void cellularModule::startStopUpdate () {
    if (this->powerControlButtonInput->read () == OFF  && this->turningPower == false ) {  
        this->turningPower = true;
        *this->powerKeyOutput = ON;
        this->powerChangeDurationtimer->restart ();
    }  
    if (this->turningPower == true && this->powerChangeDurationtimer->read() )  {
        this->turningPower = false;
        *this->powerKeyOutput = OFF;
    } 
}



/** 
* @brief 
* 
* @returns 
*/
 BufferedSerial* cellularModule::getUART () {
     return this->serialComunicationUART;
}




//=====[Implementations of private functions]==================================
/** 
* @brief
* @note 
*/
