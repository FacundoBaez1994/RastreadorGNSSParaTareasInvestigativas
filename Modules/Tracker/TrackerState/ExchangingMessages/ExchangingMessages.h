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
class Tracker; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class ExchangingMessages : public TrackerBaseState {
public:
//=====[Declaration of public methods]=========================================
    ExchangingMessages (Tracker * tracker, trackerStatus_t trackerStatus);
    virtual ~ExchangingMessages ();
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);
    virtual void exchangeMessages (CellularModule * cellularTransceiver,
    char * message, TcpSocket * socketTargetted, char * receivedMessage );
private:
//=====[Declaration of privates atributes]=========================================
    Tracker * tracker;
    trackerStatus_t currentStatus;
//=====[Declaration of privates methods]=========================================
    bool extractField(const char* json, const char* key, char* output, size_t maxLen);
    bool parseLatencyLevel(const char* latencyStr, LatencyLevel_t *  newLatencyLevel);
    bool parseOperationMode(const char* operationModeStr, OperationMode_t * newOperationMode);
    

};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _EXCHANGING_MESSAGES_H_