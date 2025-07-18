//=====[Libraries]=============================================================

#include "ExchangingMessages.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "GoingToSleep.h"

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============


//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of private methods]===================================
/** 
* @brief attachs the callback function to the ticker
*/


//=====[Implementations of public methods]===================================


/** 
* @brief
* 
* @param 
*/
ExchangingMessages::ExchangingMessages (Tracker * tracker, trackerStatus_t trackerStatus) {
    this->tracker = tracker;
    this->currentStatus = trackerStatus;
}

/** 
* @brief
* 
* @param 
*/
ExchangingMessages::~ExchangingMessages () {
    this->tracker = NULL;
}

void ExchangingMessages::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

void ExchangingMessages::exchangeMessages (CellularModule * cellularTransceiver,
    char * message, TcpSocket * socketTargetted, char * receivedMessage ){
    static CellularTransceiverStatus_t currentTransmitionStatus;
    static bool newDataAvailable = false;
    static bool enableTransceiver = false;
    static bool rebootTransceiver = false;
    char logMessage [50];
    
    if (this->currentStatus == TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK
     || this->currentStatus == TRACKER_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK) {
        if (enableTransceiver == false) {
            cellularTransceiver->enableTransceiver();
            enableTransceiver = true; 
        }
       currentTransmitionStatus = cellularTransceiver->exchangeMessages (message, socketTargetted,
       receivedMessage, &newDataAvailable);

        if (currentTransmitionStatus == CELLULAR_TRANSCEIVER_STATUS_SEND_OK) {
            snprintf(logMessage, sizeof(logMessage), "The message was send with success");
            uartUSB.write (logMessage , strlen (logMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}

        if (newDataAvailable == true) {

            snprintf(logMessage, sizeof(logMessage),"new Message received:");
            uartUSB.write (logMessage , strlen (logMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}

            snprintf(logMessage, sizeof(logMessage), "%s",  receivedMessage);
            uartUSB.write (logMessage , strlen (logMessage ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            newDataAvailable = false;
            enableTransceiver = false;
            this->tracker->changeState (new GoingToSleep (this->tracker));
            return;
            } else {
                snprintf(logMessage, sizeof(logMessage),"No Messages received:");
                uartUSB.write (logMessage , strlen (logMessage));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only}
                newDataAvailable = false;
                enableTransceiver = false;
                this->tracker->changeState (new GoingToSleep (this->tracker));
                return;
            }    
        }  else if (currentTransmitionStatus != CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND
        && currentTransmitionStatus != CELLULAR_TRANSCEIVER_STATUS_UNAVAIBLE) {

            snprintf(logMessage, sizeof(logMessage),"The message couldn't be sent");
            uartUSB.write (logMessage , strlen (logMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            rebootTransceiver = true;
        }
    } else {
        // try with LoRa
    }

    if (rebootTransceiver == true) {
        if (cellularTransceiver->reboot()) {
            // save message in memory..
            newDataAvailable = false;
            enableTransceiver = false;
            rebootTransceiver = false; 
            this->tracker->changeState (new GoingToSleep (this->tracker));
            return;
        }
    }
    return;
}
 
//=====[Implementations of private methods]==================================