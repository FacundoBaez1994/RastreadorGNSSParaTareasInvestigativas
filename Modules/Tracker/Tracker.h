//=====[#include guards - begin]===============================================

#ifndef _TRACKER_H_
#define _TRACKER_H_

#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "CellularModule.h"
#include "EEPROMManager.h"
#include "GNSSModule.h"
#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "string.h"
#include "TrackerState.h"
#include "TrackerStatus.h"

#include "IMUManager.h"
#include "EEPROMManager.h"

#include "MessageHandler.h"
#include "MessageHandlerStatus.h"
#include "AuthenticationGenerator.h"
#include "AuthenticationVerifier.h"
#include "BaseMessageHandler.h"
#include "ChecksumVerifier.h"
#include "ChecksumGenerator.h"
#include "Decrypter.h"
#include "Encrypter.h"
#include "DecrypterBase64.h"
#include "EncrypterBase64.h"
#include <CustomJWT.h>


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
    void changeState  (TrackerState * newTrackerState);
    void encodeJWT(char * payloadToJWT, char * jwtEncoded);
    bool decodeJWT (char * jwtToDecode, char * payloadRetrived);
    bool encryptMessage (char * message, unsigned int messageSize);
    bool decryptMessage (char * message, unsigned int messageSize);

    bool prepareLoRaMessage (char * message, unsigned int messageSize);
    bool processLoRaMessage (char * message, unsigned int messageSize);

    
private:

    TrackerState * currentState;

  // IMU 
    IMUManager * inertialSensor;
    IMUData_t * imuData;
    std::vector<IMUData_t*> IMUDataSamples;


    EEPROMManager * memory;
    
    CellularModule* cellularTransceiver;
    TcpSocket * socketTargetted;
    CellInformation * currentCellInformation; 
    std::vector<CellInformation*> neighborsCellInformation;

    GNSSModule* currentGNSSModule;
    GNSSData * currentGNSSdata;

    NonBlockingDelay * latency;

    BatteryData  * batteryStatus;


   // Message Handlers
    MessageHandler * encrypter;
    MessageHandler * encrypterBase64;
    MessageHandler * authgen;
    MessageHandler * ckgen;
    MessageHandler * checksumVerifier;
    MessageHandler * authVer;
    MessageHandler * decrypter;
    MessageHandler * decrypterBase64;
    CustomJWT * jwt;
    char JWTKey [40] = "a-string-secret-at-least-256-bits-long";

};


//=====[#include guards - end]=================================================

#endif // _TRACKER_H_
