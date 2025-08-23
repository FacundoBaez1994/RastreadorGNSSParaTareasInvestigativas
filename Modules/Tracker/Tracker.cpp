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


//#define HOUR_MS  (1 * 60 * 60 * 1000)  // 1 hours
#define HOUR_MS  (3 * 60 * 1000)  // 3 min TEST ONLY

#define TIMEOUT_WATCHDOG_TIMER_MS     5000
#define POWERCHANGEDURATION  700
#define TIME_BETWEEN_IMU_SAMPLES 10 // 10 seconds

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public methods]===================================
Tracker::Tracker () {
    Watchdog &watchdog = Watchdog::get_instance(); // singletom
    watchdog.start(TIMEOUT_WATCHDOG_TIMER_MS);
    char StringToSendUSB [50] = "Tracker initialization";
    uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only

    this->currentOperationMode = NORMAL_OPERATION_MODE;
    //this->currentOperationMode = PERSUIT_OPERATION_MODE;
    this->latency = new NonBlockingDelay (EXTREMELY_LOW_LATENCY_MS);
    this->silentTimer = new NonBlockingDelay (HOUR_MS);
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
    this->imuData->timestamp = new char [20];
    this->imuData->timeBetweenSamples = TIME_BETWEEN_IMU_SAMPLES;

    this->inertialSensor = new IMUManager (); 
    this->memory = new EEPROMManager ();

    this->currentState =  new CalibratingInertialSensor (this);
    //this->currentState =  new SavingMessage (this);
    //this->currentState =  new LoadingMessage (this);

    this->LoRaTransciever = new LoRaClass ();

     //this->encrypter = new Encrypter ();
    this->encrypterBase64 = new EncrypterBase64 ();
    this->authgen = new AuthenticationGenerator ();
    this->ckgen = new ChecksumGenerator ();
    

    this->checksumVerifier = new ChecksumVerifier ();
    this->authVer = new AuthenticationVerifier ();
     //this->decrypter = new Decrypter ();
    this->decrypterBase64 = new DecrypterBase64 ();

    while (! this->memory->clearAll()) {
        
    }
    snprintf(StringToSendUSB, sizeof(StringToSendUSB), "STRINGS LIMPIADAS\n\r");
    uartUSB.write(StringToSendUSB, strlen(StringToSendUSB));
}

Tracker::~Tracker() {
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
    delete[] this->socketTargetted->IpDirection; // Libera la memoria asignada a IpDirection
    this->socketTargetted->IpDirection = nullptr;
    delete this->socketTargetted; // Libera la memoria asignada al socketTargetted
    this->socketTargetted = nullptr;
    delete this->latency;
    this->latency = nullptr; 
    delete this->silentTimer;
    this->silentTimer = nullptr;
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
}

void Tracker::update () {
    static char formattedMessage [2048];
    static char receivedMessage [2048];

    static int numberOfNeighbors = 0;
    Watchdog &watchdog = Watchdog::get_instance(); // singleton
    watchdog.kick();
    this->currentState->awake(this->cellularTransceiver, this->latency, this->silentTimer);
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
    formattedMessage, this->socketTargetted, receivedMessage );
    this->currentState->exchangeMessages (this->LoRaTransciever, formattedMessage, receivedMessage);
    this->currentState->saveMessage(this->memory, formattedMessage);
    this->currentState->loadMessage(this->memory, this->currentCellInformation, this->currentGNSSdata,
     this->neighborsCellInformation, this->imuData, this->IMUDataSamples, this->batteryStatus);
    this->currentState->goToSleep (this->cellularTransceiver);
    watchdog.kick();
    
}


void Tracker::changeState  (TrackerState * newTrackerState) {
    delete this->currentState;
    this->currentState = newTrackerState;
}


void Tracker::getMovementEvent (char * movementEventString) {
    if (this->currentMovementEvent == MOVING) {
        strcpy (movementEventString, "MOVE");
    }
    if (this->currentMovementEvent == PARKING) {
        strcpy (movementEventString, "PARK");
    }
    if (this->currentMovementEvent == STOPPED) {
        strcpy (movementEventString, "STOP");
    }
    if (this->currentMovementEvent == MOVEMENT_RESTARTED) {
        strcpy (movementEventString, "MVRS");
    }
}

void Tracker::setMovementEvent (char * movementEventString) {
    if (strcmp (movementEventString,"MOVE") == 0) {
        this->currentMovementEvent = MOVING;
        return;
    }
    if (strcmp (movementEventString,"PARK") == 0) {
        this->currentMovementEvent = PARKING;
        return;
    }
    if (strcmp (movementEventString,"STOP") == 0) {
        this->currentMovementEvent = STOPPED;
        return;
    }
    if (strcmp (movementEventString,"MVRS") == 0) {
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
    char buffer[100];
    MovementEvent_t newMovementEvent;

    if (this->newMotionStatus == DEVICE_ON_MOTION && this->currentMotionStatus == DEVICE_STATIONARY) {
        newMovementEvent = MOVEMENT_RESTARTED;
        if (newMovementEvent != this->currentMovementEvent) {
            snprintf(buffer, sizeof(buffer), "\n\rUpdate movement event: MOVEMENT_RESTARTED\n\r");
            uartUSB.write(buffer, strlen(buffer));
        }
        this->currentMovementEvent = newMovementEvent;
    }
    else if (this->newMotionStatus == DEVICE_STATIONARY && this->currentMotionStatus == DEVICE_ON_MOTION) {
        newMovementEvent = PARKING;
        if (newMovementEvent != this->currentMovementEvent) {
            snprintf(buffer, sizeof(buffer), "\n\rUpdate movement event: PARKING\n\r");
            uartUSB.write(buffer, strlen(buffer));
        }
        this->currentMovementEvent = newMovementEvent;
    }
    else if (this->newMotionStatus == DEVICE_ON_MOTION && this->currentMotionStatus == DEVICE_ON_MOTION) {
        newMovementEvent = MOVING;
        if (newMovementEvent != this->currentMovementEvent) {
            snprintf(buffer, sizeof(buffer), "\n\rUpdate movement event: MOVING\n\r");
            uartUSB.write(buffer, strlen(buffer));
        }
        this->currentMovementEvent = newMovementEvent;
    }
    else if (this->newMotionStatus == DEVICE_STATIONARY && this->currentMotionStatus == DEVICE_STATIONARY) {
        newMovementEvent = STOPPED;
        if (newMovementEvent != this->currentMovementEvent) {
            snprintf(buffer, sizeof(buffer), "\n\rUpdate movement event: STOPPED\n\r");
            uartUSB.write(buffer, strlen(buffer));
        }
        this->currentMovementEvent = newMovementEvent;
    }

    this->currentMotionStatus = this->newMotionStatus;
}

void Tracker::setOperationMode(OperationMode_t newOperationMode) {
    this->currentOperationMode = newOperationMode;
}

void Tracker::setSilentTimer (int hours) {
    this->silentTimer->write(hours * HOUR_MS);
}

void Tracker::setLatency(LatencyLevel_t level) {
    tick_t newLatency = EXTREMELY_LOW_LATENCY_MS;

    switch (level) {
        case EXTREMELY_LOW_LATENCY:
            newLatency = EXTREMELY_LOW_LATENCY_MS;
            break;
        case VERY_LOW_LATENCY:
            newLatency = VERY_LOW_LATENCY_MS;
            break;
        case LOW_LATENCY:
            newLatency = LOW_LATENCY_MS;
            break;
        case MEDIUM_LATENCY:
            newLatency = MEDIUM_LATENCY_MS;
            break;
        case HIGH_LATENCY:
            newLatency = HIGH_LATENCY_MS;
            break;
        case VERY_HIGH_LATENCY:
            newLatency = VERY_HIGH_LATENCY_MS;
            break;
        case EXTREMELY_HIGH_LATENCY:
            newLatency = EXTREMELY_HIGH_LATENCY_MS;
            break;
        default:
            break;
    }

    this->latency->write(newLatency);
    this->latency->restart();

    char buffer[100];
    snprintf(buffer, sizeof(buffer), "\n\rNew latency set: %llu ms\n\r", newLatency);
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


bool Tracker::checkMessageIntegrity ( char *messageReceived) {
    char logMessage [60];

    char payload [60];
    long long int deviceIdReceived;
    int messageNumberReceived; 
    char payloadReceived [60];

    if (sscanf(messageReceived, "%lld,%d,%s", &deviceIdReceived, &messageNumberReceived, payloadReceived) == 3) {
        bool messageCorrect = false;
        uartUSB.write ("\r\n", strlen("\r\n"));
        snprintf(logMessage, sizeof(logMessage), "Device ID Received: %lld\r\n", deviceIdReceived);
        uartUSB.write(logMessage, strlen(logMessage));
        if (deviceIdReceived == this->currentCellInformation->IMEI) {
            uartUSB.write("OK\r\n", strlen("OK\r\n"));
        } else {
            uartUSB.write("ACK invalido\r\n", strlen("ACK invalido\r\n"));
            return false;
        }
        snprintf(logMessage, sizeof(logMessage), "Message Number Received: %d\r\n", messageNumberReceived);
        uartUSB.write(logMessage, strlen(logMessage));
        if (messageNumberReceived == this->loraMessageNumber) {
            uartUSB.write("OK\r\n", strlen("OK\r\n"));
        } else {
            uartUSB.write("ACK invalido\r\n", strlen("ACK invalido\r\n"));
            return false;
        }
        snprintf(logMessage, sizeof(logMessage), "Payload Received: %s\r\n", payloadReceived);
        uartUSB.write(logMessage, strlen(logMessage));
        if (strcmp (payloadReceived, "ACK") == 0 || strcmp (payloadReceived, "ACK\r") == 0 ||
         strcmp (payloadReceived, "ACK\r\n") == 0 ) {
            uartUSB.write("OK\r\n", strlen("OK\r\n"));
        } else {
            uartUSB.write("ACK invalido\r\n", strlen("ACK invalido\r\n"));
            return false;
        }
        this->increaseLoraMessageNumber ();
        return true;
    } else {
        uartUSB.write("ACK invalido\r\n", strlen("ACK invalido\r\n"));
        return false;
    }
 }





//=====[Implementations of private methods]==================================