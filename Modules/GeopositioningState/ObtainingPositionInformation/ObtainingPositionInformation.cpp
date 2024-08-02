//=====[Libraries]=============================================================

#include "ObtainingPositionInformation.h" 
#include "GNSSModule.h" //debido a declaracion adelantada
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
ObtainingPositionInformation::ObtainingPositionInformation () {
    this->currentGNSSModule = NULL;
    this->readyToSend = true;
}


/** 
* @brief
* 
* @param 
*/
ObtainingPositionInformation::ObtainingPositionInformation  (GNSSModule * aGNSSModule) {
    this->currentGNSSModule = aGNSSModule;
    this->readyToSend = true;
}


/** 
* @brief 
* 
* 
* @returns 
*/
ObtainingPositionInformation::~ObtainingPositionInformation  () {
    this->currentGNSSModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
bool ObtainingPositionInformation::retrivGeopositioning (char * message, ATCommandHandler * ATHandler,
     NonBlockingDelay * refreshTime)  {
 
    char StringToSend [15] = "AT+QGPSLOC?";
    char StringToBeRead [256];
    char ExpectedResponse [15] = "OK";
    char sessionNoTActive [20] = "+CME ERROR: 505";
    char StringToSendUSB [40] = "OBTAINING POSITION INFO";



    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToSend);
        this->readyToSend = false;
        ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ////   ////   ////   ////   ////   ////   
    }

    if ( ATHandler->readATResponse ( StringToBeRead) == true) {
         ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         ////   ////   ////   ////   ////   ///
        if (strcmp (StringToBeRead, ExpectedResponse) == 0 ) {
            ////   ////   ////   ////   ////   ////
            char StringToSendUSB [40] = "GNSS Locked!";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////            
            // this->currentGNSSModule->changeGeopositioningState (new .... (this->mobileNetworkModule));
        }
        if (strcmp (StringToBeRead, sessionNoTActive) == 0 ) {
            ////   ////   ////   ////   ////   ////
            char StringToSendUSB [40] = "GNSS NOT active";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////            
            this->currentGNSSModule->changeGeopositioningState (new TurningOnGNSS (this->currentGNSSModule));
        }

        


    }

    if (refreshTime->read()) {
        this->readyToSend = true;
    }
    //
    //
    //
    return false;
}



//=====[Implementations of private functions]==================================
