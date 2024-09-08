//=====[Libraries]=============================================================

#include "tracker.h"
#include "Debugger.h" // due to global usbUart


//=====[Declaration of private defines]========================================
#define LATENCY     500
#define POWERCHANGEDURATION  700

#define WHO_AM_I_MPU9250 0x75 // Should return 0x71

#define ADO 0
#if ADO
#define MPU9250_ADDRESS 0x69<<1  // Device address when ADO = 1
#else
#define MPU9250_ADDRESS 0x68<<1  // Device address when ADO = 0
#endif  
#define INT_STATUS 0x3A

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
    this->t.start();
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
    static float deltat = 0.0f;                             // integration interval for both filter schemes
    static int lastUpdate = 0, firstUpdate = 0, Now = 0;    // used to calculate integration interval    
  


    if (this->latency->read() && transmissionSecuenceActive == false) { // WRITE
        transmissionSecuenceActive = true;

    }

    if ( sensorsReady == false){
        if (this->sensor->initializeSensors()){
             sensorsReady = true;
        }
    }

    Now =  chrono::duration_cast<chrono::microseconds>(this->t.elapsed_time()).count();
    deltat =  static_cast<float>((Now - lastUpdate)/1000000.0f) ; // set integration time by time elapsed since last filter update
    lastUpdate = Now;

    if ( sensorsReady == true && transmissionSecuenceActive  == true) {
        if(this->sensor->acquireData(deltat)) {
      
            snprintf(buffer, sizeof(buffer), "now = %d  \n\r", Now);
            uartUSB.write(buffer, strlen(buffer));

            snprintf(buffer, sizeof(buffer), "delta T = %f  \n\r", deltat);
            uartUSB.write(buffer, strlen(buffer));
            transmissionSecuenceActive = false;
        }
    }

}

//=====[Implementations of private methods]==================================

