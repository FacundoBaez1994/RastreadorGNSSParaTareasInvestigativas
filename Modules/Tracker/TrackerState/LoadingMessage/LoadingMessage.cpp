//=====[Libraries]=============================================================

#include "LoadingMessage.h"
#include "SavingMessage.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
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
LoadingMessage::LoadingMessage  (Tracker * tracker) {
    this->tracker = tracker;
}

/** 
* @brief
* 
* @param 
*/
LoadingMessage::~LoadingMessage  () {
    this->tracker = nullptr;
}

void LoadingMessage::loadMessage (EEPROMManager * memory, char * message) {
    static char  poppedString [2048] = {0};
    char  log [100];
    static bool decryptionProcessFinished = false;
    static bool popProcessFinished = false;
    static bool initialization = false;

    if (initialization  == false) {
        memset(poppedString, 0, sizeof(poppedString));
        initialization = true;
    }
    
    
    EEPROMStatus state;
    if (popProcessFinished  == false) {
        state = memory->popStringFromEEPROM( poppedString, sizeof(poppedString));
        if (state == EEPROMStatus::POPPEDSTRINGOK) {
            snprintf(log, sizeof(log), "popped string From Memory\n\r");
            uartUSB.write(log, strlen(log));
            uartUSB.write(poppedString, strlen(poppedString));
            uartUSB.write("\n\r", strlen("\n\r"));

            popProcessFinished = true;
        } else if (state ==  EEPROMStatus::EMPTY) {
            snprintf(log, sizeof(log), "EEPROM empty\n\r");
            uartUSB.write(log, strlen(log));
            initialization = false;
            this->tracker->changeState  (new GoingToSleep (this->tracker));
            return;
        }
    } else {
        if (this->tracker->decryptMessage(poppedString, sizeof (poppedString)) == true) {
            snprintf(log, sizeof(log), "\n\rultimo string descifrado:\n\r");
            uartUSB.write(log, strlen(log));
            uartUSB.write(poppedString, strlen(poppedString));
            uartUSB.write("\n\r", strlen("\n\r"));
            initialization = false;
            popProcessFinished = false;
            this->tracker->changeState  (new GoingToSleep (this->tracker));
            return;
        }
    }
    return;
 }

//=====[Implementations of private methods]==================================