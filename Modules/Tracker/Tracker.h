//=====[#include guards - begin]===============================================
/**
 * @file Tracker.h
 * @brief Declares the Tracker class, which manages the state machine for tracking operations.
 *
 * This class is responsible for controlling the device's tracking behavior,
 * including state transitions, message handling, and management of
 * communication modules. It follows the State design pattern, where
 * different states encapsulate specific behavior.
 */

#ifndef _TRACKER_H_
#define _TRACKER_H_

#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "CellularModule.h"
#include "EEPROMManager.h"
#include "GNSSModule.h"
#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "string.h"
#include "TrackerState.h"
#include "TrackerStatus.h"

#include "IMUManager.h"
#include "EEPROMManager.h"

#include "LoRa.h"

#include "MessageHandler.h"
#include "MessageHandlerStatus.h"
#include "AuthenticationGenerator.h"
#include "AuthenticationVerifier.h"
#include "BaseMessageHandler.h"
#include "ChecksumVerifier.h"
#include "ChecksumGenerator.h"
#include "Decrypter.h"
#include "Encrypter.h"
#include "DecrypterBase64.h"
#include "EncrypterBase64.h"


//=====[Declaration of public defines]=========================================
/**
 * @brief Enumeration representing latency levels for tracker operations.
 */
typedef enum {
    EXTREMELY_LOW_LATENCY,
    VERY_LOW_LATENCY,
    LOW_LATENCY,
    MEDIUM_LATENCY,
    HIGH_LATENCY,
    VERY_HIGH_LATENCY,
    EXTREMELY_HIGH_LATENCY,
} LatencyLevel_t;


/**
 * @brief Enumeration representing the operation modes of the tracker.
 */
typedef enum {
    NORMAL_OPERATION_MODE,      /**< Standard tracking behavior */
    PURSUIT_OPERATION_MODE,      /**< Aggressive tracking mode for pursuit scenarios */
    SILENT_OPERATION_MODE,       /**< Reduced activity to minimize detection or save power */
} OperationMode_t;

/**
 * @brief Enumeration of movement-related events detected by the tracker.
 */
typedef enum {
    MOVING,                 /**< Device is currently in motion */
    STOPPED,                /**< Device is stationary */
    PARKING,                /**< Device is stopped after being in motion */
    MOVEMENT_RESTARTED,     /**< Movement resumed after being stopped */
} MovementEvent_t;



//=====[Declaration of public data types]======================================

//=====[Declaration of public classes]=========================
/**
 * @brief High-level GNSS tracker class coordinating all subsystems.
 * @details This class represents the main control unit of the tracking device.
 * It manages sensors, communication modules, encryption/authentication mechanisms,
 * and operational states using the State design pattern.
 * It delegates specific behaviors to its current state (`TrackerState`) and handles
 * interactions between modules such as:
 * - Cellular communications
 * - GNSS positioning
 * - IMU sensing
 * - EEPROM storage
 * - Encryption, decryption, and authentication
 */
class Tracker {
public:
    /**
     * @brief Constructs the tracker and initializes modules.
     */
    Tracker ();

    /**
     * @brief Destroys the tracker and releases allocated resources.
     */
    virtual ~Tracker ();

    /**
    * @brief Main periodic update function.
    * @details This method acts as the primary execution loop for the Tracker class.
    * It invokes the current state's behavior by calling the state's update methods,
    * orchestrating the sequence of operations required for the device to function correctly.
    * During each call, it handles sensor calibration, power management, GNSS positioning,
    * network connectivity, message formatting and exchange, data saving/loading, and
    * sleep management. The update function is designed to be called repeatedly, typically
    * from the main program loop (or from a even higher class that uses a tracker) 
    * or timer interrupt, ensuring the Tracker's state machine
    * progresses and the device operates responsively in real time.
    */
    void update();

    /**
     * @brief Changes the current operational state of the tracker.
     * @param newTrackerState Pointer to the new state instance.
     */
    void changeState  (TrackerState * newTrackerState);

    /**
     * @brief Updates the movement event based on IMU readings and motion status.
     */
    void updateMovementEvent ();

    /**
     * @brief Sets the current movement event using a string identifier.
     * @param event String representing the movement event.
     */
    void setMovementEvent (char * event);

    /**
     * @brief Gets the current movement event as an enumeration.
     * @return Current movement event.
     */
    MovementEvent_t getMovementEvent ();

    /**
     * @brief Gets the current movement event as a string.
     * @param movementEventString Buffer to store the event string.
     */
    void getMovementEvent (char * movementEventString);

    /**
     * @brief Sets the latency level for tracker operations.
     * @param level Latency level.
     */
    void setLatency(LatencyLevel_t level);

    /**
     * @brief Retrieves the current operation mode.
     * @return Current operation mode.
     */
    OperationMode_t getOperationMode ();

    /**
     * @brief Sets the current operation mode.
     * @param newOperationMode New operation mode.
     */
    void setOperationMode (OperationMode_t newOperationMode);

    /**
     * @brief Configures the silent mode timer.
     * @param hours Duration of silent mode in hours.
     */
    void setSilentTimer (int hours);

    /**
     * @brief Actualize the keep alive timer.
     * @details when the tracker is in STOPPED MODE, it would send a message
     * periodically but with and increase latency based on the current latency.
     */
    void actualizeKeepAliveLatency ();

    /**
     * @brief Encrypts a message using the configured encryption chain.
     * @param message Pointer to the message buffer.
     * @param messageSize Size of the message in bytes.
     * @return True if encryption succeeded, false otherwise.
     */
    bool encryptMessage (char * message, unsigned int messageSize);

    /**
     * @brief Decrypts a message using the configured decryption chain.
     * @param message Pointer to the message buffer.
     * @param messageSize Size of the message in bytes.
     * @return True if decryption succeeded, false otherwise.
     */
    bool decryptMessage (char * message, unsigned int messageSize);
    
    /**
     * @brief Prepares a LoRa message for transmission.
     * @param message Pointer to the message buffer.
     * @param messageSize Size of the message in bytes.
     * @return True if preparation succeeded, false otherwise.
     */
    bool prepareLoRaMessage (char * message, unsigned int messageSize);

    /**
     * @brief Processes a received LoRa message.
     * @param message Pointer to the message buffer.
     * @param messageSize Size of the message in bytes.
     * @return True if processing succeeded, false otherwise.
     */
    bool processLoRaMessage (char * message, unsigned int messageSize);

    /**
     * @brief Retrivs the current numeration of messages sent via LoRa.
     * @return an int with.
     */
    int getLoraMessageNumber ();

    /**
     * @brief increase by one the current numeration of messages sent via LoRa.
     */
    void increaseLoraMessageNumber ();

    /**
     * @brief Checks if the message from the gateway is the correct ACK
     * with the correct identifier and messageNumber.
     * increases LoraMessageNumber if correct
     * @param message Pointer to the message buffer in plaintext.
     * @return bolean true if the ACK message is the expected.
     */
    bool checkMessageIntegrity ( char *messageReceived);

    /**
     * @brief obtain the url path to the server
     * @param urlPathChannel Pointer to a string to store the path.
     */
    void getUrlPathChannel ( char * urlPathChannel);

    /**
     * @brief obtain the identifier of this device
     * @param deviceId Pointer to a string to store the identifier.
     */
    void getDeviceIdentifier ( char * deviceId);

    /**
     * @brief obtain the current Sequence number
     * @returns an integrer with the current Sequence Number.
     */
    int getSequenceNumber ();

    /**
     * @brief increase the sequence Number by one
     */
    void increaseSequenceNumber ();

    /**
     * @brief copies the current chain hash on the prev chain hash
     */
    void progressOnHashChain ();

     /**
     * @brief set a hash chain as the current one
     * @param hashChain a valid hash Chain
     */
    void setCurrentHashChain (char * hashChain);

    /**
     * @brief get the prev hash chain
     * @param hashChain a valid hash Chain
    */
    void getPrevHashChain (char * hashChain);

    
private:
    TrackerState* currentState;            /**< Current operational state */
    NonBlockingDelay* latency;             /**< Latency timer */
    NonBlockingDelay* silentKeepAliveTimer;  /**< Silent mode timer */
    LatencyLevel_t latencyLevel;             /**< latency level */
    OperationMode_t currentOperationMode;   /**< Current mode of operation */

    // MetaData
    int sequenceMessageNumber = 0;  /**< counter of the sequence of messages succefully sent to the remote server from this tracker */
    char* deviceIdentifier; /**< a string that identifies this particular tracker from others of the same type */
    char* prevChainHash;    /**< a string containing a hash of the message that was sent before the current one*/
    char* currChainHash;     /**< a string containing a hash of the message that is about to be sent or was recently sent*/

    deviceMotionStatus_t newMotionStatus = DEVICE_ON_MOTION;        /**< Latest motion status */
    deviceMotionStatus_t currentMotionStatus = DEVICE_ON_MOTION;    /**< Current motion status */
    MovementEvent_t currentMovementEvent = MOVING;                  /**< Current detected movement event */

    // IMU 
    IMUManager* inertialSensor;                /**< IMU sensor manager */
    IMUData_t* imuData;                         /**< Latest IMU data */
    std::vector<IMUData_t*> IMUDataSamples;      /**< Sampled IMU data */

     // IMU 
    int loraMessageNumber = 1;      /**< interger counting the number of messages sent by LoRa */
    LoRaClass * LoRaTransciever;    /**< LoRa Module transceiver */

    // EEPROM
    EEPROMManager* memory;                     /**< EEPROM storage manager */

    // MN Module 
    CellularModule* cellularTransceiver;        /**< Cellular transceiver */
    RemoteServerInformation* serverTargetted;  /**< contains information of the server Target  */
    CellInformation* currentCellInformation;   /**< Current cell information */
    std::vector<CellInformation*> neighborsCellInformation;    /**< Neighbor cell data */

    // GNSS Module 
    GNSSModule* currentGNSSModule;      /**< GNSS receiver module */
    GNSSData* currentGNSSdata;         /**< Latest GNSS data */

    // Battery
    BatteryData* batteryStatus;        /**< Current battery status */

   // Message Handlers
    //MessageHandler* encrypter;          /**< Encryption handler */
    MessageHandler* encrypterBase64;    /**< Base64 encryption handler */
    MessageHandler* authgen;            /**< Authentication generator */
    MessageHandler* ckgen;             /**< Checksum generator */
    MessageHandler* checksumVerifier;  /**< Checksum verifier */
    MessageHandler* authVer;           /**< Authentication verifier */
    //MessageHandler* decrypter;         /**< Decryption handler */
    MessageHandler* decrypterBase64;   /**< Base64 decryption handler */

};


//=====[#include guards - end]=================================================

#endif // _TRACKER_H_
