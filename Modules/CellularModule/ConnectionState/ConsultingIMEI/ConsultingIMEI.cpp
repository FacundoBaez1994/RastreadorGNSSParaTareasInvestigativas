//=====[Libraries]=============================================================

#include "ConsultingIMEI.h"
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
ConsultingIMEI::ConsultingIMEI (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->ATFirstResponseRead  = false;
    this->readyToSend = true;
    this->IMEIRetrived = false;
}


/** 
* @brief 
* 
* 
* @returns 
*/
ConsultingIMEI::~ConsultingIMEI () {
    this->mobileNetworkModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
bool ConsultingIMEI::connect (ATCommandHandler * ATHandler, NonBlockingDelay * refreshTime,
CellInformation * currentCellInformation) {

    static char StringToBeRead [256];
    char ExpectedResponse [15] = "OK";
     char StringToSend [15] = "AT+CGSN";
    char StringToSendUSB [40] = "CONSULTING IMEI STATE";

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

 if ( this->IMEIRetrived == false) {
        if ( ATHandler->readATResponse ( StringToBeRead) == true ) {
        
            ////   ////   ////   ////   ////   ////
            uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////
           // this->ATFirstResponseRead = true;
             refreshTime->restart();
            if (this->RetrivIMEI (StringToBeRead,  this->IMEI )) {
                char msgStringSignalQuality [20]= "";
                sprintf (msgStringSignalQuality, "%lld",  this->IMEI );  
                char msg []  = "Module IMEI: "; 
                uartUSB.write (msg,  strlen (msg) );  // debug only
                uartUSB.write (msgStringSignalQuality,  strlen (msgStringSignalQuality) );  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                this->IMEIRetrived= true;
           }
        } 
    } 

    if (this->IMEIRetrived == true) {
        if  (ATHandler->readATResponse ( StringToBeRead) == true) {
            if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////     
                char StringToSendUSB [40] = "Cambiando de estado 3";
                uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   //// 
                currentCellInformation->IMEI = this->IMEI;      
                this->mobileNetworkModule->changeConnectionState (new ConsultingSIMCardStatus (this->mobileNetworkModule) );
                return false;
            }
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->IMEIRetrived= false;
    }
    return false;

}



//=====[Implementations of private functions]==================================
bool ConsultingIMEI::RetrivIMEI(char *response, long long int &value) {
    // Verificar si los primeros tres caracteres son dígitos
    for (int i = 0; i < 3; ++i) {
        if (!isdigit(response[i])) {
            return false; // Los primeros tres caracteres no son dígitos
        }
    }

    // Extraer el número de IMEI de la respuesta
    char imeiPart[16] = {0}; // Crear un buffer para almacenar el IMEI
    strncpy(imeiPart, response, 15); // Copiar los primeros 15 caracteres

    // Verificar si la longitud del IMEI es la esperada (15 dígitos)
    if (strlen(imeiPart) != 15) {
        return false; // Longitud de IMEI no es válida
    }

    // Convertir la parte del IMEI a un número entero largo
    value = std::atoll(imeiPart);
    
    return true; // Recuperación exitosa del IMEI
}