//=====[#include guards - begin]===============================================

#ifndef _CONFIGURE_SSL
#define _CONFIGURE_SSL

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
    SETTING_SSL_CONTEXT,
    SETTING_SSL_VERSION,
    SETTING_SSL_CIPHER_SUITE,
    SETTING_SSL_LEVEL,
} SSLConfigStatus_t;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class ConfigureSSL : public TransceiverState {
public:
//=====[Declaration of public methods]=========================================
    ConfigureSSL();
    ConfigureSSL (CellularModule * mobileModule);
    virtual ~ConfigureSSL ();
    virtual void enableTransceiver ();
    virtual CellularTransceiverStatus_t exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable);
private:
    CellularModule * mobileNetworkModule;
    bool readyToSend;
    int Attempts; 
    int maxAttempts; 
    SSLConfigStatus_t currentStatus;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CONFIGURE_PDP_FOR_HTTP_H_