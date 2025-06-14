//=====[Libraries]=============================================================

#include "Tracker.h"
#include "Debugger.h" // due to global usbUart
#include "CalibratingInertialSensor.h"
#include "SavingMessage.h"


//=====[Declaration of private defines]========================================
#define LATENCY        5000
#define TIMEOUT_MS     5000
#define POWERCHANGEDURATION  700

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public methods]===================================
/** 
* @brief Contructor method creates a new trackerGPS instance ready to be used
*/
Tracker::Tracker () {
    Watchdog &watchdog = Watchdog::get_instance(); // singletom
    watchdog.start(TIMEOUT_MS);
    char StringToSendUSB [50] = "Tracker initialization";
    uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only

    this->latency = new NonBlockingDelay (LATENCY);
    this->cellularTransceiver = new CellularModule ( );
    this->currentGNSSModule = new GNSSModule (this->cellularTransceiver->getPowerManager()
    , this->cellularTransceiver->getATHandler());
    //both share the same power manager and ATHandler (uart)

    this->socketTargetted = new TcpSocket;
    this->socketTargetted->IpDirection = new char[16]; // 
    strcpy(this->socketTargetted->IpDirection, "186.19.62.251");
    this->socketTargetted->TcpPort = 123;

    this->currentCellInformation = new CellInformation;
    this->currentCellInformation->timestamp  = new char [20];
    this->currentCellInformation->band = new char [20];

    this->currentGNSSdata = new GNSSData;
    this->batteryStatus = new BatteryData;

    this->imuData = new IMUData_t;

    this->inertialSensor = new IMUManager (); 
    this->memory = new EEPROMManager ();

    //this->currentState =  new CalibratingInertialSensor (this);
    this->currentState =  new SavingMessage (this);

    this->jwt = new CustomJWT (this->JWTKey, 256);
    this->encrypter = new Encrypter ();
    this->encrypterBase64 = new EncrypterBase64 ();
    this->authgen = new AuthenticationGenerator ();
    this->ckgen = new ChecksumGenerator ();

    this->checksumVerifier = new ChecksumVerifier ();
    this->authVer = new AuthenticationVerifier ();
    this->decrypter = new Decrypter ();
    this->decrypterBase64 = new DecrypterBase64 ();


    while (! this->memory->clearAll()) {
        
    }
    snprintf(StringToSendUSB, sizeof(StringToSendUSB), "STRINGS LIMPIADAS\n\r");
    uartUSB.write(StringToSendUSB, strlen(StringToSendUSB));
}


Tracker::~Tracker() {
    delete this->imuData;
    this->imuData = nullptr;

    delete[] this->currentCellInformation->timestamp;
    this->currentCellInformation->timestamp = nullptr;
    delete[] this->currentCellInformation->band;
    this->currentCellInformation->band = nullptr;
    delete this->currentCellInformation;
    this->currentCellInformation = nullptr;
    delete this->currentGNSSdata;
    delete[] this->socketTargetted->IpDirection; // Libera la memoria asignada a IpDirection
    this->socketTargetted->IpDirection = nullptr;
    delete this->socketTargetted; // Libera la memoria asignada al socketTargetted
    this->socketTargetted = nullptr;
    delete this->latency;
    this->latency = nullptr; 
    delete this->currentGNSSModule;
    this->currentGNSSModule = nullptr;
    delete this->cellularTransceiver;
    this->cellularTransceiver = nullptr;

    delete this->inertialSensor;
    this->inertialSensor = nullptr;
    delete this->memory;
    this->memory = nullptr;

    delete this->encrypter;
    this->encrypter = nullptr;
    delete this->encrypterBase64;
    this->encrypterBase64 = nullptr;
    delete this->authgen;
    this->authgen = nullptr;
    delete this->ckgen;
    this->ckgen = nullptr;
    delete this->checksumVerifier;
    this->checksumVerifier = nullptr;
    delete this->authVer;
    this->authVer = nullptr;
    delete this->decrypter;
    this->decrypter = nullptr;
    delete this->decrypterBase64;
    this->decrypterBase64 = nullptr;
}


/** 
* @brief Main rutine of the tracker device
*   
*
*/
void Tracker::update () {
    static char formattedMessage [2048] = "MNMN,722,7,11A4,62A840F,-55.00,7,1,2850,LTE BAND 7,1,0,89,0,0.01,-0.00,-0.01,168.27,-1.52,-5.91|2,722,34,13F5,1583,-62.00|2,722,34,13F5,1B48,-60.00|2,722,34,13F5,1581,57.00|2,722,34,13F5,DC3,-46.00|2,722,34,13F5,1582,-44.00|4,722,34,3B05,7A3B603,-73.00|4,722,34,3B05,7A3B600,-92.00|4,722,34,3B05,7A3B600,-92.00|4,722,34,3B05,7A1C90E,-94.00|4,722,34,3B0C,7D44708,-122.00|2,722,310,1BD5,41E9,-34.00|2,722,310,1BD5,65AF,-33.00|2,722,310,1BD5,6A05,-28.00|3,722,310,4317,16438A6,-75.00";
    //static char formattedMessage [2048];
    static char receivedMessage [2048];


    //static std::vector<CellInformation*> neighborsCellInformation;
    static int numberOfNeighbors = 0;
    Watchdog &watchdog = Watchdog::get_instance(); // singleton
    watchdog.kick();
    this->currentState->awake(this->cellularTransceiver, this->latency);
    this->currentState->calibrateIMU (this->inertialSensor);
    this->currentState->updatePowerStatus (this->cellularTransceiver, this->batteryStatus);
    this->currentState->obtainGNSSPosition (this->currentGNSSModule, this->currentGNSSdata);
    this->currentState->connectToMobileNetwork (this->cellularTransceiver,
    this->currentCellInformation);
    this->currentState->obtainNeighborCellsInformation (this->cellularTransceiver, 
    this->neighborsCellInformation, numberOfNeighbors );
    this->currentState->obtainInertialMeasures(this->inertialSensor, this->imuData);
    this->currentState->formatMessage (formattedMessage, this->currentCellInformation,
    this->currentGNSSdata, this->neighborsCellInformation, this->imuData, this->batteryStatus); 
    this->currentState->exchangeMessages (this->cellularTransceiver,
    formattedMessage, this->socketTargetted, receivedMessage ); // agregar modulo LoRa al argumento
    this->currentState->saveMessage(this->memory, formattedMessage);
    this->currentState->loadMessage(this->memory, this->currentCellInformation,
    this->currentGNSSdata, this->neighborsCellInformation, this->imuData, this->batteryStatus);
    this->currentState->goToSleep (this->cellularTransceiver);
    watchdog.kick();
    
}


void Tracker::changeState  (TrackerState * newTrackerState) {
    delete this->currentState;
    this->currentState = newTrackerState;
}


bool Tracker::encryptMessage (char * message, unsigned int messageSize) {
    this->encrypterBase64->setNextHandler(nullptr);
    if (this->encrypterBase64->handleMessage (message, messageSize) == MESSAGE_HANDLER_STATUS_PROCESSED) {
        return true;
    } else {
        return false;
    }
}

bool Tracker::decryptMessage (char * message, unsigned int messageSize) {
    this->decrypterBase64->setNextHandler(nullptr);
    if (this->decrypterBase64->handleMessage (message, messageSize) == MESSAGE_HANDLER_STATUS_PROCESSED) {
        return true;
    } else {
        return false;
    }
}

bool Tracker::prepareLoRaMessage (char * message, unsigned int messageSize) {
    this->encrypter->setNextHandler(this->authgen)->setNextHandler(this->ckgen);
    if (this->encrypter->handleMessage (message, messageSize) == MESSAGE_HANDLER_STATUS_PROCESSED) {
        return true;
    } else {
        return false;
    }
}

bool Tracker::processLoRaMessage (char * message, unsigned int messageSize) {
    this->checksumVerifier->setNextHandler(this->authVer)->setNextHandler(this->decrypter);
    if (this->checksumVerifier->handleMessage (message, messageSize) == MESSAGE_HANDLER_STATUS_PROCESSED) {
        return true;
    } else {
        return false;
    }
}


void printData(char *data, size_t dataLen) {
    char logMessage [150];
    snprintf(logMessage, sizeof(logMessage), "\n\rData: = %s \n\r", data);
    uartUSB.write(logMessage , strlen(logMessage ));
      snprintf(logMessage, sizeof(logMessage), "\n\rData Length: = %i \n\r", dataLen);
    uartUSB.write(logMessage , strlen(logMessage ));
}


void Tracker::encodeJWT(char * payloadToJWT, char * jwtEncoded) {
    char logMessage [250];
    this->jwt->allocateJWTMemory();
    snprintf(logMessage, sizeof(logMessage), "Generating a JWT"); 
    uartUSB.write(logMessage , strlen(logMessage ));

    this->jwt->encodeJWT( payloadToJWT);

    snprintf(logMessage, sizeof(logMessage), "Header Info"); 
    uartUSB.write(logMessage , strlen(logMessage ));
    printData(this->jwt->header, this->jwt->headerLength);

    snprintf(logMessage, sizeof(logMessage), "Payload Info"); 
    uartUSB.write(logMessage , strlen(logMessage ));
    printData(this->jwt->payload, this->jwt->payloadLength);

    snprintf(logMessage, sizeof(logMessage), "Signaure Info"); 
    uartUSB.write(logMessage , strlen(logMessage ));;
    printData(this->jwt->signature, this->jwt->signatureLength);

    snprintf(logMessage, sizeof(logMessage), "Final Output Info"); 
    uartUSB.write(logMessage , strlen(logMessage ));;
    printData(this->jwt->out, this->jwt->outputLength);

    strcpy (jwtEncoded, this->jwt->out);
    jwtEncoded [this->jwt->outputLength] = '\0';
    this->jwt->clear();
}



void Tracker::decodeJWT (char * jwtToDecode, char * payloadRetrived) {
    char logMessage [150];
    this->jwt->allocateJWTMemory();
    //Decode the JWT
    snprintf(logMessage, sizeof(logMessage), "Decoding and verifying the JWT\n\r");
    uartUSB.write(logMessage , strlen(logMessage ));
    snprintf(logMessage, sizeof(logMessage), "JWT Decode ended with result: \n\r");
    uartUSB.write(logMessage , strlen(logMessage ));
    //Code 0: Decode success \n Code 1: Memory not allocated \n Code 2: Invalid JWT \n Code 3: Signature Mismatch
    snprintf(logMessage, sizeof(logMessage), "\n\rCode result = %i \n\r", this->jwt->decodeJWT(jwtToDecode)); 
    uartUSB.write(logMessage , strlen(logMessage ));

    snprintf(logMessage, sizeof(logMessage), "Header Info"); 
    uartUSB.write(logMessage , strlen(logMessage ));
    printData(this->jwt->header, this->jwt->headerLength);

    snprintf(logMessage, sizeof(logMessage), "Payload Info"); 
    uartUSB.write(logMessage , strlen(logMessage ));
    printData(this->jwt->payload, this->jwt->payloadLength);

    snprintf(logMessage, sizeof(logMessage), "Signaure Info"); 
    uartUSB.write(logMessage , strlen(logMessage ));
    printData(this->jwt->signature, this->jwt->signatureLength);
    
    strcpy (payloadRetrived, this->jwt->payload);
    this->jwt->clear();
}


//=====[Implementations of private methods]==================================