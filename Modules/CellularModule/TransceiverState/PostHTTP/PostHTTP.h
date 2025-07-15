//=====[#include guards - begin]===============================================

#ifndef _POST_HTTP_H_
#define _POST_HTTP_H_

//==================[Libraries]===============================================

#include "CellularModule.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransceiverState.h"
#include "Non_Blocking_Delay.h"
#include "TransceiverUnavailable.h"
#include "DeactivatePDP.h"


//=====[Declaration of public data types]======================================
class CellularModule; //debido a declaracion adelantada
struct TcpSocket;

typedef enum {
    POST_OK,
    KEEP_TRYING_TO_POST,
    POST_FAILURE,
} PostResult_t;

typedef enum {
    SETTING_URL,
    POSTING_DATA,
    READING_DATA,
} PostStatus_t;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class PostHTTP : public TransceiverState {
public:
//=====[Declaration of public methods]=========================================
    PostHTTP();
    PostHTTP (CellularModule * mobileModule);
    virtual ~PostHTTP ();
    virtual void enableTransceiver ();
    virtual CellularTransceiverStatus_t exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable);
private:
    CellularModule * mobileNetworkModule;
    bool readyToSend;
    int Attempts; 
    int maxAttempts; 
    PostStatus_t currentStatus;
//=====[Declaration of privates atributes]=========================================
    PostResult_t checkHTTPPostResult(char * responseBuffer);

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _POST_HTTP_H_