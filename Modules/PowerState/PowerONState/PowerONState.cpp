//=====[Libraries]=============================================================

#include "PowerONState.h"
#include "PowerManager.h" //debido a declaracion adelantada
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
PowerONState::PowerONState () {
    this->manager = NULL;
    this->status = POWER_ON;
    this->ManualTurningPower = false;
    this->SignalTurningPowerUp = false;
    this->TurningDown = false;
}


/** 
* @brief
* 
* @param 
*/
PowerONState::PowerONState (PowerManager * newManager) {
    this->manager = newManager;
    this->status = POWER_ON;
    this->ManualTurningPower = false;
    this->SignalTurningPowerUp = false;
    this->TurningDown = false;
}


/** 
* @brief 
* 
* 
* @returns 
*/
PowerONState::~PowerONState () {
    this->manager = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
powerStatus_t PowerONState::startStopUpdate (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {

    // PowerStatus ON equals 
    if (this->manager->readPowerStatus()  == ON) {
        ////////////  //////////// ////////////
            char StringToSend [30] = "POWER OFF DETECTED";;
            uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            //////////// //////////// ////////////
        if (this->ManualTurningPower == false) {
            ////////////  //////////// ////////////
            char StringToSend [30] = "AUTOMATIC TURNING OFF";;
            uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            //////////// //////////// ////////////
            this->manager->changePowerState (new PowerOFFState ( this->manager) );
            this->status = POWER_OFF;
            return this->status;
        } else {
            ////////////  //////////// ////////////
            char StringToSend [30] = "MANUAL TURNING OFF";;
            uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            //////////// //////////// ////////////
            this->manager->changePowerState (new ManualPowerOFFState ( this->manager) );
            this->status = MANUAL_POWER_OFF;
            return this->status;

        }
    }

    // reverse logic
    // Reading button
    if (this->manager->readInputControlButton() == OFF) {
        this->ManualTurningPower = true;
         this->TurningDown = true;
    }

    // Start Stop Signal
    if (ManualTurningPower == true && SignalTurningPowerUp == false && this->TurningDown == true ) {

        //////////////////////////////////////////
        char StringToSend [30] = "UP";
        uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         //////////////////////////////////////////
        this->manager->changeKeyDigitalSignal (true);
        this->ManualTurningPower = true;
        this->SignalTurningPowerUp = true;
        powerChangeDurationTimer->restart();
    }
    if (this->SignalTurningPowerUp  == true && powerChangeDurationTimer->read() && this->TurningDown == true) {

        //////////////////////////////////////////
        char StringToSend [30] = "DOWN";
        uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         //////////////////////////////////////////
        this->manager->changeKeyDigitalSignal (false);
        this->SignalTurningPowerUp = false;
        powerChangeDurationTimer->restart();
        this->TurningDown = false;
    }
    return this->status;
}

/** 
* @brief 
* 
* 
* @returns 
*/
void PowerONState::reboot (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    static bool readyToSend = true;
    char StringToSend [15] = "AT+QPOWD";
    char StringToBeRead [256];
    char ExpectedResponse [15] = "POWERED DOWN";
    char StringToSendUSB [40] = "Rebooting";

    if (readyToSend == true) {
        AThandler->sendATCommand(StringToSend);
        readyToSend = false;
        ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ////   ////   ////   ////   ////   ////   
    }


    if ( AThandler->readATResponse ( StringToBeRead) == true) {
         ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         ////   ////   ////   ////   ////   ////

        if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
            ////   ////   ////   ////   ////   ////
            char StringToSendUSB [40] = "TURNING DOWN";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////            
            this->manager->changePowerState (new PowerOFFState (this->manager));
        }
    }

    if (powerChangeDurationTimer->read()) {
        readyToSend = true;
    }
}

/** 
* @brief 
* 
* 
* @returns 
*/
void PowerONState::goToSleep (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {

  // 
  // poner DTR A HIGH 
}

/** 
* @brief 
* 
* 
* @returns 
*/
void PowerONState::awake (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    return;
}


//=====[Implementations of private functions]==================================
