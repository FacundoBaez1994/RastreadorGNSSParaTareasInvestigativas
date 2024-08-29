//=====[Libraries]=============================================================

#include "tracker.h"
#include "Debugger.h" // due to global usbUart


//=====[Declaration of private defines]========================================
#define LATENCY        500
#define POWERCHANGEDURATION  700

#define WHO_AM_I_MPU9250 0x75 // Should return 0x71

#define ADO 0
#if ADO
#define MPU9250_ADDRESS 0x69<<1  // Device address when ADO = 1
#else
#define MPU9250_ADDRESS 0x68<<1  // Device address when ADO = 0
#endif  

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
    this->sensor = new InertialSensor ();
}


tracker::~tracker() {

}


/** 
* @brief Main rutine of the tracker device
*   
*
*/
void tracker::update () {
    char buffer [80];
    static bool sensorsReady =  false;
    static bool transmissionSecuenceActive = true;

    static bool a = false;
    static bool b = false;
    static bool c = false;
    static bool d = false;
    static bool e = false;
    

    if (this->latency->read() && transmissionSecuenceActive == false) { // WRITE
        transmissionSecuenceActive = true;

    }
/*
    if (a == false) {
         if (this->sensor->checkCommunicationWithModule () == true) {
        a = true;
         }
    }

    if (b == false && a == true) {
         if (this->sensor->resetMPU9250 () == true) {
        b = true;
        }
    }

    if (c == false && b == true) {
         if (this->sensor->calibrateMPU9250 () == true) {
        c = true;
        }
    }

    if (d == false && c == true) {
         if (this->sensor->initMPU9250 () == true) {
        d = true;
        }
    }
    if (e == false && d == true) {
         if (this->sensor->initAK8963 () == true) {
        e = true;
        }
    }
*/
    if ( sensorsReady == false){
        if (this->sensor->initializeSensors()){
             sensorsReady = true;
        }
    }

    if ( sensorsReady == true && transmissionSecuenceActive  == true) {
        this->sensor->readAccelData();
        this->sensor->readGyroData();
        this->sensor->readMagData();
        this->sensor->readTempData();
    }



}

//=====[Implementations of private methods]==================================

