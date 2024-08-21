//=====[Libraries]=============================================================

#include "tracker.h"
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

}


tracker::~tracker() {

}


/** 
* @brief Main rutine of the tracker device
*   
*
*/
void tracker::update () {
    


    static bool transimissionSecuenceActive =  false;


    if (this->latency->read() && transimissionSecuenceActive == false) { // WRITE
        transimissionSecuenceActive = true;

    }


}

//=====[Implementations of private methods]==================================

