//=====[Libraries]=============================================================

#include "tracker.h"
#include "mbed.h"
#include "non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "string.h"
#include "cellularModule.h"


//=====[Declaration of private defines]========================================
#define LATENCY        100
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
    this->latency = new nonBlockingDelay (LATENCY);
    this->cellularTransmitter = new cellularModule ( );
}
/** 
* @brief Main rutine of the tracker device
*   
*
*/
void tracker::update () {
    DigitalOut led(LED1);
    this->cellularTransmitter->startStopUpdate();
    
    if (this->latency->read()) { // WRITE
        led = !led;
    }
    this->cellularTransmitter->connectToMobileNetwork();
}




//=====[Implementations of private methods]==================================
