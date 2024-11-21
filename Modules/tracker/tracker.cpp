//=====[Libraries]=============================================================

#include "tracker.h"
#include "Debugger.h" // due to global usbUart


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
tracker::tracker () {

    this->LoRaTransciver = new LoRaClass ();
    if (!this->LoRaTransciver->begin (915E6)) {
        uartUSB.write ("LoRa Module Failed to Start!", strlen ("LoRa Module Failed to Start"));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    }
    this->timeout = new NonBlockingDelay (TIMEOUT_MS);

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


tracker::~tracker() {
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
void tracker::update () {
    char message[50];
    char buffer[64];
    char payload[50] = {0};
    int deviceId = 1;
    static int messageNumber = 0;
    static bool messageSent = false;
    static bool debugFlag = false;
    int deviceIdReceived;
    int messageNumberReceived;


    snprintf(message, sizeof(message), "%d,%d,hello", deviceId, messageNumber);


    if ( messageSent == false) {
        uartUSB.write ("Sending message:", strlen ("Sending message:"));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (message, strlen (message));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        this->LoRa_txMode ();
        this->LoRaTransciver->beginPacket();
        this->LoRaTransciver->write((uint8_t *)message, strlen(message));
        this->LoRaTransciver->endPacket();
        messageSent = true;
        timeout->restart();
    }   



     if (messageSent == true) {
        if (debugFlag == false) {
            uartUSB.write("Wating For ACK\r\n", strlen("Wating For ACK\r\n")); // Debug
            debugFlag = true; 
        }
        this->LoRa_rxMode ();
        int packetSize = this->LoRaTransciver->parsePacket();
        if (packetSize) {
            uartUSB.write("Packet Received!\r\n", strlen("Packet Received!\r\n")); // Debug

            int maxIterations = 100; // Límite para evitar un ciclo infinito
            int iterations = 0;

            // Leer los datos disponibles
            while (this->LoRaTransciver->available() > 0 && iterations < maxIterations) {
                ssize_t bytesRead = this->LoRaTransciver->read(reinterpret_cast<uint8_t*>(buffer), sizeof(buffer));
                if (bytesRead > 0) {
                    // Enviar los bytes leídos al puerto serie
                    uartUSB.write(buffer, bytesRead);
                }
                iterations++;
            }
            if (iterations >= maxIterations) {
                uartUSB.write("Warning: Exceeded max iterations\r\n", strlen("Warning: Exceeded max iterations\r\n"));
            }

            // ACK message Analizing
            if (sscanf(buffer, "%d,%d,%49s", &deviceIdReceived, &messageNumberReceived, payload) == 3) {
                uartUSB.write ("\r\n", strlen("\r\n"));
                snprintf(message, sizeof(message), "Device ID Received: %d\r\n", deviceIdReceived);
                uartUSB.write(message, strlen(message));
                if (deviceIdReceived == deviceId) {
                    uartUSB.write("OK\r\n", strlen("OK\r\n"));
                }

                snprintf(message, sizeof(message), "Message Number Received: %d\r\n", messageNumberReceived);
                uartUSB.write(message, strlen(message));
                if (messageNumberReceived == messageNumber) {
                    uartUSB.write("OK\r\n", strlen("OK\r\n"));
                }

                snprintf(message, sizeof(message), "Payload Received: %s\r\n", payload);
                uartUSB.write(message, strlen(message));
                if (strcmp (payload, "ACK") == 0) {
                    uartUSB.write("OK\r\n", strlen("OK\r\n"));
                }
            } else {
                uartUSB.write("Error parsing message.\r\n", strlen("Error parsing message.\r\n"));
            }
            // Leer el RSSI del paquete recibido
            int packetRSSI = this->LoRaTransciver->packetRssi();
            snprintf(message, sizeof(message), "packet RSSI: %d\r\n", packetRSSI);
            uartUSB.write(message, strlen(message));
            messageSent = false;
            messageNumber ++;
        }
    }

    // timeout
    if (this->timeout->read() && messageSent == true) {
        uartUSB.write ("Timeout for ACK\r\n", strlen ("Timeout for ACK\r\n"));  // debug only
        messageSent = false;
    }

}

//=====[Implementations of private methods]==================================
void tracker::LoRa_rxMode(){
  LoRa.enableInvertIQ();                // active invert I and Q signals
  //LoRa.receive();                       // set receive mode
}

void tracker::LoRa_txMode(){
  LoRa.idle();                          // set standby mode
  LoRa.disableInvertIQ();               // normal mode
}



char* tracker::formMessage(GNSSData* GNSSInfo) {
    static char message[50]; 
    snprintf(message, sizeof(message), "%.6f,%.6f", GNSSInfo->latitude,
     GNSSInfo->longitude);
    return message;
}

char* tracker::formMessage(CellInformation* aCellInfo, std::vector<CellInformation*> 
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

char* tracker::formMessage(CellInformation* aCellInfo, GNSSData* GNSSInfo, BatteryData  * batteryStatus) {
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