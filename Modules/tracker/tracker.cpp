//=====[Libraries]=============================================================

#include "tracker.h"
#include "mbed.h"
#include "non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "string.h"
#include "cellularModule.h"

//=====[Declaration of private defines]========================================
#define LATENCY        50
#define POWERCHANGEDURATION  700

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================
UnbufferedSerial uartUSB(USBTX, USBRX, 115200 ); // debug only

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
    this->bufferIndex = 0;
}
/** 
* @brief Main rutine of the tracker device
*   
*
*/
void tracker::update () {
    char StringToSend [15] = "ATI";
    char StringRead [100] = "";
    DigitalOut led(LED1);

    char receivedCharLocal;
    char buffer[100]; // Buffer to store the received string


    if (this->latency->read()) { // WRITE
        led = !led;

        // // // // // // // // 
        uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
        uartUSB.write ( "\r\n",  2 );  // debug only
        // // // // // // 

        this->cellularTransmitter->getUART()->write (StringToSend, strlen (StringToSend));  // debug only
        this->cellularTransmitter->getUART()->write ( "\r\n",  3 );  // debug only
    }

    this->cellularTransmitter->startStopUpdate();
    if (this->cellularTransmitter->getUART()->readable()) { // READ
        this->cellularTransmitter->getUART()->read(&receivedCharLocal, 1);
        if (receivedCharLocal == '\r' || receivedCharLocal == '\n') { 
            if (this->bufferIndex > 0) {
                buffer[bufferIndex] = '\0'; // Null-terminate the string
                uartUSB.write(buffer, strlen(buffer)); // debug only
                uartUSB.write("\r\n",  2); // debug only
                this->bufferIndex = 0; // Reset buffer index for the next string
                }
        } else {
            if (this->bufferIndex < sizeof(buffer) - 1) { // Ensure there is space in the buffer
                buffer[this->bufferIndex++] = receivedCharLocal;
            }
        }
    }
}




//=====[Implementations of private methods]==================================
