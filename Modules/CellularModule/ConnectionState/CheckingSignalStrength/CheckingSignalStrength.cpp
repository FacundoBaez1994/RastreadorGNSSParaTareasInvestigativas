//=====[Libraries]=============================================================

#include "CheckingSignalStrength.h"
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
CheckingSignalStrength::CheckingSignalStrength (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->ATFirstResponseRead  = false;
    this->readyToSend = true;
    this->signalLevelRetrived = false;
}


/** 
* @brief 
* 
* 
* @returns 
*/
CheckingSignalStrength::~CheckingSignalStrength () {
    this->mobileNetworkModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
bool CheckingSignalStrength::connect (ATCommandHandler * ATHandler, NonBlockingDelay * refreshTime,
CellInformation * currentCellInformation) {

    static char StringToBeRead [256];
    char ExpectedResponse [15] = "OK";
     char StringToSend [15] = "AT+CSQ";
    char StringToSendUSB [40] = "CHECKING NETWORK STATE";

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

    if ( this->signalLevelRetrived == false) {
        if ( ATHandler->readATResponse ( StringToBeRead) == true ) {
        
            ////   ////   ////   ////   ////   ////
            uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////
            this->ATFirstResponseRead = true;
             refreshTime->restart();
            if (this->checkExpectedResponse (StringToBeRead,  this->signalLevel )) {
                char msgStringSignalQuality [20]= "";
                sprintf (msgStringSignalQuality, "%.2f",  this->signalLevel );  
                char msg []  = "signal level: "; 
                uartUSB.write (msg,  strlen (msg) );  // debug only
                uartUSB.write (msgStringSignalQuality,  strlen (msgStringSignalQuality) );  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                this->signalLevelRetrived = true;
            }
        } 
     } 
  
    if (this->signalLevelRetrived == true) {
        if  (ATHandler->readATResponse ( StringToBeRead) == true) {
            if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////     
                char StringToSendUSB [40] = "Cambiando de estado 2";
                uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////            
                currentCellInformation->signalLevel = this->signalLevel;
                this->mobileNetworkModule->changeConnectionState (new ConsultingIMEI (this->mobileNetworkModule) );
                return false;
            }
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->signalLevelRetrived = false;
    }
    return false;
}



//=====[Implementations of private functions]==================================
bool CheckingSignalStrength::checkExpectedResponse(char *response, float &value) {
    char StringToCompare[15] = "+CSQ:";
    
    // Verificar si la respuesta comienza con "+CSQ:"
    if (strncmp(response, StringToCompare, strlen(StringToCompare)) == 0) {
        // La respuesta comienza con "+CSQ:"
        // Extraer la parte numérica después de "+CSQ:"
        char *numericPart = response + strlen(StringToCompare);
        
        // Convertir la parte numérica a float
        value = std::strtof(numericPart, nullptr);
        
        return true;
    } else {
        // La respuesta no comienza con "+CSQ:"
        return false;
    }
}
