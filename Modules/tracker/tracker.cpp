//=====[Libraries]=============================================================

#include "tracker.h"
#include "GNSSModule.h"
#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "string.h"
#include "CellularModule.h"
#include "Debugger.h" // due to global usbUart


//=====[Declaration of private defines]========================================
#define LATENCY        5000
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
    this->latency = new NonBlockingDelay (LATENCY);
    this->cellularTransmitter = new CellularModule ( );
    this->currentGNSSModule = new GNSSModule (this->cellularTransmitter->getPowerManager()
    , this->cellularTransmitter->getATHandler());
    //both share the same power manager and ATHandler (uart)

    this->socketTargetted = new TcpSocket;
    this->socketTargetted->IpDirection = new char[16]; // 
    strcpy(this->socketTargetted->IpDirection, "186.19.62.251");
    this->socketTargetted->TcpPort = 123;

    this->currentCellInformation = new CellInformation;
    this->currentCellInformation->date  = new char [10];
    this->currentCellInformation->time  = new char [10];
    this->currentCellInformation->band = new char [20];
    this->currentCellInformation->lac = new char [10];
    this->currentCellInformation->cellId = new char [20];

    this->currentGNSSdata = new GNSSData;
    this->batteryStatus = new BatteryData;

}


tracker::~tracker() {
    delete[] this->currentCellInformation->date;
    this->currentCellInformation->date = NULL;
    delete[] this->currentCellInformation->time;
    this->currentCellInformation->time = NULL;
    delete[] this->currentCellInformation->band;
    this->currentCellInformation->band = NULL;
    delete[] this->currentCellInformation->lac;
    this->currentCellInformation->lac = NULL;
    delete[] this->currentCellInformation->cellId;
    this->currentCellInformation->cellId = NULL;
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
    delete this->cellularTransmitter;
    this->cellularTransmitter = NULL;
}


/** 
* @brief Main rutine of the tracker device
*   
*
*/
void tracker::update () {
    
    static char* formattedMessage;
    static char messagetry [200];


    static GNSSState_t GnssCurrentStatus;
    static bool GNSSAdquisitionSuccesful = false;
    static bool enableTransmission = false; 
    static bool transimissionSecuenceActive =  false;
    static bool messageFormatted = false;
    static bool batterySensed = false;


    this->cellularTransmitter->startStopUpdate();
    //this->currentGNSSModule->startStopUpdate();

    if (this->latency->read() && transimissionSecuenceActive == false) { // WRITE
        transimissionSecuenceActive = true;
        batterySensed = false;
    }

    if (batterySensed == false &&  transimissionSecuenceActive == true) {
        if (this->cellularTransmitter->measureBattery(this->batteryStatus)) {
           batterySensed = true;
            this->currentGNSSModule->enableGNSS();
        }
    }
    
    GnssCurrentStatus = this->currentGNSSModule->retrivGeopositioning(this->currentGNSSdata);
    if (GnssCurrentStatus == GNSS_STATE_CONNECTION_OBTAIN ) {
        GNSSAdquisitionSuccesful = true;
        enableTransmission = true;
        char StringToSendUSB [40] = "GNSS OBTAIN!!!!";
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    }
    if (GnssCurrentStatus == GNSS_STATE_CONNECTION_UNAVAILABLE ) {
        GNSSAdquisitionSuccesful = false;
        enableTransmission = true;
        char StringToSendUSB [40] = "GNSS UNAVAILABLE!!!!";
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    }
    
    if (enableTransmission == true ) {
        if (this->cellularTransmitter->connectToMobileNetwork(this->currentCellInformation)) {
            if (GNSSAdquisitionSuccesful == false) {
                if (messageFormatted == false) {
                    formattedMessage = this->formMessage(this->currentCellInformation);
                    messageFormatted = true;
                    uartUSB.write (formattedMessage , strlen (formattedMessage ));  // debug only
                    uartUSB.write ( "\r\n",  3 );  // debug only
                }
            } else {
                if (messageFormatted == false) {
                    formattedMessage = this->formMessage(this->currentCellInformation, this->currentGNSSdata);
                    messageFormatted = true;
                    uartUSB.write (formattedMessage , strlen (formattedMessage ));  // debug only
                    uartUSB.write ( "\r\n",  3 );  // debug only
                }

            }
            if (this->cellularTransmitter->sendMessage
            (formattedMessage, this->socketTargetted) == true) {
                enableTransmission = false;
                transimissionSecuenceActive = false;
                messageFormatted = false;
                this->latency->restart();
            }
        }
    }
}

//=====[Implementations of private methods]==================================
char* tracker::formMessage(GNSSData* GNSSInfo) {
    static char message[50]; 
    snprintf(message, sizeof(message), "%.6f,%.6f", GNSSInfo->latitude, GNSSInfo->longitude);
    return message;
}

char* tracker::formMessage(CellInformation* aCellInfo) {
    static char message[200]; 
    snprintf(message, sizeof(message), 
             "MN,MN,%s,%s,%d,%d,%.2f,%d,%d,%d,%s,%s,%s", 
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
             aCellInfo->time);
        
    return message;
}

char* tracker::formMessage(CellInformation* aCellInfo, GNSSData* GNSSInfo) {
    static char message[200]; 
    snprintf(message, sizeof(message), 
             "MN,GNSS,%.6f,%.6f,%.2f,%.2f,%.2f,%.2f,%s,%s,%d,%d,%.2f,%d,%d,%d,%s,%s,%s", 
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
            GNSSInfo->utc
            );
        
    return message;
}