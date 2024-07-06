//=====[Libraries]=============================================================

#include "tracker.h"
#include "mbed.h"
#include "non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "string.h"

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
    this->trackerState = TRACKER_STATE_OFF;
    this->powerChangeDurationtimer = new nonBlockingDelay (POWERCHANGEDURATION  );
    this->cellularModuleUART = new BufferedSerial  (PA_9, PA_10, 115200);
    this->turningPower = false;
}
/** 
* @brief Main rutine of the tracker device
*   
*
*/
void tracker::update () {
     // // // // MOVER TODO DE ACA, SE RE DEFINE EN CADA PASADA!!
    nonBlockingDelay delayLatency (LATENCY);
    char StringToSend [15] = "ATI\n";
    // UnbufferedSerial uartUsb(USBTX, USBRX, 115200 ); // debug only
    DigitalOut led(LED1);
    DigitalIn celullarModuleStatusInput (PA_8); 
    DigitalIn powerControlButtonInput (PA_1);
    DigitalOut powerKeyOutput (PB_4);
    DigitalOut powerDownOutput(PB_0);
    char receivedCharLocal;
    powerKeyOutput = OFF;
    powerDownOutput = ON; //ENABLE DE LA ALIMENTACION

    while (true) {
        if (powerControlButtonInput.read () == OFF  && this->turningPower == false ) {  
            this->turningPower = true;
            powerKeyOutput = ON;
            this->powerChangeDurationtimer->restart ();
        }  
        if (this->turningPower == true && this->powerChangeDurationtimer->read() )  {
            this->turningPower = false;
            powerKeyOutput = OFF;
        } 

        if (delayLatency.read()) { // WRITE
            led = !led;
            uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            this->cellularModuleUART->write (StringToSend, strlen (StringToSend));  // debug only
            this->cellularModuleUART->write ( "\r\n",  3 );  // debug only
        }
        if (this->cellularModuleUART->readable() ) { // READ
            this->cellularModuleUART->read(&receivedCharLocal, 1);
            char str[2] = "";
            sprintf (str, "%c", receivedCharLocal);
            uartUSB.write( str, strlen(str) ); // debug only
            uartUSB.write ( "\r\n",  3 ); // debug only
        }
    }
}




//=====[Implementations of private methods]==================================
