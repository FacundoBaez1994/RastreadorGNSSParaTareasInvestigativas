//=====[#include guards - begin]===============================================

#ifndef _CONFIGURE_PDP_FOR_HTTP_H_
#define _CONFIGURE_PDP_FOR_HTTP_H_

//==================[Libraries]===============================================

#include "CellularModule.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransceiverState.h"
#include "Non_Blocking_Delay.h"
#include "TransceiverUnavailable.h"
#include "PostHTTP.h"

//=====[Declaration of public data types]======================================
class CellularModule; //debido a declaracion adelantada
struct TcpSocket;

typedef enum {
    ASSING_PDPCONTEXT_TO_HTTP_STACK,
    CONFIGURING_PDP_PROFILE,
    ACTIVATING_PDP_CONTEXT,
    QUERING_PDP_CONTEXT,
    CONFIGURING_SERVER_DNS,
    PING_SERVER_DNS,
} PDPConfigStatus_t;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class ConfigurePDPForHTTP : public TransceiverState {
public:
//=====[Declaration of public methods]=========================================
    ConfigurePDPForHTTP();
    ConfigurePDPForHTTP (CellularModule * mobileModule);
    virtual ~ConfigurePDPForHTTP ();
    virtual void enableTransceiver ();
    virtual CellularTransceiverStatus_t exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable);
private:
    CellularModule * mobileNetworkModule;
    bool readyToSend;
    int Attempts; 
    int maxAttempts; 
    PDPConfigStatus_t currentStatus;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CONFIGURE_PDP_FOR_HTTP_H_