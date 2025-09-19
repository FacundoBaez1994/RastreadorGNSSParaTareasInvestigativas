//=====[#include guards - begin]===============================================

#ifndef _EXCHANGING_LORA_MESSAGE_H_
#define _EXCHANGING_LORA_MESSAGE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "Non_Blocking_Delay.h"
#include "TrackerBaseState.h"
#include "Tracker.h"
//#include "WaitingAcknowledgement.h"

//=====[Declaration of public data types]======================================
class Tracker; ///< Forward declaration to avoid circular dependency

/**
 * @enum ExchangingLoRaMessagesStatus_t
 * @brief Represents the internal states of the LoRa message exchange process.
 */
typedef enum {
    INITIALIZE_TRANSCIEVER, ///< Initializes the LoRa transceiver before communication.
    SENDING_MESSAGE,        ///< Sends a message in chunks over LoRa.
    WAITING_FOR_ACK,        ///< Waits for an acknowledgment from the receiver.
} ExchangingLoRaMessagesStatus_t;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class ExchangingLoRaMessages  : public TrackerBaseState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructs the ExchangingLoRaMessages state.
     * 
     * @param tracker Pointer to the Tracker object that owns this state.
     * @param trackerStatus Current status of the tracker when entering this state.
     */
    ExchangingLoRaMessages  (Tracker * tracker, trackerStatus_t trackerStatus);

    /**
     * @brief Destroys the ExchangingLoRaMessages state.
     * Releases allocated resources such as the NonBlockingDelay instance.
     */
    virtual ~ExchangingLoRaMessages  ();

    /**
     * @brief Manages the process of sending and receiving LoRa messages.
     * This function should be called periodically.  
     * It handles the entire process of:
     * - Initializing the LoRa transceiver.
     * - Sending a message in multiple chunks if needed.
     * - Waiting for an acknowledgment from the remote device.
     * - Processing the acknowledgment and transitioning to the next state.
     * @param LoRaModule Pointer to the LoRaClass object used for communication.
     * @param message Pointer to the message to be transmitted.
     * @param receivedMessage Pointer to the buffer where received message will be stored.
     */
    void exchangeMessages (LoRaClass * LoRaModule, char * message, char * receivedMessage );
private:
    Tracker * tracker;  ///< Pointer to the tracker that owns this state.
    trackerStatus_t currentStatus; ///< Current tracker status when in this state.
    NonBlockingDelay * backoffTime; ///< Timer used for delays and timeouts.
    ExchangingLoRaMessagesStatus_t currentExchangingLoRaMessagesStatus; ///< Current internal state of the exchange process.
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _EXCHANGING_LORA_MESSAGE_H_