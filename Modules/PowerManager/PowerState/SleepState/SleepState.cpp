//=====[Libraries]=============================================================

#include "SleepState.h"
#include "PowerManager.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define MAX_TRIES 10
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
SleepState::SleepState () {
    this->manager = NULL;
    this->status = SLEEP;
    this->ManualTurningPower = false;
    this->SignalTurningPowerUp = false;
    this->TurningDown = false;
}


/** 
* @brief
* 
* @param 
*/
SleepState::SleepState (PowerManager * newManager) {
    this->manager = newManager;
    this->status = SLEEP;
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
SleepState::~SleepState () {
    this->manager = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
powerStatus_t SleepState::startStopUpdate (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    return SLEEP;
}

/** 
* @brief 
* 
* 
* @returns 
*/
bool SleepState::reboot (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    return false;
}

/** 
* @brief 
* 
* 
* @returns 
*/
bool SleepState::goToSleep (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
  return true;
}

/** 
* @brief 
* 
* 
* @returns 
*/
void SleepState::awake (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    char StringToSendUSB [40] = "Awakening";
    uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
     uartUSB.write ( "\r\n",  3 );  // debug only
    this->manager->changeDTRSignal(OFF);
    this->manager->changePowerState (new PowerONState ( this->manager) );
}

/** 
* @brief 
* 
* 
* @returns 
*/
bool SleepState::measureBattery (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer
    ,  BatteryData * currentBatteryData) {
   return false;
}

/** 
* @brief 
* 
* 
* @returns 
*/
bool SleepState::turnOn (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    this->awake(AThandler, powerChangeDurationTimer);
    return true;
}


/** 
* @brief 
* 
* 
* @returns 
*/
bool SleepState::turnOff (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    static bool readyToSend = true;
    static bool hardPowerOffUnderProcess = false;
    static int retryCounter = 0;
    char StringToSend [15] = "AT+QPOWD";
    char StringToBeRead [40];
    char ExpectedResponse [15] = "POWERED DOWN";
    char StringToSendUSB [40] = "Turning off";

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
            this->manager->changePowerState (new ManualPowerOFFState (this->manager));
            return true;
        }
    }

    if (retryCounter <= MAX_TRIES) {
        if (powerChangeDurationTimer->read()  ) {
            retryCounter++;
            readyToSend = true;
        }
    } else { //// if there is no response form the module use the hard power option
        if (powerChangeDurationTimer->read()  ) {
            if (hardPowerOffUnderProcess  == false) {
                this->manager->changePowerDownSignal (OFF);
                hardPowerOffUnderProcess = true;
            } else {
                this->manager->changePowerDownSignal (ON);
                readyToSend = true;
                hardPowerOffUnderProcess = false;
                retryCounter = 0;
                this->manager->changePowerState (new ManualPowerOFFState (this->manager));
                return true;

            }

        }
    }
    
    return false;
}