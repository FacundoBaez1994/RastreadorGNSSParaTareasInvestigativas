//=====[Libraries]=============================================================

#include "EEPROMManager.h"
#include "Debugger.h"
#include "Non_Blocking_Delay.h"

#include <vector>
#include <string>


//=====[Declaration of private defines]========================================
#define REFRESH_MEMORY 20
#define CHUNK_SIZE 252
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



//=====[Implementations of public methods]===================================
/**
 * @brief
 */
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
    static int index = 0;
    static bool indexInit = false;

    const char* SEPARATOR = "@@";


    int strLength = strlen(newString);
    int chunksCount = (strLength + CHUNK_SIZE - 1) / CHUNK_SIZE;
    //snprintf(log, sizeof(log), "\n\rchunksCount %i\n\r", chunksCount);
    //uartUSB.write(log, strlen(log));

    std::vector<std::string> chunks;
    chunks.reserve(chunksCount);

    for (int i = 0; i < chunksCount; i++) {
        int startPos = i * CHUNK_SIZE;
        int copyLength = (startPos + CHUNK_SIZE > strLength)
                       ? (strLength - startPos)
                       : CHUNK_SIZE;

        std::string chunk;
        chunk.append(newString + startPos, copyLength);

        if (i < chunksCount - 1) {
            chunk.append(SEPARATOR);
        }

        chunks.push_back(std::move(chunk));
    }

    if (!indexInit) {
        index = chunks.size() - 1;  // Inicializa index al último elemento
        indexInit = true;
    }

    if (index >= 0 && index < chunks.size()) {  // Verificación adicional

        const std::string& currentChunk = chunks[index];
        

        
        
        if (this->pushPartialStringToEEPROM(const_cast<char*>(currentChunk.c_str())) != EEPROMStatus::PUSHOK) {
            return EEPROMStatus::PROCESSING;
        }
        uartUSB.write("\n\rpushedPartialString:\n\r", strlen("\n\rpushedPartialString:\n\r"));
        uartUSB.write(currentChunk.c_str(), currentChunk.length());
        index--;
    } else {
        indexInit = false;  // Reset para la próxima vez
        //uartUSB.write("EEPROMStatus::PUSHOK\n\r", strlen("EEPROMStatus::PUSHOK\n\r"));
        return EEPROMStatus::PUSHOK;
    }

    //uartUSB.write("EEPROMStatus::PROCESSING\n\r", strlen("EEPROMStatus::PROCESSING\n\r"));
    return EEPROMStatus::PROCESSING;
}






void EEPROMManager::printAllStringsFromEEPROM() {
    int currentAddress = 0;
    int stringIndex = 1;
    char log [40];

    while (true) {
        std::string currentString = this->readCStringFromEEPROM(currentAddress);

        if (currentString.empty()) {
            // Llegamos al final (EEPROM vacía o no hay más strings)
            break;
        }
        uartUSB.write("\n\r", strlen("\n\r"));
        snprintf(log, sizeof(log), "String %d:", stringIndex);
        uartUSB.write(log, strlen(log));
        uartUSB.write(currentString.c_str(), currentString.length() );
        uartUSB.write("\n\r", strlen("\n\r"));

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
    static std::string accumulatedString;  // Almacena los chunks acumulados
    EEPROMStatus status;
    std::string bufferString;
    // char buffer[300];
    static bool init = false;

    if (init == false) {
        this->buffer = new char [this->sizeOfBuffer];
        init = true;
    }
    
    status = this->popPartialStringFromEEPROM(this->buffer, this->sizeOfBuffer);
    
    if (status == EEPROMStatus::POPPEDSTRINGOK) {
        uartUSB.write("\n\rpoppedPartialString:\n\r", strlen("\n\rpoppedPartialString:\n\r"));
        uartUSB.write(this->buffer, strlen(this->buffer));
        size_t len = strlen(buffer);
        bool hasSeparator = (len >= 2) && (strcmp(buffer + len - 2, "@@") == 0);
        
        if (hasSeparator) {
            // Remueve el @@ y acumula
            this->buffer[len-2] = '\0';
            accumulatedString += this->buffer;
            return EEPROMStatus::PROCESSING; // Espera siguiente chunk
        } else {
            // Último chunk, copia todo
            accumulatedString += this->buffer;
            strncpy(outputBuffer, accumulatedString.c_str(), bufferSize-1);
            outputBuffer[bufferSize-1] = '\0';
            accumulatedString.clear();
            delete [] this->buffer;
            this->buffer = nullptr;
            init = false;
            return EEPROMStatus::POPPEDSTRINGOK;
        }
    }
    if (status == EEPROMStatus::NOMEMORY) {
        uartUSB.write("\n\rEEPROMStatus::NOMEMORY\n\r", strlen("\n\rEEPROMStatus::NOMEMORY\n\r"));
        delete [] this->buffer;
        this->buffer = nullptr;
        init = false;
        return EEPROMStatus::NOMEMORY;
    }
    if (status == EEPROMStatus::EMPTY) {
        delete [] this->buffer;
        this->buffer = nullptr;
        init = false;      
        return EEPROMStatus::EMPTY;
    }
    return EEPROMStatus::PROCESSING; // Retorna otros estados (PROCESSING, EMPTY, etc)
}


EEPROMStatus EEPROMManager::popPartialStringFromEEPROM(char* outputBuffer, size_t bufferSize) {
    static int currentAddress = 0;
    static int prevAddress = 0;
    static std::string currentString;
    static std::string prevString;
    static int lenToClear = 0;
    static bool readingPhase = true;
    static bool clearingPhase = false;
    static bool copiedToBuffer = false;  // NUEVO

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
                copiedToBuffer = false;  // NUEVO
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
            copiedToBuffer = false;  // NUEVO
            return EEPROMStatus::POPPEDSTRINGOK;
        }

        return EEPROMStatus::PROCESSING;
    }

    return EEPROMStatus::PROCESSING;
}


//=====[Implementations of private methods]===================================

EEPROMStatus EEPROMManager::pushPartialStringToEEPROM(const char* newString) {
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