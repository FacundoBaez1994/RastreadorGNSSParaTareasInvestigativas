//=====[Libraries]=============================================================

#include "tracker.h"
#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "string.h"
#include "CellularModule.h"


//=====[Declaration of private defines]========================================
#define LATENCY        500
#define POWERCHANGEDURATION  700

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============
DigitalOut led(LED1);

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public methods]===================================
/** 
* @brief Contructor method creates a new trackerGPS instance ready to be used
*/
tracker::tracker () {
    this->latency = new NonBlockingDelay (LATENCY);
    this->cellularTransmitter = new CellularModule ( );
}

/** 
* @brief Main rutine of the tracker device
*   
*
*/
void tracker::update () {
    char message [15] = "Hola Mundo!";
    char ipDirection [15] = "186.19.62.251";
    int tcpPort = 123;
    static bool enableTransmission = false;

    this->cellularTransmitter->startStopUpdate();

    if (this->latency->read() && enableTransmission == false) { // WRITE
        led = !led;
        enableTransmission = true;
    }
    
    if (enableTransmission == true ) {
        this->cellularTransmitter->connectToMobileNetwork();
        if (this->cellularTransmitter->sendMessage (message, ipDirection, tcpPort) == true) {
            enableTransmission = false;
        }
    }

}

//=====[Implementations of private methods]==================================
