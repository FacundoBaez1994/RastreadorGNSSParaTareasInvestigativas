//=====[Libraries]=============================================================

#include "tracker.h"
#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "string.h"
#include "CellularModule.h"


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
}

/** 
* @brief Main rutine of the tracker device
*   
*
*/
void tracker::update () {
    char message [15] = "Hola Mundo!";
    GNSSData Geodata;
    char ipDirection [15] = "186.19.62.251";
    int tcpPort = 123;
    static bool enableGNSSAdquisition = false;
    static bool enableTransmission = false; 

    //this->cellularTransmitter->startStopUpdate();
    this->currentGNSSModule->startStopUpdate();

    if (this->latency->read() && enableGNSSAdquisition == false) { // WRITE
       enableGNSSAdquisition = true;
    }
    
    if (enableGNSSAdquisition == true) {
        this->currentGNSSModule->enableGNSS();
        if (this->currentGNSSModule->retrivGeopositioning(&Geodata)) {
            enableGNSSAdquisition = false;
        }
    }


/*
    
    if (enableTransmission == true ) {
        this->cellularTransmitter->connectToMobileNetwork();
        if (this->cellularTransmitter->sendMessage (message, ipDirection, tcpPort) == true) {
            enableTransmission = false;
        }
    }
*/
}

//=====[Implementations of private methods]==================================
