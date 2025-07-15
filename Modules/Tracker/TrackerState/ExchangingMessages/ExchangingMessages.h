//=====[#include guards - begin]===============================================

#ifndef _EXCHANGING_MESSAGES_H_
#define _EXCHANGING_MESSAGES_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "TrackerBaseState.h"
#include "Tracker.h"

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
    Tracker * tracker;
    trackerStatus_t currentStatus;
    bool extractField(const char* json, const char* key, char* output, size_t maxLen);
  

//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _EXCHANGING_MESSAGES_H_