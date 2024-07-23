//=====[Libraries]=============================================================

#include "RetrievingTimeAndDate.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart 

//=====[Declaration of private defines]========================================

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
RetrievingTimeAndDate::RetrievingTimeAndDate (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->timeAndDateRetrived = false;
    this->readyToSend = true;
}


/** 
* @brief 
* 
* 
* @returns 
*/
RetrievingTimeAndDate::~RetrievingTimeAndDate () {
    this->mobileNetworkModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
void RetrievingTimeAndDate::connect (ATCommandHandler * ATHandler, NonBlockingDelay * refreshTime) {

    static char StringToBeRead [256];
    char ExpectedResponse [15] = "OK";
    char StringToSend [15] =  "AT+QLTS=2";

    char StringToSendUSB [40] = "RETRIVING TIME AND DATE ";

    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToSend);
        this->readyToSend = false;
        ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        refreshTime->restart();
        ////   ////   ////   ////   ////   //// 
    }

    if ( this->timeAndDateRetrived == false) {
        if ( ATHandler->readATResponse ( StringToBeRead) == true ) {
        
            ////   ////   ////   ////   ////   ////
            uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////
             refreshTime->restart();
           if (this->retrieveNetworkTime (StringToBeRead)) {
                this->timeAndDateRetrived = true;
            }
        } 
    } 

    if (this->timeAndDateRetrived  == true) {
        if  (ATHandler->readATResponse ( StringToBeRead) == true) {
            if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////     
                char StringToSendUSB [40] = "Cambiando de estado 6";
                uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////            
                this->mobileNetworkModule->changeConnectionState (new AttachingToPacketService (this->mobileNetworkModule) );
            }
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->timeAndDateRetrived = false;
    }

}

//=====[Implementations of private functions]==================================
bool RetrievingTimeAndDate::retrieveNetworkTime(char *response) {
    char StringToCompare[8] = "+QLTS: ";

    if (strncmp(response, StringToCompare, strlen(StringToCompare)) == 0) {
        char * DateAndTimePart = response + strlen(StringToCompare);
        strncpy(this->dateTimeAndTimeZoneString, DateAndTimePart, sizeof(this->dateTimeAndTimeZoneString) - 1);
        this->dateTimeAndTimeZoneString[sizeof(this->dateTimeAndTimeZoneString) - 1] = '\0';

        uartUSB.write("Time and Date: ", strlen("Time and Date: ")); // debug
        uartUSB.write(this->dateTimeAndTimeZoneString, strlen(this->dateTimeAndTimeZoneString)); // debug
        uartUSB.write("\r\n", 2); // debug
        return true;
    }

    return false;
}

