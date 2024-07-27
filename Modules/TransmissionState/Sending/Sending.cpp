//=====[Libraries]=============================================================

#include "Sending.h"
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
Sending::Sending () {
    this->mobileNetworkModule = NULL;
    this->readyToSend = true;
    this->transmissionEnable = false;
}


/** 
* @brief
* 
* @param 
*/
Sending::Sending (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
    this->transmissionEnable = false;
}


/** 
* @brief 
* 
* 
* @returns 
*/
Sending::~Sending () {
    this->mobileNetworkModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
void Sending::send(ATCommandHandler *ATHandler,
    NonBlockingDelay *refreshTime, char *message, char *ipDirection, int tcpPort) {
    char StringToBeSend[100];
    char StringToBeRead[100];
    char StringToBeSendUSB[] = "SENDING DATA"; 
    char ATcommand[] = "AT+QISEND=";
    int connectID = 0; // Puede ser entre 0 y 11
    char confirmationToSend[] = "\x1a";
    char confirmationChar = '>';
    char recievedChar;
    static int counter = 0;


    // Formatear la cadena final
    int result = snprintf(StringToBeSend, sizeof(StringToBeSend), "%s%d,%d", ATcommand, connectID, strlen(message));


    if ( this->transmissionEnable == true) {
        if (ATHandler->readChar(&recievedChar) ==  true) {
            if (recievedChar  == confirmationChar) {
                uartUSB.write("Sending message", strlen("Sending message"));  // debug only
                uartUSB.write("\r\n", 3);  // debug only
                ATHandler->sendATCommand(message);
                uartUSB.write(message, strlen(message));  // debug only
                uartUSB.write("\r\n", 3);  // debug only
                ATHandler->sendATCommand(confirmationToSend);
                this->transmissionEnable = false;  // Reiniciar la bandera después de enviar //CAMBIO
                this->readyToSend = false;
            }
        }
    }


    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToBeSend);
        uartUSB.write(StringToBeSendUSB, strlen(StringToBeSendUSB));  // debug only
        uartUSB.write("\r\n", 3);  // debug only
        uartUSB.write(StringToBeSend, strlen(StringToBeSend));  // debug only
        uartUSB.write("\r\n", 3);  // debug only
        refreshTime->restart();
        this->readyToSend = false;
        this->transmissionEnable = true;
    }

    if (refreshTime->read()) {
        counter++;
        if (counter > 5) {
            this->readyToSend = true;
            counter = 0;
            this->transmissionEnable = false;
        }
    }
}


//=====[Implementations of private functions]==================================
