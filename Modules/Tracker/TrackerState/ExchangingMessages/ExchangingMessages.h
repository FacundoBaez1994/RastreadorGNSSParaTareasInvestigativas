//=====[#include guards - begin]===============================================
#ifndef _EXCHANGING_MESSAGES_H_
#define _EXCHANGING_MESSAGES_H_

//==================[Libraries]===============================================
#include "mbed.h"
#include "arm_book_lib.h"
#include "TrackerBaseState.h"
#include "Tracker.h"
#include "JWTManager.h"

//=====[Declaration of public data types]======================================
class Tracker; ///< Forward declaration due to circular dependency

//=====[Declaration of public classes]=========================================
/**
 * @brief State class implementing message exchange with the server.
 * @details Implements the state machine behavior for sending messages via
 * cellular transceiver and receiving/parsing server responses. Manages
 * message retransmission, parsing JSON payload fields, and transitioning
 * to subsequent states based on success or failure.
 */
class ExchangingMessages : public TrackerBaseState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructor
     * @param tracker Pointer to the Tracker object managing states and data
     * @param trackerStatus Current tracker status to manage state transitions
     */
    ExchangingMessages (Tracker* tracker, trackerStatus_t trackerStatus);

    /**
     * @brief Destructor
     */
    virtual ~ExchangingMessages ();

    /**
     * @brief Updates power status of the cellular transceiver
     * @param cellularTransceiver Pointer to the CellularModule instance
     * @param currentBatteryStatus Pointer to battery status (currently unused)
     */
    virtual void updatePowerStatus (CellularModule* cellularTransceiver, BatteryData* currentBatteryStatus);

    /**
     * @brief Exchanges messages with the server via cellular transceiver
     * @param cellularTransceiver Pointer to the CellularModule instance
     * @param message Pointer to the message buffer to send
     * @param socketTargetted Pointer to the TCP socket to send through
     * @param receivedMessage Pointer to buffer for storing received server response
     * @details Manages message sending, response reception, JSON parsing of
     * fields such as success status, latency, operation mode, and silent timer.
     * Transitions to appropriate next states depending on communication success.
    */
    virtual void exchangeMessages (CellularModule* cellularTransceiver,
    char* message, TcpSocket* socketTargetted, char* receivedMessage );
private:
//=====[Declaration of privates atributes]=========================================
    Tracker* tracker; ///< Pointer to the tracker instance for state transitions
    trackerStatus_t currentStatus;  ///< Current operational status
//=====[Declaration of privates methods]=========================================
    /**
     * @brief Extracts the value of a JSON field from a string
     * @param json Pointer to JSON formatted string
     * @param key Key name to find in the JSON string (including quotes)
     * @param output Output buffer to copy extracted field value
     * @param maxLen Maximum length of output buffer
     * @return true if key and value were found and extracted, false otherwise
     */
    bool extractField(const char* json, const char* key, char* output, size_t maxLen);

    /**
     * @brief Parses a latency level string to a LatencyLevel_t enum
     * @param latencyStr String representing latency level (e.g. "ELL", "VLL")
     * @param newLatencyLevel Pointer to output LatencyLevel_t variable
     * @return true if string matches known latency levels, false otherwise
     */
    bool parseLatencyLevel(const char* latencyStr, LatencyLevel_t*  newLatencyLevel);

    /**
     * @brief Parses an operation mode string to an OperationMode_t enum
     * @param operationModeStr String representing operation mode ("NOPM", "POPM", "SOPM")
     * @param newOperationMode Pointer to output OperationMode_t variable
     * @return true if string matches known operation modes, false otherwise
     */
    bool parseOperationMode(const char* operationModeStr, OperationMode_t* newOperationMode);

};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _EXCHANGING_MESSAGES_H_