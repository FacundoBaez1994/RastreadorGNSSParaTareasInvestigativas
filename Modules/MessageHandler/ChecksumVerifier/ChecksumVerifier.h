//=====[#include guards - begin]===============================================

#ifndef _CHECKSUM_VERIFIER_H_
#define _CHECKSUM_VERIFIER_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "BaseMessageHandler.h"
#include <string.h>
#include <stdio.h>
#include <MbedCRC.h>
 
//=====[Declaration of public data types]======================================
class MessageHandler; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  
 * 
 */
class ChecksumVerifier : public BaseMessageHandler {
public:
//=====[Declaration of public methods]=========================================
    ChecksumVerifier();
    virtual ~ChecksumVerifier ();
    virtual MessageHandlerStatus_t handleMessage (char * message) override;

private:
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CHECKSUM_VERIFIER_H_