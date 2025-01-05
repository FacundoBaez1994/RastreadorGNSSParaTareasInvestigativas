//=====[#include guards - begin]===============================================

#ifndef _TRACKER_H_
#define _TRACKER_H_

#include "mbed.h"
#include "GNSSModule.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "string.h"
#include "CellularModule.h"
#include "LoRa.h"
#include "RFTransicieverState.h"
#include "SendingMessage.h"

#include "MessageHandler.h"
#include "MessageHandlerStatus.h"
#include "AuthenticationGenerator.h"
#include "AuthenticationVerifier.h"
#include "BaseMessageHandler.h"
#include "ChecksumVerifier.h"
#include "ChecksumGenerator.h"
#include "Decrypter.h"
#include "Encrypter.h"


//=====[Declaration of public defines]=========================================


//=====[Declaration of public data types]======================================

//=====[Declaration of public classes]=========================
/*
 * Class implementation for a GPS tracker
 * High hierarchy class
 * it will be instantiated and used from the main function
 */
class Tracker {
public:
    Tracker ();
    virtual ~Tracker ();
    void update();
    void changeState  (RFTransicieverState * newState);
    bool prepareMessage (char * messageOutput);
    bool processMessage (char * incomingMessage);

private:

    RFTransicieverState * RFState;
    int deviceId;
    int messageNumber;

    bool checkMessageIntegrity ( char * messageSent, char *messageReceived);
    char* formMessage (GNSSData * GNSSInfo);
    char* formMessage(CellInformation* aCellInfo, std::vector<CellInformation*> 
    &neighborsCellInformation, BatteryData  * batteryStatus); 
    char* formMessage(CellInformation* aCellInfo, GNSSData* GNSSInfo, BatteryData  * batteryStatus);
    CellularModule* cellularTransceiver;
    TcpSocket * socketTargetted;
    CellInformation * currentCellInformation; 
    LoRaClass * LoRaTransciever;

    GNSSModule* currentGNSSModule;
    GNSSData * currentGNSSdata;
    NonBlockingDelay * timeout;
    BatteryData  * batteryStatus;

    // Message Handlers
    MessageHandler * encrypter;
    MessageHandler * authgen;
    MessageHandler * ckgen;
    MessageHandler * checksumVerifier;
    MessageHandler * authVer;
    MessageHandler * decrypter;
};


//=====[#include guards - end]=================================================

#endif // _TRACKER_H_
