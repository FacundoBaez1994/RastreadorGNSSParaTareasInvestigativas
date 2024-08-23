//=====[Libraries]=============================================================

#include "tracker.h"
#include "Debugger.h" // due to global usbUart


//=====[Declaration of private defines]========================================
#define LATENCY        5000
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


    static bool transimissionSecuenceActive =  false;

/*
    if (this->latency->read() && transimissionSecuenceActive == false) { // WRITE
        transimissionSecuenceActive = true;

    }*/

    wait_us(5000000);
    // Read the WHO_AM_I register, this is a good test of communication
    uint8_t whoami = sensor->readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);  // Read WHO_AM_I register for MPU-9250
    sprintf(buffer, "I AM 0x%x\n\r", whoami);  // Format message with whoami value
    uartUSB.write(buffer, strlen(buffer));     // Send formatted message over UART

        if (whoami == 0x71) // WHO_AM_I should always be 0x68
        {  
            sprintf(buffer, "MPU9250 is online...\n\r");
            uartUSB.write(buffer, strlen(buffer));
            wait_us(1000000);
            
            this->sensor->resetMPU9250(); // Reset registers to default in preparation for device calibration
            this->sensor->calibrateMPU9250( ); // Calibrate gyro and accelerometers, load biases in bias registers  

            wait_us(2000000);
            
            this->sensor->initMPU9250(); 
            this->sensor->initAK8963();

            wait_us(2000000);
            this->sensor->readAccelData();
            wait_us(2000000);
            this->sensor->readGyroData();
            wait_us(2000000);
            this->sensor->readMagData();
            wait_us(2000000);
            this->sensor->readTempData();
            wait_us(2000000);
        }
        else {
            sprintf(buffer, "Could not connect to MPU9250: \n\r");
            uartUSB.write(buffer, strlen(buffer));
            sprintf(buffer, "%#x \n", whoami);
            uartUSB.write(buffer, strlen(buffer));
        }

}

//=====[Implementations of private methods]==================================

