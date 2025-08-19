//=====[Libraries]=============================================================
#include "EEPROMManager.h"
#include "Debugger.h"
#include "Non_Blocking_Delay.h"

#include <vector>
#include <string>


//=====[Declaration of private defines]========================================
#define REFRESH_MEMORY 5
#define EEPROM_SIZE 32768

#define I2C_SDA PA_10
#define I2C_SCL PA_9

#define LOG_BUFFER 40 
#define MAX_STRING_SIZE_ALLOWED 2048 

//=====[Declaration of private data types]=====================================
const int EEPROM_ADDRESS = 0x50 << 1; //I2C dir del AT24C256 (desplazada)
const int PAGE_SIZE = 64;

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============



//=====[Declarations (prototypes) of private functions]========================



//=====[Implementations of public methods]===================================
EEPROMManager::EEPROMManager ()  {
    this->address = EEPROM_ADDRESS;
    this->pageSize = PAGE_SIZE;

    this->delay = new NonBlockingDelay (REFRESH_MEMORY);
    this->i2c = new I2C (I2C_SDA, I2C_SCL);
}

EEPROMManager::~EEPROMManager() {
    delete this->delay;
    this->delay = nullptr;
    delete this->i2c;
    this->i2c = nullptr;
}

bool EEPROMManager::isEEPROMConnected() {
    char addr[2] = {0x00, 0x00};  // Dir 0x0000

    int ack = this->i2c->write(this->address, addr, 2, true);  // true = no stop condition
    return (ack == 0);  // ack == 0 means response
}

void EEPROMManager::printAllStringsFromEEPROM() {
    int currentAddress = 0;
    int stringIndex = 1;
    char log [LOG_BUFFER];

    while (true) {
        std::string currentString = this->readCStringFromEEPROM(currentAddress);

        if (currentString.empty()) {
            // reach the end (empty or no more Strings in EEPROM)
            break;
        }
        uartUSB.write("\n\r", strlen("\n\r"));
        snprintf(log, sizeof(log), "String %d:", stringIndex);
        uartUSB.write(log, strlen(log));
        uartUSB.write(currentString.c_str(), currentString.length() );
        uartUSB.write("\n\r", strlen("\n\r"));

        currentAddress += currentString.length() + 1; // Advancing to the next String
        stringIndex++;
    }
}


bool EEPROMManager::clearAll() {
    static int currentAddress = 0;
    static int lenToClear = 0;
    static bool clearingInProgress = false;
    static std::string currentString;
    static bool readingString = true;
    char buffer[2 + PAGE_SIZE];

    if (!this->delay->read()) {
        return false;
    }

    if (!clearingInProgress) {
        currentAddress = 0;
        clearingInProgress = true;
        readingString = true;
    }

    if (readingString) {
        currentString = this->readCStringFromEEPROM(currentAddress);
        if (currentString.empty()) {
            clearingInProgress = false; 
            return true;
        }
        lenToClear = currentString.length() + 1;
        readingString = false;
    }

    int pageOffset = currentAddress % PAGE_SIZE;
    int spaceInPage = PAGE_SIZE - pageOffset;
    int bytesToWrite = std::min(spaceInPage, lenToClear);

    buffer[0] = (currentAddress >> 8) & 0xFF;
    buffer[1] = currentAddress & 0xFF;
    memset(&buffer[2], 0, bytesToWrite);

    this->i2c->write(this->address, buffer, 2 + bytesToWrite);

    currentAddress += bytesToWrite;
    lenToClear -= bytesToWrite;

    if (lenToClear <= 0) {
        readingString = true; 
    }

    return false;
}

EEPROMStatus EEPROMManager::popStringFromEEPROM(char* outputBuffer, size_t bufferSize) {
    static int currentAddress = 0;
    static int prevAddress = 0;
    static std::string currentString;
    static std::string prevString;
    static int lenToClear = 0;
    static bool readingPhase = true;
    static bool clearingPhase = false;
    static bool copiedToBuffer = false; 

    if (!this->delay->read()) {
        return EEPROMStatus::PROCESSING;
    }

    if (readingPhase) {
        currentString = this->readCStringFromEEPROM(currentAddress);

        if (currentString.empty()) {
            readingPhase = false;

            if (prevString.empty()) {
                currentAddress = 0;
                prevAddress = 0;
                currentString.clear();
                prevString.clear();
                lenToClear = 0;
                readingPhase = true;
                clearingPhase = false;
                copiedToBuffer = false; 
                return EEPROMStatus::EMPTY;
            }

            if (!copiedToBuffer) {
                strncpy(outputBuffer, prevString.c_str(), bufferSize - 1);
                outputBuffer[bufferSize - 1] = '\0';
                copiedToBuffer = true;
            }

            lenToClear = prevString.length() + 1;
            clearingPhase = true;
        } else {
            prevString = currentString;
            prevAddress = currentAddress;
            currentAddress += currentString.length() + 1;
        }

        return EEPROMStatus::PROCESSING;
    }

    if (clearingPhase) {
        int pageOffset = prevAddress % PAGE_SIZE;
        int spaceInPage = PAGE_SIZE - pageOffset;
        int bytesToWrite = std::min(spaceInPage, lenToClear);

        char buffer[2 + PAGE_SIZE];
        buffer[0] = (prevAddress >> 8) & 0xFF;
        buffer[1] = prevAddress & 0xFF;
        memset(&buffer[2], 0, bytesToWrite);

        this->i2c->write(this->address, buffer, 2 + bytesToWrite);

        prevAddress += bytesToWrite;
        lenToClear -= bytesToWrite;

        if (lenToClear <= 0) {
            // Resetear todo
            currentAddress = 0;
            prevAddress = 0;
            currentString.clear();
            prevString.clear();
            lenToClear = 0;
            readingPhase = true;
            clearingPhase = false;
            copiedToBuffer = false;
            return EEPROMStatus::POPPEDSTRINGOK;
        }

        return EEPROMStatus::PROCESSING;
    }

    return EEPROMStatus::PROCESSING;
}

EEPROMStatus EEPROMManager::pushStringToEEPROM(const char* newString) {
    static uint16_t memoryAddress = 0;
    static bool locatingAddress = true;

    if (locatingAddress) {
        memoryAddress = 0;

        while (true) {
            std::string s = this->readCStringFromEEPROM(memoryAddress);
            if (s.empty()) break;
            memoryAddress += s.length() + 1;
        }

        locatingAddress = false;
    }

    if ((memoryAddress + strlen(newString) + 1) > EEPROM_SIZE) {
        locatingAddress = true;
        return EEPROMStatus::NOMEMORY;
    }

    bool writeCompleted = this->writeStringToEEPROM(memoryAddress, newString);

    if (writeCompleted) {
        locatingAddress = true;
        return EEPROMStatus::PUSHOK;
    }

    return EEPROMStatus::PROCESSING;
}

//=====[Implementations of private methods]===================================

bool EEPROMManager::writeStringToEEPROM (int memoryAddress, const char* data) {
    static int dataLen = 0;
    static int totalLen = 0;
    static int written = 0;
    static const char* dataPtr = nullptr;
    static int currentAddress = 0;
    static bool writingInProgress = false;

    if (!writingInProgress) {
        dataLen = strlen(data);
        totalLen = dataLen + 1;
        written = 0;
        currentAddress = memoryAddress;
        dataPtr = data;
        writingInProgress = true;
    }

    if (!this->delay->read()) {
        return false;
    }

    if (written >= totalLen) {
        writingInProgress = false;
        return true;
    }

    int pageOffset = currentAddress % PAGE_SIZE;
    int spaceInPage = PAGE_SIZE - pageOffset;
    int bytesToWrite = std::min(spaceInPage, totalLen - written);

    char buffer[2 + PAGE_SIZE];
    buffer[0] = (currentAddress >> 8) & 0xFF;
    buffer[1] = currentAddress & 0xFF;

    memcpy(&buffer[2], &dataPtr[written], bytesToWrite);

    this->i2c->write(this->address, buffer, 2 + bytesToWrite);

    written += bytesToWrite;
    currentAddress += bytesToWrite;

    return false;
}



std::string EEPROMManager::readCStringFromEEPROM( uint16_t memoryAddress) {
    std::string result = "";
    const size_t maxLength = MAX_STRING_SIZE_ALLOWED;

    for (size_t i = 0; i < maxLength; ++i) {
        char addr[2];
        char c;

        addr[0] = (memoryAddress >> 8) & 0xFF;
        addr[1] = memoryAddress & 0xFF;

        if (this->i2c->write(this->address, addr, 2, false) != 0) break; // Error
        if (this->i2c->read(this->address, &c, 1) != 0) break; // Error

        if (c == '\0') break;

        result += c;
        memoryAddress++;
    }

    return result;
}