//=====[Libraries]=============================================================

#include "SavingMessage.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "LoadingMessage.h"
#include "GoingToSleep.h"

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20
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
SavingMessage::SavingMessage  (Tracker * tracker) {
    this->tracker = tracker;
}

/** 
* @brief
* 
* @param 
*/
SavingMessage::~SavingMessage  () {
    this->tracker = nullptr;
}

void SavingMessage::saveMessage (EEPROMManager * memory, char * message) {
    char log [2054];
    static bool encryptionProcessFinished = false;
    EEPROMStatus state;
    
    if ( encryptionProcessFinished  == false) {
        if (this->tracker->encryptMessage (message) == true) {
            encryptionProcessFinished  = true;
        }
    }

    if ( encryptionProcessFinished  == true) {
        state = memory->pushStringToEEPROM (message);
        if ( state == EEPROMStatus::PUSHOK) {
            snprintf(log, sizeof(log), "STRINGS PUSHEADA: %s\n\r", message);
            uartUSB.write(log, strlen(log));
            encryptionProcessFinished = false;
            this->tracker->changeState  (new LoadingMessage (this->tracker));
            return;
        }  else if (state ==  EEPROMStatus::NOMEMORY) {
            snprintf(log, sizeof(log), "EEPROM has no memory left\n\r");
            uartUSB.write(log, strlen(log));
            encryptionProcessFinished = false;
            this->tracker->changeState  (new GoingToSleep (this->tracker));
            return; 
        }
    }
    return;
    
 }

//=====[Implementations of private methods]==================================