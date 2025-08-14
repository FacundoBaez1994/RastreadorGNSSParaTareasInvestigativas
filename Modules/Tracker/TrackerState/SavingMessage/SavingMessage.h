//=====[#include guards - begin]===============================================

#ifndef _SAVING_MESSAGE_H_
#define _SAVING_MESSAGE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "TrackerBaseState.h"
#include "Tracker.h"

//=====[Declaration of public data types]======================================
class Tracker; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/**
 * @class SavingMessage
 * @brief State of the Tracker responsible for encrypting and storing messages in EEPROM.
 * This state is part of the Tracker's State design pattern implementation.  
 * It handles the final stage of preparing an outgoing message, which includes:
 * - Encrypting the provided message.
 * - Saving the encrypted data into EEPROM.
 * - Logging the process via the debug interface.
 * Upon completion, the Tracker transitions to another state such as 
 * GoingToSleep, depending on the result of the operation.
 */
class  SavingMessage : public TrackerBaseState {
public:
//=====[Declaration of public methods]=========================================

    /**
     * @brief Constructs a SavingMessage state.
     * @param tracker Pointer to the Tracker context that owns this state.
     */
    SavingMessage (Tracker * tracker);

    /**
     * @brief Destructor for SavingMessage.
     * 
     * Releases internal resources and resets the tracker pointer.
    */
    virtual ~SavingMessage ();

    /**
     * @brief Encrypts and stores a message in EEPROM while in the SavingMessage state.
     * @details This method performs the following sequence:
     * - Encrypts the provided message using the tracker's encryption routine.
     * - Attempts to store the encrypted message in EEPROM via the given EEPROMManager.
     * - Logs the result over the USB debug interface.
     * - Handles two main outcomes:
     *   - **PUSHOK**: Message successfully saved; transitions to the GoingToSleep state.
     *   - **NOMEMORY**: EEPROM full; logs the error and transitions to the GoingToSleep state.
     * This function is designed to be invoked periodically until encryption 
     * and storage are completed, or an error occurs.
     * @param memory Pointer to the EEPROMManager instance responsible for EEPROM operations.
     * @param message Pointer to the null-terminated C-string containing the message to save.
     */
    virtual void saveMessage (EEPROMManager * memory, char * message);
private:
    Tracker * tracker; /**< Pointer to the Tracker context instance. */

//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _SAVING_MESSAGE_H_