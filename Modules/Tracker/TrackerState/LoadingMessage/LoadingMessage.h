//=====[#include guards - begin]===============================================
#ifndef _LOADING_MESSAGE_H_
#define _LOADING_MESSAGE_H_

//==================[Libraries]===============================================
#include "mbed.h"
#include "arm_book_lib.h"
#include "TrackerBaseState.h"
#include "Tracker.h"
#include "string.h"

//=====[Declaration of public data types]======================================
class Tracker; /**< Forward declaration to avoid circular dependencies */

//=====[Declaration of public classes]=========================================
/**
 * @class LoadingMessage
 * @brief State responsible for building and preparing the outgoing message.
 * @details This class implements the State design pattern, representing
 * the stage in which the tracker gathers and formats all necessary data
 * (GNSS, IMU, battery, and cellular information) into a message ready
 * for transmission. It handles both the collection of real-time sensor
 * data and the parsing of stored or decrypted messages. Once the message
 * is built, the tracker can transition to the next operational state.
 */
class LoadingMessage : public TrackerBaseState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructs the LoadingMessage state.
     * @param tracker Pointer to the tracker context object.
     */
    LoadingMessage (Tracker * tracker);

    /**
     * @brief Destructor.
     */
    virtual ~LoadingMessage ();

    /**
     * @brief Loads and assembles the message payload.
     * 
     * @details Collects information from multiple data sources:
     * EEPROM storage, cellular network info, GNSS positioning,
     * IMU sensor readings, and battery status. All this data is
     * formatted into a unified message for further processing
     * or transmission.
     * @param memory Pointer to EEPROM manager for stored data access.
     * @param aCellInfo Pointer to current cell information structure.
     * @param GNSSInfo Pointer to GNSS data structure.
     * @param neighborsCellInformation Reference to a vector of neighboring cell info pointers.
     * @param imuData Pointer to current IMU data structure.
     * @param IMUDataSamples Reference to a vector of IMU data samples pointers.
     * @param batteryStatus Pointer to battery status structure.
     */
    virtual void loadMessage (EEPROMManager * memory, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
    IMUData_t * imuData, std::vector<IMUData_t*> &IMUDataSamples, BatteryData  * batteryStatuss);
private:
//=====[Declaration of privates atributes]=========================================
    Tracker * tracker;  /**< Pointer to tracker context object */

//=====[Declaration of privates methods]=========================================
    /**
     * @brief Parses a decrypted message and extracts relevant information.
     * @param decryptedString Null-terminated decrypted message string.
     * @param aCellInfo Pointer to current cell information structure.
     * @param GNSSInfo Pointer to GNSS data structure.
     * @param neighborsCellInformation Reference to vector of neighboring cell info pointers.
     * @param imuData Pointer to IMU data structure.
     * @param IMUDataSamples Reference to vector of IMU data samples pointers.
     * @param batteryStatus Pointer to battery status structure.
     * @return trackerStatus_t Status code indicating parsing result.
     */
    trackerStatus_t parseDecryptedMessage(const char* decryptedString, 
        CellInformation* aCellInfo,
        GNSSData* GNSSInfo,
        std::vector<CellInformation*>& neighborsCellInformation,
        IMUData_t* imuData,
        std::vector<IMUData_t*>& IMUDataSamples,
        BatteryData* batteryStatus);

    /**
     * @brief Parses MNMN message type (cellular and basic status data).
     * @param message Null-terminated message string.
     * @param aCellInfo Pointer to current cell information structure.
     * @param neighborsCellInformation Reference to vector of neighboring cell info pointers.
     * @param imuData Pointer to IMU data structure.
     * @param batteryStatus Pointer to battery status structure.
     */
    void parseMNMN(const char* message,
        CellInformation* aCellInfo,
        std::vector<CellInformation*>& neighborsCellInformation,
        IMUData_t* imuData,
        BatteryData* batteryStatus);

    /**
     * @brief Parses MNGNSS message type (cellular and GNSS data).
     * @param message Null-terminated message string.
     * @param aCellInfo Pointer to current cell information structure.
     * @param GNSSInfo Pointer to GNSS data structure.
     * @param imuData Pointer to IMU data structure.
     * @param batteryStatus Pointer to battery status structure.
     */
    void parseMNGNSS(const char* message,
        CellInformation* aCellInfo,
        GNSSData* GNSSInfo,
        IMUData_t* imuData,
        BatteryData* batteryStatus);

    /**
     * @brief Parses GNSS-only message type.
     * 
     * @param message Null-terminated message string.
     * @param GNSSInfo Pointer to GNSS data structure.
     * @param imuData Pointer to IMU data structure.
     * @param batteryStatus Pointer to battery status structure.
     */
    void parseGNSS(const char* message,
        GNSSData* GNSSInfo,
        IMUData_t* imuData,
        BatteryData* batteryStatus);

    /**
     * @brief Parses IMU message type (inertial measurement data).
     * @param message Null-terminated message string.
     * @param imuData Pointer to IMU data structure.
     * @param IMUDataSamples Reference to vector of IMU data samples pointers.
     * @param batteryStatus Pointer to battery status structure.
     */
    void parseIMU(const char* message,
        IMUData_t* imuData,
        std::vector<IMUData_t*>& IMUDataSamples,
        BatteryData* batteryStatus);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _LOADING_MESSAGE_H_