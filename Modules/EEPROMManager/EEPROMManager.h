//=====[#include guards - begin]===============================================
#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H
 
//==================[Libraries]===============================================
#include "mbed.h"
#include "string.h"
#include "Non_Blocking_Delay.h"
#include <vector>

//=====[Declaration of public data types]======================================
/**
 * @enum EEPROMStatus
 * @brief Status codes for EEPROM operations.
 */
enum class EEPROMStatus {
    PROCESSING,      /**< Operation is still in progress */
    EMPTY,           /**< No data available in EEPROM */
    POPPEDSTRINGOK,  /**< String popped and cleared successfully */
    NOMEMORY,        /**< Not enough space available in EEPROM */
    PUSHOK           /**< String pushed successfully */
};


//=====[Declaration of public classes]=========================================
/** @class EEPROMManager
 *  @brief Class for managing storage of strings in an external I2C EEPROM device.
 * This class provides non-blocking operations to push, pop, and clear
 * null-terminated strings in EEPROM memory, using page-based writes
 * and internal delay handling to respect EEPROM timing constraints.
 */
class EEPROMManager {
 //=====[Declaration of public methods]=========================================
public:
    /**
     * @brief Constructs the EEPROM manager and initializes I2C and delay systems.
     */
    EEPROMManager ();

    /**
     * @brief Destroys the EEPROM manager and releases allocated resources.
    */
    virtual ~EEPROMManager ();

    /**
     * @brief Checks whether the EEPROM is properly connected on the I2C bus.
     * @return true if the EEPROM responds, false otherwise.
     */
    bool isEEPROMConnected();

    /**
     * @brief Prints all stored strings from the EEPROM via serial.
     * Strings are read sequentially from the EEPROM until an empty or invalid 
     * section is found.
    */
    void printAllStringsFromEEPROM();

    
    /**
     * @brief Attempts to write a null-terminated string to the next available memory space.
     * This method is non-blocking and should be called repeatedly until a final
     * status is returned.
     * @param newString The string to be stored.
     * @return EEPROMStatus The current state of the operation.
     */
    EEPROMStatus pushStringToEEPROM(const char* newString);

    /**
     * @brief Retrieves the last valid string from EEPROM and clears it.
     * This method first reads all strings to find the last valid one,
     * returns it in the provided buffer, and erases it in a non-blocking way.
     * @param outputBuffer Destination buffer to store the retrieved string.
     * @param bufferSize Size of the output buffer.
     * @return EEPROMStatus The current state of the operation.
     */
    EEPROMStatus popStringFromEEPROM( char* outputBuffer, size_t bufferSize);

    /**
     * @brief Clears all stored strings in EEPROM memory.
     * This operation is performed in a non-blocking manner. It should be
     * called periodically until it returns true.
     * @return true when the memory has been successfully cleared.
     * @return false if the operation is still in progress.
     */
    bool clearAll();
 
 private:
 //=====[Declaration of private atributtes]=========================================
    I2C* i2c;                      /**< Pointer to I2C interface */
    NonBlockingDelay* delay;      /**< Delay handler to manage timing between operations */
    int address;                  /**< I2C address of the EEPROM device */
    int pageSize;                 /**< EEPROM's internal page size in bytes */

    char* buffer;                 /**< Optional: reserved buffer for future expansion */
    size_t sizeOfBuffer = 1024;   /**< Reserved buffer size */

//=====[Declaration of private methods]=========================================
    /**
     * @brief Internal method to write a null-terminated string to EEPROM.
     * Handles page-aligned writes and uses non-blocking delay between pages.
     * @param memoryAddress Starting address in EEPROM to write the string.
     * @param data The null-terminated string to write.
     * @return true if writing is complete, false otherwise.
     */
    bool writeStringToEEPROM( int memoryAddress, const char* data);

    /**
     * @brief Reads a null-terminated string starting from the specified EEPROM address.
     * Reads until a null terminator or maximum length is reached.
     * @param memoryAddress EEPROM address to start reading from.
     * @return std::string The string read from EEPROM.
     */
    std::string readCStringFromEEPROM( uint16_t memoryAddress);

};

#endif //  EEPROM_MANAGER_H