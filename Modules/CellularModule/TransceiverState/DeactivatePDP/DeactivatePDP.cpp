//=====[Libraries]=============================================================

#include "DeactivatePDP.h"
#include "CellularModule.h" 
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================

DeactivatePDP::DeactivatePDP (CellularModule * mobileModule, bool transmissionWasASuccess ) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
    this->transmissionWasASuccess = transmissionWasASuccess;
}

DeactivatePDP::~DeactivatePDP () {
    this->mobileNetworkModule = nullptr;
}
void DeactivatePDP::enableTransceiver () {
    return;
}

CellularTransceiverStatus_t DeactivatePDP::exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable) {
    char StringToBeRead [20];
    char ExpectedResponse [15] = "OK";
    char StringToSendUSB [15] =  "DEACTIVATE PDP";
    char StringToBeSend [50];
    char ATcommand[] = "AT+QIDEACT="; 
    int connectID = 0; 
    int contextID = 1; 

    snprintf(StringToBeSend, sizeof(StringToBeSend), "%s%d", ATcommand, contextID );

    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToBeSend);
        this->readyToSend  = false;
        ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToBeSend  , strlen (StringToBeSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ////   ////   ////   ////   ////   ////   
    }

    if ( ATHandler->readATResponse ( StringToBeRead) == true) {
         ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         ////   ////   ////   ////   ////   ////

        if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
            ////   ////   ////   ////   ////   ////
            char StringToSendUSB [40] = "Cambiando de estado 5";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////     
            this->mobileNetworkModule->changeTransceiverState (new 
            TransceiverUnavailable (this->mobileNetworkModule));
            if (this->transmissionWasASuccess == true) {
                return CELLULAR_TRANSCEIVER_STATUS_SEND_OK;
            } else {
                return CELLULAR_TRANSCEIVER_STATUS_UNAVAIBLE_TO_SEND;
            }
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
    }

    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
}



//=====[Implementations of private functions]==================================
