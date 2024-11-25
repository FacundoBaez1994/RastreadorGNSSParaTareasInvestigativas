//=====[Libraries]=============================================================

#include "Tracker.h"
#include "Debugger.h" // due to global usbUart


//=====[Declaration of private defines]========================================
#define LATENCY        500
#define POWERCHANGEDURATION  700
#define MAX_TIMEOUT_COUNTER 5

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

    this->LoRaTransciever = new LoRaClass ();
    if (!this->LoRaTransciever->begin (915E6)) {
        uartUSB.write ("LoRa Module Failed to Start!", strlen ("LoRa Module Failed to Start"));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    }
    this->LoRaTransciever->setSpreadingFactor(12);   // ranges from 6-12,default 7
    this->LoRaTransciever->setSyncWord(0xF3);  // ranges from 0-0xFF, default 0x34,
    this->LoRaTransciever->setSignalBandwidth(125E3); // 125 kHz
    this->timeout = new NonBlockingDelay (LATENCY);

    this->RFState = new SendingMessage (this);

    /*
    Watchdog &watchdog = Watchdog::get_instance(); // singletom
    watchdog.start(TIMEOUT_MS);
    char StringToSendUSB [50] = "Tracker initialization";
    uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only

    
    this->cellularTransceiver = new CellularModule ( );
    this->currentGNSSModule = new GNSSModule (this->cellularTransceiver->getPowerManager()
    , this->cellularTransceiver->getATHandler());
    //both share the same power manager and ATHandler (uart)

    this->socketTargetted = new TcpSocket;
    this->socketTargetted->IpDirection = new char[16]; // 
    strcpy(this->socketTargetted->IpDirection, "186.19.62.251");
    this->socketTargetted->TcpPort = 123;

    this->currentCellInformation = new CellInformation;
    this->currentCellInformation->date  = new char [10];
    this->currentCellInformation->time  = new char [10];
    this->currentCellInformation->band = new char [20];

    this->currentGNSSdata = new GNSSData;
    this->batteryStatus = new BatteryData;
    */
}


Tracker::~Tracker() {
    /*
    delete[] this->currentCellInformation->date;
    this->currentCellInformation->date = NULL;
    delete[] this->currentCellInformation->time;
    this->currentCellInformation->time = NULL;
    delete[] this->currentCellInformation->band;
    this->currentCellInformation->band = NULL;
    delete this->currentCellInformation;
    this->currentCellInformation = NULL;
    delete this->currentGNSSdata;
    delete[] this->socketTargetted->IpDirection; // Libera la memoria asignada a IpDirection
    this->socketTargetted->IpDirection = NULL;
    delete this->socketTargetted; // Libera la memoria asignada al socketTargetted
    this->socketTargetted = NULL;
    delete this->latency;
    this->latency = NULL; 
    delete this->currentGNSSModule;
    this->currentGNSSModule = NULL;
    delete this->cellularTransceiver;
    this->cellularTransceiver = NULL;
    */
}


/** 
* @brief Main rutine of the tracker device
*   
*
*/
void Tracker::update () {
    int deviceIdReceived;
    int messageNumberReceived;
    char payload[50] = {0};
    char messageToSend[50];
    char ACKMessage [64];
    char logMessage[50];
    int static timeoutCounter = 0;

    int deviceId = 1;
    static int messageNumber = 0;
    static bool messageSent = false;
    static bool debugFlag = false;
    

    snprintf( messageToSend, sizeof(messageToSend), "%d,%d,hello", deviceId, messageNumber);

     if (this->timeout->read()) {
        this->RFState->sendMessage (this->LoRaTransciever,  messageToSend);
        timeoutCounter++;
        if (timeoutCounter > MAX_TIMEOUT_COUNTER) {
            timeoutCounter = 0;
            uartUSB.write ("Timeout for ACK\r\n", strlen ("Timeout for ACK\r\n"));  // debug only
            this->changeState ( new SendingMessage (this));
        }
    }


    if (this->RFState->getAcknowledgement (this->LoRaTransciever, ACKMessage) == true) {
       if (this->checkMessageIntegrity (messageToSend, ACKMessage))  {
           messageNumber++;
       }
    }
    
    if (this->timeout->read()) {
        uartUSB.write ("Timeout for ACK\r\n", strlen ("Timeout for ACK\r\n"));  // debug only
        this->changeState ( new SendingMessage (this));
    }
}


void Tracker::changeState  (RFTransicieverState * newState) {
    delete this->RFState;
    this->RFState = newState;
}

//=====[Implementations of private methods]==================================
 bool Tracker::checkMessageIntegrity ( char * messageSent, char *messageReceived) {
    char logMessage [60];
    int deviceId;
    int messageNumber; 
    char payload [60];
    int deviceIdReceived;
    int messageNumberReceived; 
    char payloadReceived [60];

    sscanf(messageSent, "%d,%d,%49s", &deviceId, &messageNumber, payload);

    if (sscanf(messageReceived, "%d,%d,%49s", &deviceIdReceived, &messageNumberReceived, payloadReceived) == 3) {
        bool messageCorrect = false;
        uartUSB.write ("\r\n", strlen("\r\n"));
        snprintf(logMessage, sizeof(logMessage), "Device ID Received: %d\r\n", deviceIdReceived);
        uartUSB.write(logMessage, strlen(logMessage));
        if (deviceIdReceived == deviceId) {
            uartUSB.write("OK\r\n", strlen("OK\r\n"));
        } else {
            uartUSB.write("ACK invalido\r\n", strlen("ACK invalido\r\n"));
            return false;
        }
        snprintf(logMessage, sizeof(logMessage), "Message Number Received: %d\r\n", messageNumberReceived);
        uartUSB.write(logMessage, strlen(logMessage));
        if (messageNumberReceived == messageNumber) {
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
        return true;
    } else {
        uartUSB.write("ACK invalido\r\n", strlen("ACK invalido\r\n"));
        return false;
    }
 }


void Tracker::LoRa_rxMode(){
  LoRa.enableInvertIQ();                // active invert I and Q signals
  //LoRa.receive();                       // set receive mode
}

void Tracker::LoRa_txMode(){
  LoRa.idle();                          // set standby mode
  LoRa.disableInvertIQ();               // normal mode
}



char* Tracker::formMessage(GNSSData* GNSSInfo) {
    static char message[50]; 
    snprintf(message, sizeof(message), "%.6f,%.6f", GNSSInfo->latitude,
     GNSSInfo->longitude);
    return message;
}

char* Tracker::formMessage(CellInformation* aCellInfo, std::vector<CellInformation*> 
&neighborsCellInformation, BatteryData  * batteryStatus) {
    static char message[500];
    char neighbors[50];
    int lac;
    int idCell;
    int tech;
    int mcc;
    int mnc;
    snprintf(message, sizeof(message), 
            "MN,MN,%X,%X,%d,%d,%.2f,%d,%d,%d,%s,%s,%s,%d,%d", 
            aCellInfo->lac,
            aCellInfo->cellId,
            aCellInfo->mcc,
            aCellInfo->mnc,
            aCellInfo->signalLevel,
            aCellInfo->accessTechnology,
            aCellInfo->registrationStatus,
            aCellInfo->channel,
            aCellInfo->band,
            aCellInfo->date,
            aCellInfo->time,
            batteryStatus->batteryChargeStatus,
            batteryStatus->chargeLevel
            );
    snprintf(neighbors, sizeof(neighbors),"size of vector %d", neighborsCellInformation.size()); 
    uartUSB.write (neighbors , strlen (neighbors ));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only        
    for (size_t i = 0; i < neighborsCellInformation.size(); ++i) {
        tech = neighborsCellInformation[i]->tech;
        mcc = neighborsCellInformation[i]->mcc;
        mnc = neighborsCellInformation[i]->mnc;
        lac = neighborsCellInformation[i]->lac;
        idCell = neighborsCellInformation[i]->cellId;
        snprintf(neighbors, sizeof(neighbors),",%dG,%d,%d,%X,%X",tech,mcc,mnc,lac, idCell); 
        uartUSB.write (neighbors , strlen (neighbors ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        // Concatenar el mensaje de la celda vecina al mensaje principal
        strncat(message, neighbors, sizeof(message) - strlen(message) - 1);
    }   
    return message;
}

char* Tracker::formMessage(CellInformation* aCellInfo, GNSSData* GNSSInfo, BatteryData  * batteryStatus) {
    static char message[200]; 
    snprintf(message, sizeof(message), 
             "MN,GNSS,%.6f,%.6f,%.2f,%.2f,%.2f,%.2f,%X,%X,%d,%d,%.2f,%d,%d,%d,%s,%s,%s,%d,%d", 
            GNSSInfo->latitude,
            GNSSInfo->longitude,
            GNSSInfo->hdop,
            GNSSInfo->altitude,
            GNSSInfo->cog,
            GNSSInfo->spkm,
            aCellInfo->lac,
            aCellInfo->cellId,
            aCellInfo->mcc,
            aCellInfo->mnc,
            aCellInfo->signalLevel,
            aCellInfo->accessTechnology,
            aCellInfo->registrationStatus,
            aCellInfo->channel,
            aCellInfo->band,
            GNSSInfo->date,
            GNSSInfo->utc,
            batteryStatus->batteryChargeStatus,
            batteryStatus->chargeLevel
            );
    return message;
}