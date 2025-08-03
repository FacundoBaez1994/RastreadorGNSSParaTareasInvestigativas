//=====[Libraries]=============================================================

#include "ConfigureSSL.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 30
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
ConfigureSSL::ConfigureSSL () {
    this->mobileNetworkModule = nullptr;
    this->readyToSend = true;
    this->Attempts = 0; 
    this->maxAttempts = MAXATTEMPTS; 
    this->currentStatus = SETTING_SSL_CONTEXT;
}

ConfigureSSL::ConfigureSSL (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
    this->Attempts = 0; 
    this->maxAttempts = MAXATTEMPTS; 
    this->currentStatus = SETTING_SSL_CONTEXT;
}


ConfigureSSL::~ConfigureSSL () {
    this->mobileNetworkModule = nullptr;
}

void ConfigureSSL::enableTransceiver () {
    return;
}

CellularTransceiverStatus_t ConfigureSSL::exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable) {
    char StringToBeRead [200];
    char ExpectedResponse [4] = "OK";

    char StringToSendUSB [20] =  "SETTING SSL";

    char StringToSend1 [50] = "AT+QHTTPCFG=\"sslctxid\",1";
    char StringToSend2 [50] = "AT+QSSLCFG=\"sslversion\",1,1";
    char StringToSend3 [50] = "AT+QSSLCFG=\"ciphersuite\",1,0x0005";
    char StringToSend4 [50] = "AT+QSSLCFG=\"seclevel\",1,0";

   
   switch (this->currentStatus) {
       case SETTING_SSL_CONTEXT:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend1); // AT+QHTTPCFG=\"contextid\",1";
                this->readyToSend  = false;
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                uartUSB.write (StringToSend1  , strlen (StringToSend1  ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////  
                refreshTime->write(25000);
                refreshTime->restart();
            }
                
            if ( ATHandler->readATResponse ( StringToBeRead) == true) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////

                if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                    ////   ////   ////   ////   ////   ////     
                    this->currentStatus = SETTING_SSL_VERSION;
                    this->readyToSend  = true;
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            }
            break;
        case SETTING_SSL_VERSION:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend2); // AT+QHTTPCFG=\"contextid\",1";
                this->readyToSend  = false;
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToSend2  , strlen (StringToSend2 ));  // debug only
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
                    this->currentStatus = SETTING_SSL_LEVEL;
                    this->readyToSend  = true;
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            }

            break;

        case SETTING_SSL_CIPHER_SUITE:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend3); // AT+QHTTPCFG=\"contextid\",1";
                this->readyToSend  = false;
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToSend3  , strlen (StringToSend3 ));  // debug only
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
                    this->currentStatus = SETTING_SSL_LEVEL;
                    this->readyToSend  = true;
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            }
            break;

        case SETTING_SSL_LEVEL:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend4); // AT+QHTTPCFG=\"contextid\",1";
                this->readyToSend  = false;
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToSend4  , strlen (StringToSend4 ));  // debug only
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
                    this->currentStatus = SETTING_SSL_CONTEXT;
                    this->readyToSend  = true;
                    this->mobileNetworkModule->changeTransceiverState(new PostHTTP(this->mobileNetworkModule));
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            }


            break;
            
        default:
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
   }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->Attempts++;
        if (this->Attempts >= this->maxAttempts) {
            return CELLULAR_TRANSCEIVER_STATUS_FAIL_TO_ACTIVATE_PDP;
        }
    }

    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
}



//=====[Implementations of private functions]==================================
