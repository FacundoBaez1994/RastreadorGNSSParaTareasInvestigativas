//=====[Libraries]=============================================================

#include "EEPROMManager.h"
#include "Debugger.h"
#include "Non_Blocking_Delay.h"
#include <string>

//=====[Declaration of private defines]========================================
#define REFRESH_MEMORY 5
#define EEPROM_SIZE 32768
#define I2C_SDA PA_10
#define I2C_SCL PA_9 

//=====[Declaration of private data types]=====================================
const int EEPROM_ADDRESS = 0x50 << 1; // Dirección I2C del AT24C256 (desplazada)
const int PAGE_SIZE = 64;

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============



//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
/**
 * @brief
 */
EEPROMManager::EEPROMManager() {
    this->i2c = new I2C (I2C_SDA, I2C_SCL);
    this->address = EEPROM_ADDRESS;
    this->pageSize = PAGE_SIZE;

    this->delay = new NonBlockingDelay (REFRESH_MEMORY);
}

EEPROMManager::~EEPROMManager() {
    delete this->delay;
    this->delay = nullptr;
}



bool EEPROMManager::isEEPROMConnected() {
    char addr[2] = {0x00, 0x00};  // Dirección 0x0000
    // Intentamos escribir 2 bytes de dirección sin datos
    int ack = this->i2c->write(this->address, addr, 2, true);  // true = no stop condition
    return (ack == 0);  // ack == 0 significa que respondió
}

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
        // Aún esperando que termine el retardo anterior
        return false;
    }

    if (written >= totalLen) {
        writingInProgress = false; // Finalizó la escritura
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

    return false; // Aún no terminó
}


/*
void EEPROMManager::writeStringToEEPROM( int memoryAddress, const char* data) {
    int dataLen = strlen(data);
    int totalLen = dataLen + 1; // Incluye el '\0'
    int written = 0;

    while (written < totalLen) {
        int pageOffset = memoryAddress % PAGE_SIZE;
        int spaceInPage = PAGE_SIZE - pageOffset;
        int bytesToWrite = std::min(spaceInPage, totalLen - written);

        char buffer[2 + PAGE_SIZE]; // 2 bytes para dirección, hasta 64 de datos
        buffer[0] = (memoryAddress >> 8) & 0xFF;
        buffer[1] = memoryAddress & 0xFF;

        memcpy(&buffer[2], &data[written], bytesToWrite);

        this->i2c->write(this->address, buffer, 2 + bytesToWrite);
        wait_us(5000); // Esperar escritura

        written += bytesToWrite;
        memoryAddress += bytesToWrite;
    }
}
*/

std::string EEPROMManager::readCStringFromEEPROM( uint16_t memoryAddress) {
    std::string result = "";
    const size_t maxLength = 2024;

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

    // Chequeo de memoria disponible
    if ((memoryAddress + strlen(newString) + 1) > EEPROM_SIZE) {
        locatingAddress = true; // Reset para permitir siguiente push desde cero
        return EEPROMStatus::NOMEMORY;
    }

    bool writeCompleted = this->writeStringToEEPROM(memoryAddress, newString);

    if (writeCompleted) {
        locatingAddress = true; // Para permitir un nuevo push la próxima vez
        return EEPROMStatus::PUSHOK;
    }

    return EEPROMStatus::PROCESSING;
}

void EEPROMManager::printAllStringsFromEEPROM() {
    int currentAddress = 0;
    int stringIndex = 1;
    char log [2024];

    while (true) {
        std::string currentString = this->readCStringFromEEPROM(currentAddress);

        if (currentString.empty()) {
            // Llegamos al final (EEPROM vacía o no hay más strings)
            break;
        }

        snprintf(log, sizeof(log), "String %d: %s\n\r", stringIndex, currentString.c_str());
        uartUSB.write(log, strlen(log));

        currentAddress += currentString.length() + 1; // avanzar al próximo string
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
        // Aún esperando que termine el retardo anterior
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
            clearingInProgress = false; // Terminó el borrado
            return true;
        }
        lenToClear = currentString.length() + 1;
        readingString = false;
    }

    // Borrar bytes del string actual
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
        readingString = true; // Pasamos al siguiente string
    }

    return false; // Aún no terminó
}


EEPROMStatus EEPROMManager::popStringFromEEPROM(char* outputBuffer, size_t bufferSize) {
    static int currentAddress = 0;
    static int prevAddress = 0;
    static std::string currentString;
    static std::string prevString;
    static int lenToClear = 0;
    static bool readingPhase = true;
    static bool clearingPhase = false;

    if (!this->delay->read()) {
        return EEPROMStatus::PROCESSING;
    }

    if (readingPhase) {
        currentString = this->readCStringFromEEPROM(currentAddress);

        if (currentString.empty()) {
            readingPhase = false;

            if (prevString.empty()) {
                // EEPROM vacía
                return EEPROMStatus::EMPTY;
            }

            // Copiar string válida al buffer
            strncpy(outputBuffer, prevString.c_str(), bufferSize - 1);
            outputBuffer[bufferSize - 1] = '\0';

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
            // Resetear estado interno
            currentAddress = 0;
            prevAddress = 0;
            currentString.clear();
            prevString.clear();
            lenToClear = 0;
            readingPhase = true;
            clearingPhase = false;
            return EEPROMStatus::POPPEDSTRINGOK;
        }

        return EEPROMStatus::PROCESSING;
    }

    return EEPROMStatus::PROCESSING; // En cualquier otro caso
}

