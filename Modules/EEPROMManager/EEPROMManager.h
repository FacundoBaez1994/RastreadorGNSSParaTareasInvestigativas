//=====[#include guards - begin]===============================================
#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H
 
//==================[Libraries]===============================================

#include "mbed.h"
#include "string.h"
#include "Non_Blocking_Delay.h"

//=====[Declaration of public data types]======================================
enum class EEPROMStatus {
    PROCESSING,
    EMPTY,
    POPPEDSTRINGOK,
    NOMEMORY,
    PUSHOK
};

//=====[Declaration of public classes]=========================================

class EEPROMManager {
    //=====[Declaration of public methods]=========================================
public:
    EEPROMManager ();
    virtual ~EEPROMManager ();

    bool isEEPROMConnected();
    void printAllStringsFromEEPROM();
    EEPROMStatus pushStringToEEPROM(const char* newString);
    EEPROMStatus popStringFromEEPROM( char* outputBuffer, size_t bufferSize);
    EEPROMStatus pushPartialStringToEEPROM(const char* newString);
    EEPROMStatus popPartialStringFromEEPROM( char* outputBuffer, size_t bufferSize);
    bool clearAll();
 
 private:
    I2C * i2c;
    NonBlockingDelay * delay;
    int address;
    int pageSize;

    //=====[Declaration of private methods]=========================================
    bool writeStringToEEPROM( int memoryAddress, const char* data);
    std::string readCStringFromEEPROM( uint16_t memoryAddress);
    //=====[Declaration of private atributtes]=========================================

};

//  EEPROM_MANAGER_H
#endif