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
#define LATENCY        2000
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
    this->currentCellInformation->currentOperator = new char [50];
    this->currentCellInformation->dateTimeAndTimeZoneString  = new char [35];
    this->currentCellInformation->band = new char [20];
    this->currentCellInformation->mcc = new char [4];
    this->currentCellInformation->mnc = new char [4];


    this->currentGNSSdata = new GNSSData;

}


tracker::~tracker() {
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
    
    char* formattedMessage;

    static bool enableGNSSAdquisition = false;
    static bool enableTransmission = false; 

    this->cellularTransmitter->startStopUpdate();
    this->currentGNSSModule->startStopUpdate();

    if (this->latency->read() && enableGNSSAdquisition == false
    && enableTransmission == false) { // WRITE
       enableGNSSAdquisition = true;
    }
    
    if (enableGNSSAdquisition == true) {
        this->currentGNSSModule->enableGNSS();
        if (this->currentGNSSModule->retrivGeopositioning(this->currentGNSSdata)) {
            enableGNSSAdquisition = false;
            enableTransmission = true;
            formattedMessage = this->formMessage(this->currentGNSSdata);
            uartUSB.write (formattedMessage , strlen (formattedMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
        }
    }
    
    if (enableTransmission == true ) {
        this->cellularTransmitter->connectToMobileNetwork();
        if (this->cellularTransmitter->sendMessage (formattedMessage, this->socketTargetted) == true) {
            enableTransmission = false;
            this->latency->restart();
        }
    }
}

//=====[Implementations of private methods]==================================
char* tracker::formMessage(GNSSData* GNSSInfo) {
    static char message[50]; // Asegúrate de que el tamaño sea suficiente para contener el mensaje
    snprintf(message, sizeof(message), "%.6f,%.6f", GNSSInfo->latitude, GNSSInfo->longitude);
    return message;
}
