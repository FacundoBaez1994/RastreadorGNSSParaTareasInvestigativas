//=====[Libraries]=============================================================
#include "Tracker.h"
#include "Debugger.h" // due to global usbUart
#include "CalibratingInertialSensor.h"
#include "IMUManager.h"
#include "SavingMessage.h"
#include "LoadingMessage.h"


//=====[Declaration of private defines]========================================
#define EXTREMELY_LOW_LATENCY_MS   20000          // 20 seconds
#define VERY_LOW_LATENCY_MS        (1 * 60 * 1000)   // 1 minutes
#define LOW_LATENCY_MS             (10 * 60 * 1000)  // 10 minutes
#define MEDIUM_LATENCY_MS          (30 * 60 * 1000)  // 30 minutes
#define HIGH_LATENCY_MS            (60 * 60 * 1000)  // 1 hour
#define VERY_HIGH_LATENCY_MS       (6 * 60 * 60 * 1000)   // 6 hours
#define EXTREMELY_HIGH_LATENCY_MS  (24 * 60 * 60 * 1000)  // 24 hours

#define EXTREMELY_LOW_LATENCY_KEEP_ALIVE_MULTIPLIER   180    // 1 hour
#define VERY_LOW_LATENCY_KEEP_ALIVE_MULTIPLIER        120   // 2 hour
#define LOW_LATENCY_KEEP_ALIVE_MULTIPLIER            24  // 4 hours
#define MEDIUM_LATENCY_KEEP_ALIVE_MULTIPLIER          12 // 6 hours
#define HIGH_LATENCY_KEEP_ALIVE_MULTIPLIER            8  // 8 hour
#define VERY_HIGH_LATENCY_KEEP_ALIVE_MULTIPLIER       2  // 12 hours
#define EXTREMELY_HIGH_LATENCY_KEEP_ALIVE_MULTIPLIER  1 // 24 hours


//#define HOUR_MS  (1 * 60 * 60 * 1000)  // 1 hours
#define HOUR_MS  (3 * 60 * 1000)  // 3 min TEST ONLY

#define TIMEOUT_WATCHDOG_TIMER_MS     5000
#define POWERCHANGEDURATION  700
#define TIME_BETWEEN_IMU_SAMPLES 10 // 10 seconds

#define URL_PATH_CHANNEL "https://intent-lion-loudly.ngrok-free.app/api/canal/envio"
#define CURRENT_DEVICE_IDENTIFIER "device/tracker-001"


#define MOVEMENT_EVENT_STRING_MOVING "MOVE"
#define MOVEMENT_EVENT_STRING_PARKING "PARK"
#define MOVEMENT_EVENT_STRING_STOPPED "STOP"
#define MOVEMENT_EVENT_STRING_MOVEMENT_RESTARTED "MVRS"

#define ACK_VALID_1 "ACK"
#define ACK_VALID_2 "ACK\r"
#define ACK_VALID_3 "ACK\r\n"

//=====[Log Message Definitions]==============================================
#define LOG_MESSAGE_TRACKER_INIT                   "Tracker initialization\r\n"
#define LOG_MESSAGE_TRACKER_INIT_LEN               (sizeof(LOG_MESSAGE_TRACKER_INIT) - 1)

#define LOG_MESSAGE_MOVEMENT_RESTARTED             "\n\rUpdate movement event: MOVEMENT_RESTARTED\n\r"
#define LOG_MESSAGE_MOVEMENT_RESTARTED_LEN         (sizeof(LOG_MESSAGE_MOVEMENT_RESTARTED) - 1)

#define LOG_MESSAGE_PARKING                        "\n\rUpdate movement event: PARKING\n\r"
#define LOG_MESSAGE_PARKING_LEN                    (sizeof(LOG_MESSAGE_PARKING) - 1)

#define LOG_MESSAGE_MOVING                         "\n\rUpdate movement event: MOVING\n\r"
#define LOG_MESSAGE_MOVING_LEN                     (sizeof(LOG_MESSAGE_MOVING) - 1)

#define LOG_MESSAGE_STOPPED                        "\n\rUpdate movement event: STOPPED\n\r"
#define LOG_MESSAGE_STOPPED_LEN                    (sizeof(LOG_MESSAGE_STOPPED) - 1)

#define LOG_MESSAGE_NEW_LATENCY                    "\n\rNew latency set: "
#define LOG_MESSAGE_NEW_LATENCY_LEN                (sizeof(LOG_MESSAGE_NEW_LATENCY) - 1)

#define LOG_MESSAGE_NEW_KEEPALIVE_LATENCY          "\n\rNew keep alive latency set: "
#define LOG_MESSAGE_NEW_KEEPALIVE_LATENCY_LEN      (sizeof(LOG_MESSAGE_NEW_KEEPALIVE_LATENCY) - 1)

#define LOG_MESSAGE_ACK_OK                         "OK\r\n"
#define LOG_MESSAGE_ACK_OK_LEN                     (sizeof(LOG_MESSAGE_ACK_OK) - 1)

#define LOG_MESSAGE_ACK_INVALID                    "ACK invalid\r\n"
#define LOG_MESSAGE_ACK_INVALID_LEN                (sizeof(LOG_MESSAGE_ACK_INVALID) - 1)


#define LOG_MESSAGE_DEVICE_ID_RECEIVED_PREFIX     "Device ID Received: %lld\r\n"
#define LOG_MESSAGE_DEVICE_ID_RECEIVED_LEN        (sizeof(LOG_MESSAGE_DEVICE_ID_RECEIVED_PREFIX) - 1)

#define LOG_MESSAGE_MESSAGE_NUMBER_RECEIVED_PREFIX "Message Number Received: %d\r\n"
#define LOG_MESSAGE_MESSAGE_NUMBER_RECEIVED_LEN    (sizeof(LOG_MESSAGE_MESSAGE_NUMBER_RECEIVED_PREFIX) - 1)

#define LOG_MESSAGE_PAYLOAD_RECEIVED_PREFIX       "Payload Received: %s\r\n"
#define LOG_MESSAGE_PAYLOAD_RECEIVED_PREFIX_LEN    (sizeof(LOG_MESSAGE_PAYLOAD_RECEIVED_PREFIX) - 1)


//=====[enables]=====================================
#define ENABLE_UART_USB_LOG

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public methods]===================================
Tracker::Tracker () {
    #ifdef ENABLE_UART_USB_LOG
        uartUSB.enable();
    #else
        uartUSB.disable();
    #endif

    Watchdog &watchdog = Watchdog::get_instance(); // singletom
    watchdog.start(TIMEOUT_WATCHDOG_TIMER_MS);

    uartUSB.write (LOG_MESSAGE_TRACKER_INIT , LOG_MESSAGE_TRACKER_INIT_LEN);  // debug only

    this->deviceIdentifier = new char [DEVICE_IDENTIFIER_BUFFER_SIZE];
    strcpy (this->deviceIdentifier, CURRENT_DEVICE_IDENTIFIER);
    this->prevChainHash = new char [HASH_BASE_64_BUFFER_SIZE];
    strcpy (this->prevChainHash, "-");
    this->currChainHash = new char [HASH_BASE_64_BUFFER_SIZE];
    strcpy (this->currChainHash, "-");

    this->currentOperationMode = NORMAL_OPERATION_MODE;
    //this->currentOperationMode = PERSUIT_OPERATION_MODE;
    this->latency = new NonBlockingDelay (EXTREMELY_LOW_LATENCY_MS);
    this->silentKeepAliveTimer = new NonBlockingDelay (HOUR_MS);
    this->cellularTransceiver = new CellularModule ( );
    this->currentGNSSModule = new GNSSModule (this->cellularTransceiver->getPowerManager()
    , this->cellularTransceiver->getATHandler());
    //both share the same power manager and ATHandler (uart)

    this->serverTargetted = new RemoteServerInformation;
    this->serverTargetted->url = new char[URL_SERVER_BUFFER_SIZE]; // 
    strcpy (this->serverTargetted->url, URL_PATH_CHANNEL);

    this->currentCellInformation = new CellInformation;
    this->currentCellInformation->timestamp  = new char [TIMESTAMP_BUFFER_SIZE];
    this->currentCellInformation->band = new char [BAND_STRING_BUFFER_SIZE];

    this->currentGNSSdata = new GNSSData;
    this->batteryStatus = new BatteryData;

    this->imuData = new IMUData_t;
    this->imuData->timestamp = new char [TIMESTAMP_BUFFER_SIZE];
    this->imuData->timeBetweenSamples = TIME_BETWEEN_IMU_SAMPLES;

    this->inertialSensor = new IMUManager (); 
    this->memory = new EEPROMManager ();

    this->currentState =  new CalibratingInertialSensor (this);
    //this->currentState =  new SavingMessage (this);
    //this->currentState =  new LoadingMessage (this);

    this->LoRaTransciever = new LoRaClass ();
    this->LoRaTransciever->setSpreadingFactor(12);   // ranges from 6-12,default 7
    this->LoRaTransciever->setSyncWord(0xF3);  // ranges from 0-0xFF, default 0x34,
    this->LoRaTransciever->setSignalBandwidth(125E3); // 125 kHz

     //this->encrypter = new Encrypter ();
    this->encrypterBase64 = new EncrypterBase64 ();
    this->authgen = new AuthenticationGenerator ();
    this->ckgen = new ChecksumGenerator ();
    

    this->checksumVerifier = new ChecksumVerifier ();
    this->authVer = new AuthenticationVerifier ();
     //this->decrypter = new Decrypter ();
    this->decrypterBase64 = new DecrypterBase64 ();

    this->jwt = new JWTManager ();

    /// eliminate this lines on production
   while (! this->memory->clearAll()) {
        
    }
    //snprintf(StringToSendUSB, sizeof(StringToSendUSB), "STRINGS LIMPIADAS\n\r");
    //uartUSB.write(StringToSendUSB, strlen(StringToSendUSB));
}

Tracker::~Tracker() {
    delete [] this->serverTargetted->url; 
    this->serverTargetted->url  = nullptr;
    delete this->serverTargetted; 
    this->serverTargetted = nullptr;
    delete this->latency;

    delete [] this->deviceIdentifier;
    this->deviceIdentifier  = nullptr;
    delete [] this->prevChainHash;
    this->prevChainHash = nullptr;
    delete []  this->currChainHash;
    this->currChainHash = nullptr;

    delete this->imuData->timestamp;
    this->imuData->timestamp = nullptr;
    delete this->imuData;
    this->imuData = nullptr;

    delete[] this->currentCellInformation->timestamp;
    this->currentCellInformation->timestamp = nullptr;
    delete[] this->currentCellInformation->band;
    this->currentCellInformation->band = nullptr;
    delete this->currentCellInformation;
    this->currentCellInformation = nullptr;
    delete this->currentGNSSdata;

    this->latency = nullptr; 
    delete this->silentKeepAliveTimer;
    this->silentKeepAliveTimer = nullptr;
    delete this->currentGNSSModule;
    this->currentGNSSModule = nullptr;
    delete this->cellularTransceiver;
    this->cellularTransceiver = nullptr;

    delete this->inertialSensor;
    this->inertialSensor = nullptr;
    delete this->memory;
    this->memory = nullptr;

    delete this->LoRaTransciever;
    this->LoRaTransciever  = nullptr;

    //delete this->encrypter;
    //this->encrypter = nullptr;
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
    //delete this->decrypter;
    //this->decrypter = nullptr;
    delete this->decrypterBase64;
    this->decrypterBase64 = nullptr;


    delete this->jwt;
    this->jwt = nullptr;

}

void Tracker::update () {
    static char formattedMessage [MESSAGE_BUFFER_SIZE];
    static char receivedMessage [MESSAGE_LORA_RECEPTION_BUFFER_SIZE];

    static int numberOfNeighbors = 0;
    Watchdog &watchdog = Watchdog::get_instance(); // singleton
    watchdog.kick();
    this->currentState->awake(this->cellularTransceiver, this->latency, this->silentKeepAliveTimer);
    this->currentState->calibrateIMU (this->inertialSensor);
    this->currentState->updatePowerStatus (this->cellularTransceiver, this->batteryStatus);
    this->currentState->obtainGNSSPosition (this->currentGNSSModule, this->currentGNSSdata);
    this->currentState->connectToMobileNetwork (this->cellularTransceiver,
    this->currentCellInformation);
    this->currentState->obtainNeighborCellsInformation (this->cellularTransceiver, 
    this->neighborsCellInformation, numberOfNeighbors );
    this->currentState->checkStabillity(this->inertialSensor, &this->newMotionStatus);
    this->currentState->obtainInertialMeasures(this->inertialSensor, this->imuData, this->IMUDataSamples);
    this->currentState->formatMessage (formattedMessage, this->currentCellInformation,
    this->currentGNSSdata, this->neighborsCellInformation, this->imuData, this->IMUDataSamples, this->batteryStatus); 
    this->currentState->exchangeMessages (this->cellularTransceiver,
    formattedMessage, this->serverTargetted, receivedMessage );
    this->currentState->exchangeMessages (this->LoRaTransciever, formattedMessage, receivedMessage);
    this->currentState->saveMessage(this->memory, formattedMessage);
    this->currentState->loadMessage(this->memory, this->currentCellInformation, this->currentGNSSdata,
     this->neighborsCellInformation, this->imuData, this->IMUDataSamples, this->batteryStatus, formattedMessage);
    this->currentState->goToSleep (this->cellularTransceiver);
    watchdog.kick();
    
}


void Tracker::changeState  (TrackerState * newTrackerState) {
    delete this->currentState;
    this->currentState = newTrackerState;
}



void Tracker::getMovementEvent (char * movementEventString) {
    if ( movementEventString == nullptr) {
        return;
    }
    if (this->currentMovementEvent == MOVING) {
        strcpy (movementEventString, MOVEMENT_EVENT_STRING_MOVING);
    }
    if (this->currentMovementEvent == PARKING) {
        strcpy (movementEventString, MOVEMENT_EVENT_STRING_PARKING);
    }
    if (this->currentMovementEvent == STOPPED) {
        strcpy (movementEventString, MOVEMENT_EVENT_STRING_STOPPED);
    }
    if (this->currentMovementEvent == MOVEMENT_RESTARTED) {
        strcpy (movementEventString, MOVEMENT_EVENT_STRING_MOVEMENT_RESTARTED);
    }
}

void Tracker::setMovementEvent (char * movementEventString) {
    if (movementEventString == nullptr) {
        return;
    }
    if (strcmp (movementEventString, MOVEMENT_EVENT_STRING_MOVING) == 0) {
        this->currentMovementEvent = MOVING;
        return;
    }
    if (strcmp (movementEventString, MOVEMENT_EVENT_STRING_PARKING) == 0) {
        this->currentMovementEvent = PARKING;
        return;
    }
    if (strcmp (movementEventString, MOVEMENT_EVENT_STRING_STOPPED) == 0) {
        this->currentMovementEvent = STOPPED;
        return;
    }
    if (strcmp (movementEventString, MOVEMENT_EVENT_STRING_MOVEMENT_RESTARTED) == 0) {
        this->currentMovementEvent = MOVEMENT_RESTARTED;
        return;
    }
}



MovementEvent_t Tracker::getMovementEvent () {
    return this->currentMovementEvent;
}


OperationMode_t  Tracker::getOperationMode () {
    return this->currentOperationMode;
}

void Tracker::updateMovementEvent () {
    MovementEvent_t newMovementEvent;

    if (this->newMotionStatus == DEVICE_ON_MOTION && this->currentMotionStatus == DEVICE_STATIONARY) {
        newMovementEvent = MOVEMENT_RESTARTED;
        if (newMovementEvent != this->currentMovementEvent) {
            uartUSB.write(LOG_MESSAGE_MOVEMENT_RESTARTED, LOG_MESSAGE_MOVEMENT_RESTARTED_LEN);
        }
        this->currentMovementEvent = newMovementEvent;
    }
    else if (this->newMotionStatus == DEVICE_STATIONARY && this->currentMotionStatus == DEVICE_ON_MOTION) {
        newMovementEvent = PARKING;
        if (newMovementEvent != this->currentMovementEvent) {
            uartUSB.write(LOG_MESSAGE_PARKING, LOG_MESSAGE_PARKING_LEN);
        }
        this->currentMovementEvent = newMovementEvent;
    }
    else if (this->newMotionStatus == DEVICE_ON_MOTION && this->currentMotionStatus == DEVICE_ON_MOTION) {
        newMovementEvent = MOVING;
        if (newMovementEvent != this->currentMovementEvent) {
            uartUSB.write(LOG_MESSAGE_MOVING, LOG_MESSAGE_MOVING_LEN);
        }
        this->currentMovementEvent = newMovementEvent;
    }
    else if (this->newMotionStatus == DEVICE_STATIONARY && this->currentMotionStatus == DEVICE_STATIONARY) {
        newMovementEvent = STOPPED;
        if (newMovementEvent != this->currentMovementEvent) {
            uartUSB.write(LOG_MESSAGE_STOPPED, LOG_MESSAGE_STOPPED_LEN);
        }
        this->currentMovementEvent = newMovementEvent;
    }

    this->currentMotionStatus = this->newMotionStatus;
}

void Tracker::setOperationMode(OperationMode_t newOperationMode) {
    this->currentOperationMode = newOperationMode;
}

void Tracker::setSilentTimer (int hours) {
    this->silentKeepAliveTimer->write(hours * HOUR_MS);
}

void Tracker::setLatency(LatencyLevel_t level) {
    tick_t newLatency = EXTREMELY_LOW_LATENCY_MS;

    switch (level) {
        case EXTREMELY_LOW_LATENCY:
            this->latencyLevel = EXTREMELY_LOW_LATENCY;
            newLatency = EXTREMELY_LOW_LATENCY_MS;
            break;
        case VERY_LOW_LATENCY:
            this->latencyLevel = VERY_LOW_LATENCY;
            newLatency = VERY_LOW_LATENCY_MS;
            break;
        case LOW_LATENCY:
            this->latencyLevel = LOW_LATENCY;
            newLatency = LOW_LATENCY_MS;
            break;
        case MEDIUM_LATENCY:
            this->latencyLevel = MEDIUM_LATENCY;
            newLatency = MEDIUM_LATENCY_MS;
            break;
        case HIGH_LATENCY:
            this->latencyLevel = HIGH_LATENCY;
            newLatency = HIGH_LATENCY_MS;
            break;
        case VERY_HIGH_LATENCY:
            this->latencyLevel = VERY_HIGH_LATENCY;
            newLatency = VERY_HIGH_LATENCY_MS;
            break;
        case EXTREMELY_HIGH_LATENCY:
            this->latencyLevel = EXTREMELY_HIGH_LATENCY;
            newLatency = EXTREMELY_HIGH_LATENCY_MS;
            break;
        default:
            break;
    }

    this->latency->write(newLatency);
    this->latency->restart();


    char buffer[LOG_MESSAGE_NEW_LATENCY_LEN + 20];
    snprintf(buffer, sizeof(buffer), "%s %llu ms\n\r",LOG_MESSAGE_NEW_LATENCY , newLatency);
    uartUSB.write(buffer, strlen(buffer));
}



void Tracker::actualizeKeepAliveLatency () {
tick_t newKeepAliveLatency = EXTREMELY_LOW_LATENCY_MS;

    switch (this->latencyLevel) {
        case EXTREMELY_LOW_LATENCY:
            newKeepAliveLatency = EXTREMELY_LOW_LATENCY_MS * EXTREMELY_LOW_LATENCY_KEEP_ALIVE_MULTIPLIER ;
            break;
        case VERY_LOW_LATENCY:
            newKeepAliveLatency = VERY_LOW_LATENCY_MS * VERY_LOW_LATENCY_KEEP_ALIVE_MULTIPLIER ;
            break;
        case LOW_LATENCY:
            newKeepAliveLatency = LOW_LATENCY_MS * VERY_LOW_LATENCY_KEEP_ALIVE_MULTIPLIER;
            break;
        case MEDIUM_LATENCY:
            newKeepAliveLatency = MEDIUM_LATENCY_MS * MEDIUM_LATENCY_KEEP_ALIVE_MULTIPLIER;
            break;
        case HIGH_LATENCY:
            newKeepAliveLatency = HIGH_LATENCY_MS * HIGH_LATENCY_KEEP_ALIVE_MULTIPLIER;
            break;
        case VERY_HIGH_LATENCY:
            newKeepAliveLatency = VERY_HIGH_LATENCY_MS * VERY_HIGH_LATENCY_KEEP_ALIVE_MULTIPLIER;
            break;
        case EXTREMELY_HIGH_LATENCY:
            newKeepAliveLatency = EXTREMELY_HIGH_LATENCY_MS * EXTREMELY_HIGH_LATENCY_KEEP_ALIVE_MULTIPLIER;
            break;
        default:
            break;
    }

    this->silentKeepAliveTimer->write(newKeepAliveLatency);
    this->silentKeepAliveTimer->restart();

    char buffer[100];
    snprintf(buffer, sizeof(buffer), "\n\rNew keep alive latency set: %llu ms\n\r", newKeepAliveLatency);
    uartUSB.write(buffer, strlen(buffer));
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
    this->encrypterBase64->setNextHandler(this->authgen)->setNextHandler(this->ckgen);
    if (this->encrypterBase64->handleMessage (message, messageSize) == MESSAGE_HANDLER_STATUS_PROCESSED) {
        return true;
    } else {
        return false;
    }
}

bool Tracker::processLoRaMessage (char * message, unsigned int messageSize) {
    this->checksumVerifier->setNextHandler(this->authVer)->setNextHandler(this->decrypterBase64);
    if (this->checksumVerifier->handleMessage (message, messageSize) == MESSAGE_HANDLER_STATUS_PROCESSED) {
        return true;
    } else {
        return false;
    }
}



int Tracker::getLoraMessageNumber () {
    return this->loraMessageNumber;
}

void Tracker::increaseLoraMessageNumber () {
    this->loraMessageNumber++;
    if (this->loraMessageNumber >= 2147483647) { // max int value
        this->loraMessageNumber = 1;
    }
    return;
}


bool Tracker::checkMessageIntegrity ( char* messageReceived) {
    char logMessage [LOG_MESSAGE_PAYLOAD_RECEIVED_PREFIX_LEN + 20];
    long long int deviceIdReceived;
    int messageNumberReceived; 
    char payloadReceived [ACK_BUFFER_SIZE];

    if (messageReceived == nullptr) {
        return false;
    }

    if (sscanf(messageReceived, "%lld,%d,%s", &deviceIdReceived, &messageNumberReceived, payloadReceived) == 3) {
        bool messageCorrect = false;
        uartUSB.write ("\r\n", strlen("\r\n"));
        snprintf(logMessage, sizeof(logMessage),  LOG_MESSAGE_DEVICE_ID_RECEIVED_PREFIX, deviceIdReceived);
        uartUSB.write(logMessage, strlen(logMessage));
        if (deviceIdReceived == this->currentCellInformation->IMEI) {
            uartUSB.write(LOG_MESSAGE_ACK_OK, LOG_MESSAGE_ACK_OK_LEN);
        } else {
            uartUSB.write(LOG_MESSAGE_ACK_INVALID, LOG_MESSAGE_ACK_INVALID_LEN);
            return false;
        }
        snprintf(logMessage, sizeof(logMessage), LOG_MESSAGE_MESSAGE_NUMBER_RECEIVED_PREFIX, messageNumberReceived);
        uartUSB.write(logMessage, strlen(logMessage));
        if (messageNumberReceived == this->loraMessageNumber) {
            uartUSB.write(LOG_MESSAGE_ACK_OK, LOG_MESSAGE_ACK_OK_LEN);
        } else {
            uartUSB.write(LOG_MESSAGE_ACK_INVALID, LOG_MESSAGE_ACK_INVALID_LEN);
            return false;
        }
        snprintf(logMessage, sizeof(logMessage), LOG_MESSAGE_PAYLOAD_RECEIVED_PREFIX, payloadReceived);
        uartUSB.write(logMessage, strlen(logMessage));
        if (strcmp (payloadReceived, ACK_VALID_1) == 0 || strcmp (payloadReceived, ACK_VALID_2) == 0 ||
         strcmp (payloadReceived, ACK_VALID_3) == 0 ) {
            uartUSB.write(LOG_MESSAGE_ACK_OK, LOG_MESSAGE_ACK_OK_LEN);
        } else {
            uartUSB.write(LOG_MESSAGE_ACK_INVALID, LOG_MESSAGE_ACK_INVALID_LEN);
            return false;
        }
        this->increaseLoraMessageNumber ();
        return true;
    } else {
        uartUSB.write(LOG_MESSAGE_ACK_INVALID, LOG_MESSAGE_ACK_INVALID_LEN);
        return false;
    }
 }


char* Tracker::getUrlPathChannel ( ) {
    return this->serverTargetted->url;
}

 
char* Tracker::getDeviceIdentifier ( ) {
    return this->deviceIdentifier;
}

int Tracker::getSequenceNumber () {
    return this->sequenceMessageNumber;
}


void Tracker::increaseSequenceNumber () {
    this->sequenceMessageNumber ++;
}

void Tracker::progressOnHashChain () {
    strcpy (this->prevChainHash, this->currChainHash);
}

void Tracker::setCurrentHashChain (const char * hashChain) {
    strcpy (this->currChainHash, hashChain );
}


char* Tracker::getPrevHashChain ( ) {
    return this->prevChainHash;
}



void Tracker::encodeJWT(char * payloadToJWT, char * jwtEncoded)  {
    this->jwt->encodeJWT (payloadToJWT, jwtEncoded);
}

bool Tracker::decodeJWT(char * jwtToDecode, char * payloadRetrived) {
    return this->jwt->decodeJWT(jwtToDecode, payloadRetrived);
}



//=====[Implementations of private methods]==================================