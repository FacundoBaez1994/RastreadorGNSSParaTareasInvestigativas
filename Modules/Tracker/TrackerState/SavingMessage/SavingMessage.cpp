//=====[Libraries]=============================================================

#include "SavingMessage.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "LoadingMessage.h"
#include "GoingToSleep.h"

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20

#define LOG_MESSAGE_PUSHED_INTO_MEMORY            "Pushed string:\n\r"
#define LOG_MESSAGE_PUSHED_INTO_MEMORY_LEN        (sizeof(LOG_MESSAGE_PUSHED_INTO_MEMORY) - 1)

#define LOG_MESSAGE_EEPROM_NO_MEMORY_LEFT                  "EEPROM has no memory left\n\r"
#define LOG_MESSAGE_EEPROM_NO_MEMORY_LEFT_LEN              (sizeof(LOG_MESSAGE_EEPROM_NO_MEMORY_LEFT) - 1)

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
SavingMessage::SavingMessage  (Tracker * tracker) {
    this->tracker = tracker;

}

SavingMessage::~SavingMessage  () {
    this->tracker = nullptr;
}

void SavingMessage::saveMessage (EEPROMManager * memory, char * message) {
    static bool bufferCharged = false;
    static bool encryptionProcessFinished = false;
    EEPROMStatus state;

    if (memory == nullptr || message == nullptr ) {
        this->tracker->changeState  (new GoingToSleep(this->tracker));
        return;
    }

    if ( encryptionProcessFinished  == false) {
        if (this->tracker->encryptMessage (message, strlen ( message)) == true) {
            encryptionProcessFinished  = true;
        }
    }

    if ( encryptionProcessFinished  == true) {
        state = memory->pushStringToEEPROM (message);
        if ( state == EEPROMStatus::PUSHOK) {
            uartUSB.write(LOG_MESSAGE_PUSHED_INTO_MEMORY, LOG_MESSAGE_PUSHED_INTO_MEMORY_LEN);
            uartUSB.write(message, strlen(message));
            encryptionProcessFinished = false;
            this->tracker->changeState  (new GoingToSleep (this->tracker));
            return;
        }  else if (state ==  EEPROMStatus::NOMEMORY) {
            uartUSB.write(LOG_MESSAGE_EEPROM_NO_MEMORY_LEFT, LOG_MESSAGE_EEPROM_NO_MEMORY_LEFT_LEN);
            encryptionProcessFinished = false;
            bufferCharged = false;
            this->tracker->changeState  (new GoingToSleep (this->tracker));
            return; 
        }
    }
    return;
    
 }

//=====[Implementations of private methods]==================================